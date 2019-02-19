# Actinidia Tools

## Image Concatenate

This tool sort some images by their name, then concatenate these images generating a long picture. It s a significant function so that you can make animation easily. For example, the lauching animation on Windows 8, just like this:

![](https://moooc.oss-cn-shenzhen.aliyuncs.com/blog/2015-05-04_162943.png)

You should use Flash or AfterEffect to export a series of animation frames. Then, use ImageConcatenate to connect them.

![](https://moooc.oss-cn-shenzhen.aliyuncs.com/blog/linker1.png)

![](https://moooc.oss-cn-shenzhen.aliyuncs.com/blog/linker2.png)

**Attention**

- Up to 100 images at a time 
- All images must have the same width
- Max image size: 65535\*65535

## Resource Packer/Unpacker

Pack resource folder to a single `.res` file. The resource file can be load by Actinidia Host. On the contrary, you can also extract the resource file.

## MapEditor

Build tile maps. Originally it is provided in prior version, but now it is now **removed** because of it's
awful implementation. Here's a [Screenshot](https://moooc.oss-cn-shenzhen.aliyuncs.com/blog/Actinidia_mapeditor_prev.png).

## Used libraries

- [libpng](http://www.libpng.org/pub/png/libpng.html)
- [libjpeg-turbo](https://www.libjpeg-turbo.org/)
- [zlib](https://www.zlib.net/)
