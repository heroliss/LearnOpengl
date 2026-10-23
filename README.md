# LearnOpengl

C++ OpenGL 学习与渲染实验工程。

## 内容

项目包含 GLFW 窗口、ImGui 测试菜单、Assimp 模型加载和多个渲染实验：OpenGL 4.5 Core 窗口、Cube/Sphere/Nanosuit 等模型切换、Skybox、光照、阴影、法线/深度/视差贴图，以及灰度、反色、卷积和 Tone Mapping 后处理。

资源位于 `res/`，源码主要位于 `src/`。

## 构建运行

1. Windows 安装 Visual Studio 2022 或更新版、Windows SDK 和「使用 C++ 的桌面开发」工作负载。
2. 执行 `git submodule update --init --recursive`，拉取 assimp、glfw、glm、imgui 和 stb。
3. 如果模型或纹理由 Git LFS 管理，执行 `git lfs pull`。
4. 运行 `powershell -ExecutionPolicy Bypass -File scripts/build.ps1`，或打开 `LearnOpengl.sln`，选择 x64 Debug 或 Release。工程采用当前 Visual Studio 的默认工具集，无需安装固定的 v143。
5. 从 Visual Studio 运行，工作目录保持为仓库根目录，以便读取 `res/`。

WASD/QE 移动相机，鼠标拖动旋转，方向键移动模型，滚轮调整视野，Shift 加速，Esc 退出；ImGui 菜单用于选择渲染实验和参数。

## 当前状态

保留原有渲染实验的 OpenGL 学习项目。2026-10-23 已在 Visual Studio 2026 / MSVC 14.51、Windows x64 Release 环境编译，并通过 NVIDIA OpenGL 4.5 实际绘制和 PNG 导出验证。Win32、Linux、macOS 及其他 GPU 未验证；现有静态库/Assimp DLL 为仓库提供的 Windows x64 依赖。

## 无需操作窗口的真实截图

从仓库根目录执行（需要可用的桌面 GPU 与 OpenGL 4.5 驱动）：

```powershell
./bin/Release/LearnOpengl.exe --hidden --preset cube --capture captures/cube.png
./bin/Release/LearnOpengl.exe --hidden --preset cube-flat --capture captures/flat.png
./bin/Release/LearnOpengl.exe --hidden --preset cube --with-ui --capture captures/inspector.png
```

截图读取原程序实际 OpenGL framebuffer，固定 1600×900，等待 12 帧后导出并退出；`--with-ui` 包含真实 ImGui 面板。`--preset` 只是设置现有实验参数，不另建渲染器。其他预设与参数可用 `--help` 查看。仅保留 `--preset cube` 可直接打开场景继续交互。

本轮还修复了未初始化的显示开关、无位移贴图的材质仍执行视差/自阴影，以及含空格目录的构建后复制命令。截图展示的是这些修复后的原工程。

## 目录

- `src/`：C++ 源码。
- `res/`：模型、纹理和渲染资源。
- `vendor/`：第三方依赖和子模块。
- `LearnOpengl.sln`、`LearnOpengl.vcxproj`：Visual Studio 工程。

## 许可证

仓库未声明统一许可证；第三方依赖按各自许可证使用。

