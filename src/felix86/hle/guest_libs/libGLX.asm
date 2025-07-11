; Generated by generator.cpp
; See guest_libs/README.md
bits 64

; These are guest function pointers that we wanna call from host code at will
section .data

align 16
libname:
db "libGLX.so", 0

guest_func_names:
XGetVisualInfo_name:
db "XGetVisualInfo", 0

XSync_name:
db "XSync", 0

malloc_name:
db "malloc", 0


section .text

extern __felix86_glXGetProcAddressSelf
extern XGetVisualInfo
extern XSync
extern malloc

global __felix86_constructor:function
align 16
__felix86_constructor:
invlpg [rbx]
ret
dd 0x12345678 ; invlpg + ret are 4 bytes, four more here to align to pointer
dq libname
; Here follows the null terminated list of {const char*, void*} (names, functions)
dq XGetVisualInfo_name
dq XGetVisualInfo
dq XSync_name
dq XSync
dq malloc_name
dq malloc
dq 0
dq 0

global glXChooseVisual:function
align 16
glXChooseVisual:
invlpg [rax]
db "glXChooseVisual", 0
ret

global glXCreateContext:function
align 16
glXCreateContext:
invlpg [rax]
db "glXCreateContext", 0
ret

global glXDestroyContext:function
align 16
glXDestroyContext:
invlpg [rax]
db "glXDestroyContext", 0
ret

global glXMakeCurrent:function
align 16
glXMakeCurrent:
invlpg [rax]
db "glXMakeCurrent", 0
ret

global glXCopyContext:function
align 16
glXCopyContext:
invlpg [rax]
db "glXCopyContext", 0
ret

global glXSwapBuffers:function
align 16
glXSwapBuffers:
invlpg [rax]
db "glXSwapBuffers", 0
ret

global glXCreateGLXPixmap:function
align 16
glXCreateGLXPixmap:
invlpg [rax]
db "glXCreateGLXPixmap", 0
ret

global glXDestroyGLXPixmap:function
align 16
glXDestroyGLXPixmap:
invlpg [rax]
db "glXDestroyGLXPixmap", 0
ret

global glXQueryExtension:function
align 16
glXQueryExtension:
invlpg [rax]
db "glXQueryExtension", 0
ret

global glXQueryVersion:function
align 16
glXQueryVersion:
invlpg [rax]
db "glXQueryVersion", 0
ret

global glXIsDirect:function
align 16
glXIsDirect:
invlpg [rax]
db "glXIsDirect", 0
ret

global glXGetConfig:function
align 16
glXGetConfig:
invlpg [rax]
db "glXGetConfig", 0
ret

global glXGetCurrentContext:function
align 16
glXGetCurrentContext:
invlpg [rax]
db "glXGetCurrentContext", 0
ret

global glXGetCurrentDrawable:function
align 16
glXGetCurrentDrawable:
invlpg [rax]
db "glXGetCurrentDrawable", 0
ret

global glXWaitGL:function
align 16
glXWaitGL:
invlpg [rax]
db "glXWaitGL", 0
ret

global glXWaitX:function
align 16
glXWaitX:
invlpg [rax]
db "glXWaitX", 0
ret

global glXUseXFont:function
align 16
glXUseXFont:
invlpg [rax]
db "glXUseXFont", 0
ret

global glXChooseFBConfig:function
align 16
glXChooseFBConfig:
invlpg [rax]
db "glXChooseFBConfig", 0
ret

global glXCreateNewContext:function
align 16
glXCreateNewContext:
invlpg [rax]
db "glXCreateNewContext", 0
ret

global glXCreatePbuffer:function
align 16
glXCreatePbuffer:
invlpg [rax]
db "glXCreatePbuffer", 0
ret

global glXCreatePixmap:function
align 16
glXCreatePixmap:
invlpg [rax]
db "glXCreatePixmap", 0
ret

global glXCreateWindow:function
align 16
glXCreateWindow:
invlpg [rax]
db "glXCreateWindow", 0
ret

global glXDestroyPbuffer:function
align 16
glXDestroyPbuffer:
invlpg [rax]
db "glXDestroyPbuffer", 0
ret

global glXDestroyPixmap:function
align 16
glXDestroyPixmap:
invlpg [rax]
db "glXDestroyPixmap", 0
ret

global glXDestroyWindow:function
align 16
glXDestroyWindow:
invlpg [rax]
db "glXDestroyWindow", 0
ret

global glXGetClientString:function
align 16
glXGetClientString:
invlpg [rax]
db "glXGetClientString", 0
ret

global glXGetCurrentDisplay:function
align 16
glXGetCurrentDisplay:
invlpg [rax]
db "glXGetCurrentDisplay", 0
ret

global glXGetCurrentReadDrawable:function
align 16
glXGetCurrentReadDrawable:
invlpg [rax]
db "glXGetCurrentReadDrawable", 0
ret

global glXGetFBConfigAttrib:function
align 16
glXGetFBConfigAttrib:
invlpg [rax]
db "glXGetFBConfigAttrib", 0
ret

global glXGetFBConfigs:function
align 16
glXGetFBConfigs:
invlpg [rax]
db "glXGetFBConfigs", 0
ret

global glXGetProcAddress:function
global glXGetProcAddressARB:function
align 16
glXGetProcAddress:
glXGetProcAddressARB:
push rdi
; glXGetProcAddress can be called with a glX function such as glXChooseVisual
; We need to return a guest pointer to that function which is easier to do in guest code
call __felix86_glXGetProcAddressSelf wrt ..plt
test rax, rax
pop rdi
jnz ptr_ok
; If not found, use the host function. It's probably a GL function in this case
invlpg [rax]
db "glXGetProcAddress", 0
ptr_ok:
ret

global glXGetSelectedEvent:function
align 16
glXGetSelectedEvent:
invlpg [rax]
db "glXGetSelectedEvent", 0
ret

global glXGetVisualFromFBConfig:function
align 16
glXGetVisualFromFBConfig:
invlpg [rax]
db "glXGetVisualFromFBConfig", 0
ret

global glXMakeContextCurrent:function
align 16
glXMakeContextCurrent:
invlpg [rax]
db "glXMakeContextCurrent", 0
ret

global glXQueryContext:function
align 16
glXQueryContext:
invlpg [rax]
db "glXQueryContext", 0
ret

global glXQueryDrawable:function
align 16
glXQueryDrawable:
invlpg [rax]
db "glXQueryDrawable", 0
ret

global glXQueryExtensionsString:function
align 16
glXQueryExtensionsString:
invlpg [rax]
db "glXQueryExtensionsString", 0
ret

global glXQueryServerString:function
align 16
glXQueryServerString:
invlpg [rax]
db "glXQueryServerString", 0
ret

global glXSelectEvent:function
align 16
glXSelectEvent:
invlpg [rax]
db "glXSelectEvent", 0
ret

global __glXGLLoadGLXFunction:function
align 16
__glXGLLoadGLXFunction:
invlpg [rax]
db "__glXGLLoadGLXFunction", 0
ret


section .data.rel.ro
global __GLXGL_CORE_FUNCTIONS
align 16

__GLXGL_CORE_FUNCTIONS:
    dq glXChooseFBConfig
    dq glXChooseVisual
    dq glXCopyContext
    dq glXCreateContext
    dq glXCreateGLXPixmap
    dq glXCreateNewContext
    dq glXCreatePbuffer
    dq glXCreatePixmap
    dq glXCreateWindow
    dq glXDestroyContext
    dq glXDestroyGLXPixmap
    dq glXDestroyPbuffer
    dq glXDestroyPixmap
    dq glXDestroyWindow
    dq glXGetClientString
    dq glXGetConfig
    dq glXGetCurrentContext
    dq glXGetCurrentDrawable
    dq glXGetCurrentReadDrawable
    dq glXGetFBConfigAttrib
    dq glXGetFBConfigs
    dq glXGetProcAddress
    dq glXGetProcAddressARB
    dq glXGetSelectedEvent
    dq glXGetVisualFromFBConfig
    dq glXIsDirect
    dq glXMakeContextCurrent
    dq glXMakeCurrent
    dq glXQueryContext
    dq glXQueryDrawable
    dq glXQueryExtension
    dq glXQueryExtensionsString
    dq glXQueryServerString
    dq glXQueryVersion
    dq glXSelectEvent
    dq glXSwapBuffers
    dq glXUseXFont
    dq glXWaitGL
    dq glXWaitX

section .init_array
    dq __felix86_constructor