# LearnOpenGL · Windows x64

这是原 C++ OpenGL 学习工程的可运行版本，包含全部原实验、模型与纹理。

## 运行

1. 完整解压 ZIP，保留 `LearnOpengl.exe`、DLL 和 `res` 目录的相对位置。
2. 双击 `LearnOpengl.exe`，默认进入木盒材质实验。
3. 左侧 Inspector 调整贴图、光照和后处理；点 `<-` 回到原实验菜单。

需要 Windows 10/11 x64 与支持 OpenGL 4.5 的显卡驱动。包内提供 app-local Visual C++ 运行库，不需要安装 Visual Studio、Python 或 Unity。无需管理员权限。它是学习与实验程序，操作参数时仍可能出现极端画面。

WASD / QE 移动相机，鼠标拖动旋转，方向键移动模型，滚轮调整视野，Shift 加速，Esc 退出。完整界面需要较宽的桌面视口；模型、材质、法线和后处理均保留原有选项。

界面配置写在 `%LOCALAPPDATA%/Liss/LearnOpenGL/`，不写入源码目录。若没有微软雅黑，会使用系统可用字体或内置默认字体；本包不重新分发 Windows 字体。

## 命令行与截图

程序从自身位置寻找 `res`，不依赖终端当前目录。相对截图输出路径仍以调用终端的目录为准。

```powershell
.\LearnOpengl.exe --menu
.\LearnOpengl.exe --preset cube-normal
.\LearnOpengl.exe --preset car
.\LearnOpengl.exe --hidden --preset cube --with-ui --capture output.png
.\LearnOpengl.exe --help
```

预设：`cube`、`cube-flat`、`cube-normal`、`character`、`car`、`sphere`、`grid-sphere`、`planet`、`rock`、`plane`、`normals`、`wireframe`、`grayscale`、`clear-color`、`texture-2d`。`--menu` 打开 Clear Color / Texture 2D / 3D Cube 原菜单。截图是原程序 GPU framebuffer，`--frames` 可指定等待帧数。

## 文件与来源

- `release-manifest.json` 记录版本、源码提交、每个分发文件的 SHA-256 和资源清单。
- `THIRD_PARTY_NOTICES.md` 与 `licenses/` 保留第三方说明；仓库没有新增统一许可证。
- 源码与构建脚本：https://github.com/heroliss/LearnOpengl
