/*
gl-wes-v2:  OpenGL 2.0 to OGLESv2.0 wrapper
Contact:    lachlan.ts@gmail.com
Copyright (C) 2009  Lachlan Tychsen - Smith aka Adventus

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 3 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "wes.h"
#include "wes_begin.h"
#include "wes_shader.h"
#include "wes_matrix.h"

#if defined(_WIN32)
    #include <windows.h>
    #define dlopen(A, B)    LoadLibrary(A)
    #define dlsym(A, B)     GetProcAddress((HINSTANCE__*) A, B)
    #define dlclose(A)      FreeLibrary((HINSTANCE__*) A)
#else
    #include <dlfcn.h>
#endif

void            *wes_glhandle = NULL;
gles2lib_t      *wes_gl = NULL;
void            *wes_eglhandle = NULL;
egllib_t        *wes_egl = NULL;


const char * eglfuncnames[] =
{
    "eglGetError",
    "eglGetDisplay",
    "eglInitialize",
    "eglTerminate",
    "eglQueryString",
    "eglGetConfigs",
    "eglChooseConfig",
    "eglGetConfigAttrib",
    "eglCreateWindowSurface",
    "eglCreatePbufferSurface",
    "eglCreatePixmapSurface",
    "eglDestroySurface",
    "eglQuerySurface",
    "eglBindAPI",
    "eglQueryAPI",
    "eglWaitClient",
    "eglReleaseThread",
    "eglCreatePbufferFromClientBuffer",
    "eglSurfaceAttrib",
    "eglBindTexImage",
    "eglReleaseTexImage",
    "eglSwapInterval",
    "eglCreateContext",
    "eglDestroyContext",
    "eglMakeCurrent",
    "eglGetCurrentContext",
    "eglGetCurrentSurface",
    "eglGetCurrentDisplay",
    "eglQueryContext",
    "eglWaitGL",
    "eglWaitNative",
    "eglSwapBuffers",
    "eglCopyBuffers",
    "eglGetProcAddress"
};

//OpenGL ES 2 function names for runtime library loading:
const char *glfuncnames[] =
{
    "glActiveTexture",
    "glAttachShader",
    "glBindAttribLocation",
    "glBindBuffer",
    "glBindFramebuffer",
    "glBindRenderbuffer",
    "glBindTexture",
    "glBlendColor",
    "glBlendEquation",
    "glBlendEquationSeparate",
    "glBlendFunc",
    "glBlendFuncSeparate",
    "glBufferData",
    "glBufferSubData",
    "glCheckFramebufferStatus",
    "glClear",
    "glClearColor",
    "glClearDepthf",
    "glClearStencil",
    "glColorMask",
    "glCompileShader",
    "glCompressedTexImage2D",
    "glCompressedTexSubImage2D",
    "glCopyTexImage2D",
    "glCopyTexSubImage2D",
    "glCreateProgram",
    "glCreateShader",
    "glCullFace",
    "glDeleteBuffers",
    "glDeleteFramebuffers",
    "glDeleteTextures",
    "glDeleteProgram",
    "glDeleteRenderbuffers",
    "glDeleteShader",
    "glDetachShader",
    "glDepthFunc",
    "glDepthMask",
    "glDepthRangef",
    "glDisable",
    "glDisableVertexAttribArray",
    "glDrawArrays",
    "glDrawElements",
    "glEnable",
    "glEnableVertexAttribArray",
    "glFinish",
    "glFlush",
    "glFramebufferRenderbuffer",
    "glFramebufferTexture2D",
    "glFrontFace",
    "glGenBuffers",
    "glGenerateMipmap",
    "glGenFramebuffers",
    "glGenRenderbuffers",
    "glGenTextures",
    "glGetActiveAttrib",
    "glGetActiveUniform",
    "glGetAttachedShaders",
    "glGetAttribLocation",
    "glGetBooleanv",
    "glGetBufferParameteriv",
    "glGetError",
    "glGetFloatv",
    "glGetFramebufferAttachmentParameteriv",
    "glGetIntegerv",
    "glGetProgramiv",
    "glGetProgramInfoLog",
    "glGetRenderbufferParameteriv",
    "glGetShaderiv",
    "glGetShaderInfoLog",
    "glGetShaderPrecisionFormat",
    "glGetShaderSource",
    "glGetString",
    "glGetTexParameterfv",
    "glGetTexParameteriv",
    "glGetUniformfv",
    "glGetUniformiv",
    "glGetUniformLocation",
    "glGetVertexAttribfv",
    "glGetVertexAttribiv",
    "glGetVertexAttribPointerv",
    "glHint",
    "glIsBuffer",
    "glIsEnabled",
    "glIsFramebuffer",
    "glIsProgram",
    "glIsRenderbuffer",
    "glIsShader",
    "glIsTexture",
    "glLineWidth",
    "glLinkProgram",
    "glPixelStorei",
    "glPolygonOffset",
    "glReadPixels",
    "glReleaseShaderCompiler",
    "glRenderbufferStorage",
    "glSampleCoverage",
    "glScissor",
    "glShaderBinary",
    "glShaderSource",
    "glStencilFunc",
    "glStencilFuncSeparate",
    "glStencilMask",
    "glStencilMaskSeparate",
    "glStencilOp",
    "glStencilOpSeparate",
    "glTexImage2D",
    "glTexParameterf",
    "glTexParameterfv",
    "glTexParameteri",
    "glTexParameteriv",
    "glTexSubImage2D",
    "glUniform1f",
    "glUniform1fv",
    "glUniform1i",
    "glUniform1iv",
    "glUniform2f",
    "glUniform2fv",
    "glUniform2i",
    "glUniform2iv",
    "glUniform3f",
    "glUniform3fv",
    "glUniform3i",
    "glUniform3iv",
    "glUniform4f",
    "glUniform4fv",
    "glUniform4i",
    "glUniform4iv",
    "glUniformMatrix2fv",
    "glUniformMatrix3fv",
    "glUniformMatrix4fv",
    "glUseProgram",
    "glValidateProgram",
    "glVertexAttrib1f",
    "glVertexAttrib1fv",
    "glVertexAttrib2f",
    "glVertexAttrib2fv",
    "glVertexAttrib3f",
    "glVertexAttrib3fv",
    "glVertexAttrib4f",
    "glVertexAttrib4fv",
    "glVertexAttribPointer",
    "glViewport"
};

extern void OGL_CheckError();

GLvoid
wes_linklibrary(const char *gles2, const char *egl)
{
    int i;
    void** ptr;

    wes_gl = (gles2lib_t*) malloc(sizeof(gles2lib_t));
    if (wes_gl == NULL)
    {
        PRINT_ERROR("Could not load Allocate mem: %s", gles2);
    }

    wes_glhandle = dlopen(gles2, RTLD_LAZY | RTLD_GLOBAL);
    if (wes_glhandle == NULL)
    {
        PRINT_ERROR("Could not load OpenGL ES 2 runtime library: %s", gles2);
    }

    ptr = (void**) wes_gl;
    for(i = 0; i != WES_OGLESV2_FUNCTIONCOUNT+1; i++)
    {
        void* pfunc = (void*) dlsym(wes_glhandle, glfuncnames[i]);
        if (pfunc == NULL)
        {
            PRINT_ERROR("Could not find %s in %s", glfuncnames[i], gles2);
        }
        *ptr++ = pfunc;
    }

    //find EGL
    wes_egl = (egllib_t*) malloc(sizeof(egllib_t));
    if (wes_egl == NULL)
    {
        PRINT_ERROR("Could not load Allocate mem: %s", egl);
    }

    wes_eglhandle = dlopen(egl, RTLD_LAZY | RTLD_GLOBAL);
    if (wes_eglhandle == NULL)
    {
        PRINT_ERROR("Could not load EGL runtime library: %s", egl);
    }

    ptr = (void**) wes_egl;
    for(i = 0; i != WES_EGL_FUNCTIONCOUNT+1; i++)
    {
        void* pfunc = (void*) dlsym(wes_eglhandle, eglfuncnames[i]);
        if (pfunc == NULL)
        {
            PRINT_ERROR("Could not find %s in %s", eglfuncnames[i], egl);
        }
        *ptr++ = pfunc;
    }
}

GLvoid
wes_init()
{
    wes_shader_init();
    wes_matrix_init();
    wes_begin_init();
    wes_state_init();
}

GLvoid
wes_destroy()
{
    dlclose(wes_eglhandle);
    dlclose(wes_glhandle);
    wes_shader_destroy();
    wes_begin_destroy();
    free(wes_gl);
    free(wes_egl);
}


GLvoid
glMultiDrawArrays(GLenum mode, GLint *first, GLsizei *count, GLsizei primcount)
{
    GLuint i;
    for (i = 0; i < (unsigned)primcount; i++) {
        if (count[i] > 0){
            wes_gl->glDrawArrays(mode, first[i], count[i]);
        }
    }
}

GLvoid
glMultiDrawElements(GLenum mode, GLsizei *count, GLenum type, GLvoid **indices, GLsizei primcount)
{
    GLuint i;
    for (i = 0; i < (unsigned)primcount; i++) {
        if (count[i] > 0){
            wes_gl->glDrawElements(mode, count[i], type, indices[i]);
        }
    }
}
