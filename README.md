# LearnOpengl

C++ OpenGL 学习与渲染实验工程。

## 内容

项目包含 GLFW 窗口、ImGui 测试菜单、Assimp 模型加载和多个渲染实验：OpenGL 4.5 Core 窗口、Cube/Sphere/Nanosuit 等模型切换、Skybox、光照、阴影、法线/深度/视差贴图，以及灰度、反色、卷积和 Tone Mapping 后处理。

资源位于 `res/`，源码主要位于 `src/`。

## 构建运行

1. Windows 安装 Visual Studio 2022、Windows SDK 和 C++ 工具链。
2. 执行 `git submodule update --init --recursive`，拉取 assimp、glfw、glm、imgui 和 stb。
3. 如果模型或纹理由 Git LFS 管理，执行 `git lfs pull`。
4. 打开 `LearnOpengl.sln`，优先选择 x64 Debug 或 Release。
5. 从 Visual Studio 运行，工作目录保持为仓库根目录，以便读取 `res/`。

WASD/QE 移动相机，鼠标拖动旋转，方向键移动模型，滚轮调整视野，Shift 加速，Esc 退出；ImGui 菜单用于选择渲染实验和参数。

## 当前状态

已冻结的 OpenGL 学习项目，最后一次 push 为 2024-01-17。尚未在当前环境完整编译验证，x86 配置、子模块和 Git LFS 资源可能需要额外处理。

## 目录

- `src/`：C++ 源码。
- `res/`：模型、纹理和渲染资源。
- `vendor/`：第三方依赖和子模块。
- `LearnOpengl.sln`、`LearnOpengl.vcxproj`：Visual Studio 工程。

## 许可证

仓库未声明统一许可证；第三方依赖按各自许可证使用。

