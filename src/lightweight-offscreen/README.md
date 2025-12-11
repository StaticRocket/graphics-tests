# Lightweight Render Example

Heavily inspired by Eduardo Lima's
[gpu-playground](https://github.com/elima/gpu-playground/), this attempts to act
as the smallest demo of offscreen rendering.

This was also inspired by some of example code from Aaftab Munshi, Dan Ginsburg,
and Dave Shreiner's [OpenGLES 2.0 Programming Guide
](https://github.com/danginsburg/opengles3-book/blob/4ea81a7c8e4d447ce36a28a7a75b0fbbf11f484c/Chapter_2/Hello_Triangle/Hello_Triangle.c).

Both works are good reference and should be utilized for more advanced examples.

This project has adopted CC0 1.0 Universal so that others may use it as a
starting point without attribution.

## Checking the output

The application will currently attempt to render a 1920x1080 scene with
RGBA8888. This will be dumped directly to an output binary file. If you wish to
preview the file you can use the following imagemagick string to convert the
output to png:

```bash
magick -depth 8 -size 1920x1080+0 rgba:out.bin -flip new.png
```
