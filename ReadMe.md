# 说明

在使用[MSYS2](https://www.msys2.org/)或者[Cygwin](https://cygwin.com/)的过程中希望在cmd.exe中直接使用其中的命令（如which、find、file等命令）。

为使用相关命令，可直接将MSYS2或者Cygwin添加至PATH变量，这样做的缺点是容易与其它软件冲突（如git for windows）,导致一些奇奇怪怪的错误（主要是由于某些dll版本不对）。本仓库就是主要解决软件冲突问题。

# 环境变量

本仓库通过环境变量调控程序行为。

| 环境变量名称                | 说明                                                       |
| --------------------------- | ---------------------------------------------------------- |
| WIN32REDIRECTOR_SEARCH_PATH | 搜索路径，多个路径通过；号分隔各个子路径。规则同PATH变量。 |



# 使用

- 使用默认路径安装好MSYS2或者Cygwin，并在MSYS2或者Cygwin安装好相应软件包。
- 单独准备一个目录，并将此目录添加至PATH变量。将本仓库的程序重命名为需要重定向的目标（如find.exe）,放入准备好的目录。

# 原理

通过程序名搜索某些路径，并通过spawnv执行。

