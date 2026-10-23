"""Run the real packaged renderer from an independently extracted Unicode path."""
from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess
import zipfile


def sha256(path: Path) -> str:
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument('archive', type=Path)
    parser.add_argument('destination', type=Path, help='New verification folder; must not exist')
    args = parser.parse_args()
    archive, destination = args.archive.resolve(), args.destination.resolve()
    if destination.exists():
        raise SystemExit('Destination already exists; use a new verification directory.')
    destination.mkdir(parents=True)
    extracted = destination / '独立 解压目录'
    with zipfile.ZipFile(archive) as package:
        for name in package.namelist():
            target = (extracted / name).resolve()
            if not target.is_relative_to(extracted.resolve()):
                raise SystemExit('Archive entry escapes its extraction directory.')
        package.extractall(extracted)
    manifests = list(extracted.glob('*/release-manifest.json'))
    if len(manifests) != 1:
        raise SystemExit('Expected one release manifest.')
    root = manifests[0].parent
    manifest = json.loads(manifests[0].read_text(encoding='utf-8-sig'))
    for entry in manifest['files']:
        path = root / entry['path']
        if path.stat().st_size != entry['bytes'] or sha256(path) != entry['sha256']:
            raise SystemExit(f'Manifest mismatch: {path}')

    caller = destination / '无源码 调用目录'
    (caller / 'screenshots').mkdir(parents=True)
    logs = destination / 'logs'
    logs.mkdir()
    env = os.environ.copy()
    env['PATH'] = os.pathsep.join([str(Path(env['WINDIR']) / 'System32'), env['WINDIR']])
    env['LOCALAPPDATA'] = str(destination / 'isolated-profile')
    presets = ['default', 'menu', 'cube', 'cube-flat', 'cube-normal', 'character', 'car',
               'sphere', 'grid-sphere', 'planet', 'rock', 'plane', 'normals', 'wireframe',
               'grayscale', 'clear-color', 'texture-2d']
    report = {
        'schema': 'liss.learnopengl.package-validation.v1',
        'archive': str(archive), 'sha256': sha256(archive),
        'packageRoot': str(root), 'callerWorkingDirectory': str(caller),
        'version': manifest['version'], 'sourceCommit': manifest['sourceCommit'],
        'sourceDirty': manifest['sourceDirty'],
        'verifiedManifestFiles': len(manifest['files']), 'presets': [],
        'limits': ['Real local GPU validation, not a clean Windows VM.',
                   'Menu screenshot plus original test classes via shared preset entry; no simulated mouse clicks.'],
    }
    errors = re.compile(r'Load texture failed:|ERROR::|\[OpenGL Error\]|Failed to (compile|link|initialize)', re.I)
    for preset in presets:
        arguments = ['--hidden', '--with-ui', '--capture', f'screenshots/{preset}.png']
        if preset == 'menu':
            arguments.append('--menu')
        elif preset != 'default':
            arguments += ['--preset', preset]
        completed = subprocess.run([str(root / 'LearnOpengl.exe'), *arguments], cwd=caller,
                                   env=env, capture_output=True, timeout=120,
                                   creationflags=subprocess.CREATE_NO_WINDOW)
        output = completed.stdout.decode('utf-8', errors='replace')
        output += completed.stderr.decode('utf-8', errors='replace')
        (logs / f'{preset}.log').write_text(output, encoding='utf-8')
        screenshot = caller / 'screenshots' / f'{preset}.png'
        dlls = re.findall(r'^Runtime DLL: (.+)$', output, re.M)
        local_dlls = len(dlls) == 3 and all(Path(dll.strip()).parent.resolve() == root for dll in dlls)
        passed = (completed.returncode == 0 and screenshot.is_file() and screenshot.stat().st_size > 1000
                  and not errors.search(output) and local_dlls
                  and f'Resource root: {root}' in output)
        item = {'name': preset, 'passed': bool(passed), 'exitCode': completed.returncode,
                'screenshot': str(screenshot), 'runtimeDlls': [dll.strip() for dll in dlls],
                'errorLines': [line for line in output.splitlines() if errors.search(line)]}
        if screenshot.is_file():
            item['screenshotSha256'] = sha256(screenshot)
        report['presets'].append(item)
        report['passed'] = all(check['passed'] for check in report['presets'])
        (destination / 'validation.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
        print(f'{preset}: {"PASS" if passed else "FAIL"}', flush=True)
    settings = destination / 'isolated-profile/Liss/LearnOpenGL/imgui.ini'
    report['isolatedSettingsCreated'] = settings.is_file()
    report['passed'] = report['passed'] and report['isolatedSettingsCreated']
    (destination / 'validation.json').write_text(json.dumps(report, ensure_ascii=False, indent=2), encoding='utf-8')
    if not report['passed']:
        raise SystemExit(1)


if __name__ == '__main__':
    main()
