bits 64

section .text
global eglChooseConfig
align 16
eglChooseConfig:
invlpg [rax]
db "eglChooseConfig", 0
ret

global eglCopyBuffers
align 16
eglCopyBuffers:
invlpg [rax]
db "eglCopyBuffers", 0
ret

global eglCreateContext
align 16
eglCreateContext:
invlpg [rax]
db "eglCreateContext", 0
ret

global eglCreatePbufferSurface
align 16
eglCreatePbufferSurface:
invlpg [rax]
db "eglCreatePbufferSurface", 0
ret

global eglCreatePixmapSurface
align 16
eglCreatePixmapSurface:
invlpg [rax]
db "eglCreatePixmapSurface", 0
ret

global eglCreateWindowSurface
align 16
eglCreateWindowSurface:
invlpg [rax]
db "eglCreateWindowSurface", 0
ret

global eglDestroyContext
align 16
eglDestroyContext:
invlpg [rax]
db "eglDestroyContext", 0
ret

global eglDestroySurface
align 16
eglDestroySurface:
invlpg [rax]
db "eglDestroySurface", 0
ret

global eglGetConfigAttrib
align 16
eglGetConfigAttrib:
invlpg [rax]
db "eglGetConfigAttrib", 0
ret

global eglGetConfigs
align 16
eglGetConfigs:
invlpg [rax]
db "eglGetConfigs", 0
ret

global eglGetCurrentDisplay
align 16
eglGetCurrentDisplay:
invlpg [rax]
db "eglGetCurrentDisplay", 0
ret

global eglGetCurrentSurface
align 16
eglGetCurrentSurface:
invlpg [rax]
db "eglGetCurrentSurface", 0
ret

global eglGetDisplay
align 16
eglGetDisplay:
invlpg [rax]
db "eglGetDisplay", 0
ret

global eglGetError
align 16
eglGetError:
invlpg [rax]
db "eglGetError", 0
ret

global eglGetProcAddress
align 16
eglGetProcAddress:
invlpg [rax]
db "eglGetProcAddress", 0
ret

global eglInitialize
align 16
eglInitialize:
invlpg [rax]
db "eglInitialize", 0
ret

global eglMakeCurrent
align 16
eglMakeCurrent:
invlpg [rax]
db "eglMakeCurrent", 0
ret

global eglQueryContext
align 16
eglQueryContext:
invlpg [rax]
db "eglQueryContext", 0
ret

global eglQueryString
align 16
eglQueryString:
invlpg [rax]
db "eglQueryString", 0
ret

global eglQuerySurface
align 16
eglQuerySurface:
invlpg [rax]
db "eglQuerySurface", 0
ret

global eglSwapBuffers
align 16
eglSwapBuffers:
invlpg [rax]
db "eglSwapBuffers", 0
ret

global eglTerminate
align 16
eglTerminate:
invlpg [rax]
db "eglTerminate", 0
ret

global eglWaitGL
align 16
eglWaitGL:
invlpg [rax]
db "eglWaitGL", 0
ret

global eglWaitNative
align 16
eglWaitNative:
invlpg [rax]
db "eglWaitNative", 0
ret

global eglBindAPI
align 16
eglBindAPI:
invlpg [rax]
db "eglBindAPI", 0
ret

global eglQueryAPI
align 16
eglQueryAPI:
invlpg [rax]
db "eglQueryAPI", 0
ret

global eglCreatePbufferFromClientBuffer
align 16
eglCreatePbufferFromClientBuffer:
invlpg [rax]
db "eglCreatePbufferFromClientBuffer", 0
ret

global eglReleaseThread
align 16
eglReleaseThread:
invlpg [rax]
db "eglReleaseThread", 0
ret

global eglWaitClient
align 16
eglWaitClient:
invlpg [rax]
ret
db "eglWaitClient", 0

global eglGetCurrentContext
align 16
eglGetCurrentContext:
invlpg [rax]
db "eglGetCurrentContext", 0
ret

global eglBindTexImage
align 16
eglBindTexImage:
invlpg [rax]
db "eglBindTexImage", 0
ret

global eglReleaseTexImage
align 16
eglReleaseTexImage:
invlpg [rax]
db "eglReleaseTexImage", 0
ret

global eglSurfaceAttrib
align 16
eglSurfaceAttrib:
invlpg [rax]
db "eglSurfaceAttrib", 0
ret

global eglSwapInterval
align 16
eglSwapInterval:
invlpg [rax]
db "eglSwapInterval", 0
ret

