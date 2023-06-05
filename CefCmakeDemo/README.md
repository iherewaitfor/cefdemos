# CMakeDemo 
使用Cmake构建自己的cefsimple项目。
## How To Run
## 安装好Cmake
3.19以上版本。本文安装的是3.25.1版本。
## 下载依赖的cef_binay包
本项目使用的版本是cef_binary_97.1.9+ga00bca5+chromium-97.0.4692.99_windows32。
如果自己没有编译，可以直接下载官方编译好的二进制包。下载standard版本。
官方版本说明看这里[BranchesAndBuilding](https://bitbucket.org/chromiumembedded/cef/wiki/BranchesAndBuilding.md#markdown-header-current-release-branches-supported)。
本项目使用的windows系统的32位版本的下载链接 [https://cef-builds.spotifycdn.com/index.html#windows32](https://cef-builds.spotifycdn.com/index.html#windows32)。点击页面下方的All Builds->Show more builds，可以看到已构建的版本。
本项目使用的具体版本是[https://cef-builds.spotifycdn.com/cef_binary_97.1.9%2Bga00bca5%2Bchromium-97.0.4692.99_windows32.tar.bz2](https://cef-builds.spotifycdn.com/cef_binary_97.1.9%2Bga00bca5%2Bchromium-97.0.4692.99_windows32.tar.bz2)，选择Standard Distribution。

下载完后，解压好。比如解压到D:/develop/cef_binary_97.1.9+ga00bca5+chromium-97.0.4692.99_windows32
## 设置CEF_ROOT
直接源代码根目录中的CMakeLists.txt
将
```
#没有设置CEF_ROOT，就手动在这里设置cef的目录
	set(CEF_ROOT "D:/develop/cef_binary_97.1.9+ga00bca5+chromium-97.0.4692.99_windows32")
```
修改为自己解压的目录路径。

## 生成项目
在项目源码目录中新建build文件夹，然后命令行进入到build目录，
生成32位项目是运行命令cmake .. -G "Visual Studio 16 2019" -A Win32
```
D:\srccode\cefdemos\CefCmakeDemo\build>cmake .. -G "Visual Studio 16 2019" -A Win32
```
。然后打开生成的sln文件，将cefsimple项目设置为启动项即可。 cmake使用可参考本github项目[cmakevisualstudio](https://github.com/iherewaitfor/cmakevisualstudio)

如果你安装的是其他版本的Visual Studio，可以通过以下命令，查看对应的Visual Studio版本的命令。
```
cmake -G help
```

```
 Visual Studio 17 2022        = Generates Visual Studio 2022 project files.
                                 Use -A option to specify architecture.
  Visual Studio 16 2019        = Generates Visual Studio 2019 project files.
                                 Use -A option to specify architecture.
  Visual Studio 15 2017 [arch] = Generates Visual Studio 2017 project files.
                                 Optional [arch] can be "Win64" or "ARM".
  Visual Studio 14 2015 [arch] = Generates Visual Studio 2015 project files.
                                 Optional [arch] can be "Win64" or "ARM".
  Visual Studio 12 2013 [arch] = Generates Visual Studio 2013 project files.
                                 Optional [arch] can be "Win64" or "ARM".
  Visual Studio 11 2012 [arch] = Deprecated.  Generates Visual Studio 2012
                                 project files.  Optional [arch] can be
                                 "Win64" or "ARM".
  Visual Studio 9 2008 [arch]  = Generates Visual Studio 2008 project files
```

若需要使用Visual Studio 2017，需要构建win32项目，则可以将构建命令改成
```
cmake .. -G "Visual Studio 15 2017"
```

其他版本Visual Studeio命令可参考以下示例。
```
cmake .. -G "Visual Studio 17 2022" -A Win32
```

# 设置CEF_ROOT值
```
#
# CEF_ROOT setup.
# This variable must be set to locate the binary distribution.
# set(CEF_ROOT "D:/develop/cef_binary_97.1.9+ga00bca5+chromium-97.0.4692.99_windows32")

if(DEFINED ENV{CEF_ROOT})
	# 如果环境变量有CEF_ROOT，就设置环境变量值。
	set(CEF_ROOT $ENV{CEF_ROOT})
endif()
if(CEF_ROOT AND IS_DIRECTORY "${CEF_ROOT}")
	message(NOTICE "CEF_ROOT is ${CEF_ROOT}")
else()
	#没有设置CEF_ROOT，就手动在这里设置cef的目录
	set(CEF_ROOT "D:/develop/cef_binary_97.1.9+ga00bca5+chromium-97.0.4692.99_windows32")
	message(NOTICE "CEF_ROOT is empty. set to ${CEF_ROOT}")
endif()
```

## 读取环境变量
