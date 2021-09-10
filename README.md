# gles2n64

This is a git mirror of the [gles2n64 video plugin](https://code.google.com/archive/p/gles2n64/) by lachlan, pulled from the Google Code Archive. This plugin hasn't been updated since 2011, and has generally been superceded by [GLideN64](https://github.com/gonetz/GLideN64).

## Original About

This project aims to port the Nintendo 64 graphics plugin glN64 to OpenGL ES 2.0 supporting devices (OMAP3, etc). Since most OGLES2 devices are constrained platforms, Performance will be a high priority.

Much of the core gln64 rendering has been rewritten to use native OpenGL ES 2.0, It emulates combiners with shaders and uses vertex arrays.

In its current state (14/12/09) it renders most games glN64 supports. Potentially it has greater compatibility (ie it supports PRIM_LOD combiners, etc).
