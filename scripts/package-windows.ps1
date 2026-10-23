param(
    [Parameter(Mandatory = $true)][string]$Destination,
    [ValidatePattern('^[A-Za-z0-9][A-Za-z0-9._-]*$')][string]$Version = '2026.10.24',
    [switch]$SkipBuild
)
$ErrorActionPreference = 'Stop'
$repoRoot = [IO.Path]::GetFullPath((Split-Path $PSScriptRoot -Parent))
$outputRoot = [IO.Path]::GetFullPath($Destination)
$packageName = "LearnOpenGL-$Version-win-x64"
$packageRoot = Join-Path $outputRoot $packageName
$zipPath = Join-Path $outputRoot "$packageName.zip"
if ((Test-Path -LiteralPath $packageRoot) -or (Test-Path -LiteralPath $zipPath)) {
    throw 'A package with this version already exists. Choose a new version or destination.'
}
if (!$SkipBuild) { & (Join-Path $PSScriptRoot 'build.ps1') -Configuration Release }
$executable = Join-Path $repoRoot 'bin/Release/LearnOpengl.exe'
$assimp = Join-Path $repoRoot 'bin/Release/assimp-vc143-mt.dll'
foreach ($path in @($executable, $assimp)) {
    if (!(Test-Path -LiteralPath $path -PathType Leaf)) { throw "Missing release input: $path" }
}

# Only use the unmodified, redistributable CRT from a stable installed Visual Studio.
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
$vsRoot = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (!$vsRoot) { throw 'A stable Visual Studio C++ installation with its redistributable CRT is required.' }
$redistRoot = Join-Path $vsRoot 'VC/Redist/MSVC'
$crtDirectory = Get-ChildItem -LiteralPath $redistRoot -Directory |
    Where-Object Name -Match '^\d+\.\d+\.\d+$' |
    Sort-Object { [version]$_.Name } -Descending |
    ForEach-Object { Get-ChildItem -LiteralPath (Join-Path $_.FullName 'x64') -Directory -Filter 'Microsoft.VC*.CRT' } |
    Select-Object -First 1
if (!$crtDirectory) { throw "The x64 redistributable CRT was not found under $redistRoot" }
$crtFiles = @(Get-ChildItem -LiteralPath $crtDirectory.FullName -Filter '*.dll' -File)
foreach ($required in @('msvcp140.dll', 'vcruntime140.dll', 'vcruntime140_1.dll')) {
    if ($required -notin $crtFiles.Name) { throw "Redistributable CRT is incomplete: $required" }
}
foreach ($dll in $crtFiles) {
    if ((Get-AuthenticodeSignature -LiteralPath $dll.FullName).Status -ne 'Valid') {
        throw "Redistributable CRT signature did not validate: $($dll.Name)"
    }
}

New-Item -ItemType Directory -Path $packageRoot -Force | Out-Null
Copy-Item -LiteralPath $executable, $assimp -Destination $packageRoot
Copy-Item -LiteralPath (Join-Path $repoRoot 'res') -Destination (Join-Path $packageRoot 'res') -Recurse
foreach ($dll in $crtFiles) { Copy-Item -LiteralPath $dll.FullName -Destination $packageRoot }
Copy-Item -LiteralPath (Join-Path $repoRoot 'docs/windows-release.md') -Destination (Join-Path $packageRoot 'README.md')
Copy-Item -LiteralPath (Join-Path $repoRoot 'THIRD_PARTY_NOTICES.md') -Destination $packageRoot
$licenseRoot = Join-Path $packageRoot 'licenses'
New-Item -ItemType Directory -Path $licenseRoot | Out-Null
$licenseFiles = [ordered]@{
    'Assimp.txt' = 'vendor/assimp/LICENSE'
    'GLFW.txt' = 'vendor/glfw/LICENSE.md'
    'GLM.txt' = 'vendor/glm/copying.txt'
    'Dear-ImGui.txt' = 'vendor/imgui/LICENSE.txt'
    'stb.txt' = 'vendor/stb/LICENSE'
    'GLAD.txt' = 'third-party/GLAD-LICENSE.txt'
    'Khronos-khrplatform.h.txt' = 'vendor/glad/include/KHR/khrplatform.h'
}
foreach ($name in $licenseFiles.Keys) {
    Copy-Item -LiteralPath (Join-Path $repoRoot $licenseFiles[$name]) -Destination (Join-Path $licenseRoot $name)
}

$sourceCommit = (& git -C $repoRoot rev-parse HEAD).Trim()
$sourceDirty = [bool](& git -C $repoRoot status --porcelain)
$files = @(Get-ChildItem -LiteralPath $packageRoot -File -Recurse | Sort-Object FullName | ForEach-Object {
    [ordered]@{
        path = $_.FullName.Substring($packageRoot.Length + 1).Replace('\', '/')
        bytes = $_.Length
        sha256 = (Get-FileHash -LiteralPath $_.FullName -Algorithm SHA256).Hash.ToLowerInvariant()
    }
})
$resources = @($files | Where-Object { $_.path.StartsWith('res/') })
$manifest = [ordered]@{
    schema = 'liss.learnopengl.windows-package.v1'
    version = $Version
    architecture = 'win-x64'
    generatedUtc = [DateTime]::UtcNow.ToString('o')
    sourceRepository = 'https://github.com/heroliss/LearnOpengl'
    sourceCommit = $sourceCommit
    sourceDirty = $sourceDirty
    requirements = @('Windows 10/11 x64', 'OpenGL 4.5-capable GPU and driver')
    appLocalCrtVersion = $crtDirectory.Parent.Parent.Name
    appLocalCrtDlls = @($crtFiles.Name)
    resourceFiles = $resources.Count
    resourceBytes = ($resources | Measure-Object bytes -Sum).Sum
    files = $files
}
$manifest | ConvertTo-Json -Depth 8 | Set-Content -LiteralPath (Join-Path $packageRoot 'release-manifest.json') -Encoding utf8

Add-Type -AssemblyName System.IO.Compression.FileSystem
[IO.Compression.ZipFile]::CreateFromDirectory($packageRoot, $zipPath, [IO.Compression.CompressionLevel]::Optimal, $true)
$zipHash = (Get-FileHash -LiteralPath $zipPath -Algorithm SHA256).Hash.ToLowerInvariant()
"$zipHash  $packageName.zip" | Set-Content -LiteralPath (Join-Path $outputRoot "$packageName.zip.sha256") -Encoding ascii
[ordered]@{
    packageDirectory = $packageRoot
    zip = $zipPath
    bytes = (Get-Item -LiteralPath $zipPath).Length
    sha256 = $zipHash
    manifest = Join-Path $packageRoot 'release-manifest.json'
} | ConvertTo-Json
