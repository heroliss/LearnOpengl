# Third-party components and example assets

This notice does not grant a new license to the LearnOpengl application or to its example assets. Original notices and third-party terms remain in force.

## Libraries

The Windows binary uses Assimp, GLFW, GLM, Dear ImGui, GLAD, Khronos OpenGL declarations, stb_image, and stb_image_write. The release includes their available notices in `licenses/`:

| Component | Notice / source |
| --- | --- |
| Assimp | `Assimp.txt`, including its bundled dependency notices; https://github.com/assimp/assimp |
| GLFW | `GLFW.txt`; https://github.com/glfw/glfw |
| GLM | `GLM.txt`; https://github.com/g-truc/glm |
| Dear ImGui | `Dear-ImGui.txt`; https://github.com/ocornut/imgui |
| GLAD 0.1.36 | `GLAD.txt`; https://github.com/Dav1dde/glad/tree/v0.1.36 |
| Khronos | `Khronos-khrplatform.h.txt`; original notice is preserved in the header |
| stb_image / stb_image_write | `stb.txt`; https://github.com/nothings/stb |

The repository's precompiled `assimp-vc143-mt.dll` is retained without modification. License files are copied as supplied by the corresponding local dependency checkout; they are not replaced with an application-wide license.

## Microsoft Visual C++ runtime

The package contains unmodified, Microsoft-signed x64 CRT DLLs copied from a stable Visual Studio 2026 installation's `VC/Redist/MSVC/<version>/x64/Microsoft.VC145.CRT` directory. They are application-local redistributable components, not part of this project's source license. Their version and hashes are recorded in the package manifest.

- Microsoft Visual Studio 2026 redistributable list: https://learn.microsoft.com/en-us/visualstudio/releases/2026/redistribution
- Microsoft Visual Studio Community 2026 terms: https://visualstudio.microsoft.com/license-terms/vs2026-ga-community/
- Application-local deployment: https://learn.microsoft.com/en-us/cpp/windows/walkthrough-deploying-a-visual-cpp-application-to-an-application-local-folder

Windows system libraries, graphics drivers and system fonts are not copied into the package.

## Example assets

The complete original `res/` tree is preserved so that the original model and material menus remain functional. Models, textures and material examples retain their original authorship and are not claimed as original assets made by this application author. The historical tree does not carry a complete asset-by-asset origin/license inventory; this notice does not invent or extend those rights.

The `Metal030` and `Metal046B` material files in the RS6 directory include ambientCG metadata; the `Metal030_4K-JPG.usda` file identifies ambientCG and CC0 1.0. See https://ambientcg.com/a/Metal030 and https://ambientcg.com/a/Metal046B for the material sources. This statement does not assign the same license to the car model or other assets.
