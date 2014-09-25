if(WIN32)

ADD_DEFINITIONS(-DUNICODE -D_UNICODE)

set(COCOS_PLATFORM_SPECIFIC_SRC
  platform/win32/CCStdC.cpp
  platform/win32/CCFileUtilsWin32.cpp
  platform/win32/CCCommon.cpp
  platform/win32/CCApplication-win32.cpp
  platform/win32/CCDevice.cpp
)

elseif(APPLE)

else()

set(COCOS_PLATFORM_SPECIFIC_SRC
  platform/linux/CCStdC.cpp
  platform/linux/CCFileUtilsLinux.cpp
  platform/linux/CCCommon.cpp
  platform/linux/CCApplication-linux.cpp
  platform/linux/CCDevice.cpp
)

endif()

include_directories(
  ../external/ConvertUTF
)

set(COCOS_PLATFORM_SRC
  platform/CCSAXParser.cpp
  platform/CCThread.cpp
  platform/CCGLView.cpp
  platform/CCFileUtils.cpp
  platform/CCImage.cpp
  platform/desktop/CCGLViewImpl-desktop.cpp
  ../external-bin/edtaa3func/edtaa3func.cpp
  ../external/ConvertUTF/ConvertUTFWrapper.cpp
  ../external/ConvertUTF/ConvertUTF.c
  ${COCOS_PLATFORM_SPECIFIC_SRC}
)

