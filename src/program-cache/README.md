# Program Cache Example

This takes the `lightweight-offscreen` example and splits it into two
applications. The `program-cache-create` binary does a setup and stores the
program's data into a custom cache object on disk. The `program-cache-use` pulls
the cache object from disk and uses it for the standard render loop. This avoids
having to compile or link any shaders on subsequent use.

Note that this implementation is brittle on purpose. It's a starting point. See
the Khronos wiki for more information about issues with program caching:

<https://wikis.khronos.org/opengl/Shader_Compilation#Binary_upload>

## Checking the output

See `Checking the output` for the `lightweight-offscreen` example.
