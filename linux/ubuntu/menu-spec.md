#
- https://www.freedesktop.org/wiki/Specifications/desktop-entry-spec/
- https://www.freedesktop.org/wiki/Specifications/menu-spec/
- https://specifications.freedesktop.org/basedir-spec/basedir-spec-0.6.html
- https://gitlab.gnome.org/GNOME/alacarte

## desktop entry

The XDG Desktop Entry specification defines a standard for applications to integrate into application menus of desktop environments implementing the XDG Desktop Menuspecification.

一个 desktop entry 文件代表一个桌面环境中的一个菜单项，它的种类可以是应用、子菜单和链接。

对于应用和链接，文件扩展名为*.desktop，对于子菜单，文件扩展名为*.directory，虽然directory意为目录，但是却是作为子菜单的扩展名。

## 一些背景介绍
XDG指的是X Development Group, 这是Freedesktop.org的旧称，关于freedesktop的一些规定可以在https://specifications.freedesktop.org上找到。

## desktop 文件位置
先来看一下 XDG Base Directory Specification 定义的一些环境变量：

- $XDG_DATA_HOME - 用户数据存放的基准目录，如果没有设置，默认值为`$HOME/.local/share`
- $XDG_CONFIG_HOME - 用户配置存放的基准目录，默认为`$HOME/.config`
- $XDG_DATA_DIRS - 除了`$XDG_DATA_HOME`目录之外，一系列按优先级排序的基准目录，这些目录也是用于查找用户数据文件的。默认值为`/usr/local/share:/usr/share`
