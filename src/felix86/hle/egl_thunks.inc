// Read abi.cpp -> signature format
// X(lib name, func name, signature)
X("libEGL.so", eglBindAPI, d_d)
X("libEGL.so", eglDestroyContext, d_qq)
X("libEGL.so", eglDestroySurface, d_qq)
X("libEGL.so", eglInitialize, d_qqq)
X("libEGL.so", eglMakeCurrent, d_qqqq)
X("libEGL.so", eglQuerySurface, d_qqdq)
X("libEGL.so", eglSurfaceAttrib, d_qqdd)
X("libEGL.so", eglSwapBuffers, d_qq)
X("libEGL.so", eglTerminate, d_q)
X("libEGL.so", eglGetError, d_)
X("libEGL.so", eglCreateContext, q_qqqq)
X("libEGL.so", eglCreateWindowSurface, q_qqqq)
X("libEGL.so", eglGetCurrentContext, q_)
X("libEGL.so", eglGetCurrentDisplay, q_)
X("libEGL.so", eglGetCurrentSurface, q_d)
X("libEGL.so", eglGetDisplay, q_q)
X("libEGL.so", eglGetProcAddress, q_q)
X("libEGL.so", eglQueryString, q_qd)
X("libEGL.so", eglChooseConfig, d_qqqdq)
X("libEGL.so", eglCopyBuffers, d_qqq)
X("libEGL.so", eglCreatePbufferSurface, q_qqq)
X("libEGL.so", eglCreatePixmapSurface, q_qqqq)
X("libEGL.so", eglGetConfigAttrib, d_qqdq)
X("libEGL.so", eglGetConfigs, d_qqdq)
X("libEGL.so", eglQueryContext, d_qqdq)
X("libEGL.so", eglWaitGL, d_)
X("libEGL.so", eglWaitNative, d_d)
X("libEGL.so", eglQueryAPI, d_d)
X("libEGL.so", eglCreatePbufferFromClientBuffer, q_qdqqq)
X("libEGL.so", eglReleaseThread, d_)
X("libEGL.so", eglWaitClient, d_)
X("libEGL.so", eglBindTexImage, d_qqd)
X("libEGL.so", eglReleaseTexImage, d_qqd)
X("libEGL.so", eglSwapInterval, d_qd)
X("libEGL.so", eglSwapBuffersWithDamageKHR, d_qqqd)
X("libEGL.so", eglGetPlatformDisplay, q_dqq)
X("libEGL.so", eglGetPlatformDisplayEXT, q_dqq)
X("libEGL.so", eglCreatePlatformWindowSurfaceEXT, q_qqqq)
X("libEGL.so", eglCreatePlatformPixmapSurfaceEXT, q_qqqq)
X("libEGL.so", eglCreateSyncKHR, q_qdq)
X("libEGL.so", eglDestroySyncKHR, d_qq)
X("libEGL.so", eglClientWaitSyncKHR, d_qqdq)
X("libEGL.so", eglGetSyncAttribKHR, d_qqdq)
X("libEGL.so", eglWaitSyncKHR, d_qqd)
X("libEGL.so", eglDupNativeFenceFDANDROID, d_qq)

// MESA_query_driver
X("libEGL.so", eglGetDisplayDriverConfig, q_q)
X("libEGL.so", eglGetDisplayDriverName, q_q)

// EXT_device_query
X("libEGL.so", eglQueryDeviceAttribEXT, d_qdq)
X("libEGL.so", eglQueryDeviceStringEXT, q_qd)
X("libEGL.so", eglQueryDisplayAttribEXT, d_qdq)

// EXT_device_enumeration
X("libEGL.so", eglQueryDevicesEXT, d_dqq)
