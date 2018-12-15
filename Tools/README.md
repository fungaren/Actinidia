# Actinidia Tools

## Image Concatenate

Image Concatenate 的功能很简单：把图像序列按名称顺序排列后垂直连接起来，生成为一张长图。你可能会觉得这貌似并没什么用，
但它对于动画效果的实现其实是比较重要的。比如 Windows 8 的启动动画是这样实现的。

![](https://moooc.oss-cn-shenzhen.aliyuncs.com/blog/2015-05-04_162943.png)

首先你得使用 Flash 或者 AfterEffect 制作好一段动画，然后将所有帧保存为图像序列。接着，使用 Image Concatenate 把序列输出为长图，
比如把电影对白的截图连起来做成微博段子等等。也可以在制作游戏时免去逐张贴图的麻烦。

![](https://moooc.oss-cn-shenzhen.aliyuncs.com/blog/linker1.png)

![](https://moooc.oss-cn-shenzhen.aliyuncs.com/blog/linker2.png)

**注意事项**

1. 最多只能同时合并100张（否则会崩溃）
2. 如果图像是**不含透明通道**的 PNG 图片，请导出为 BMP 格式或 JPG 格式，否则保存的 PNG 一片空白；含有透明通道的 PNG 图片没有影响。

## Resource Packer/Unpacker

资源打包/解包工具。

### MapEditor

Build tile maps. Originally it is provided in prior version, but now it is now **removed** because of it's
awful implementation. Here's an [Screenshot](https://moooc.oss-cn-shenzhen.aliyuncs.com/blog/Actinidia_mapeditor_prev.png).

## Used libraries

- [libpng](http://www.libpng.org/pub/png/libpng.html)
- [libjpeg-turbo](https://www.libjpeg-turbo.org/)
- [zlib](https://www.zlib.net/)
