#when building DLL this must be defined only for this project and statically linked ones.
ADD_DEFINITIONS (-D_USRDLL)


if(WIN32)
ADD_DEFINITIONS(-DUNICODE -D_UNICODE)
endif()

set(COCOS_SRC
  cocos2d.cpp
)

include(2d/CMakeLists.txt)
include(3d/CMakeLists.txt)
include(platform/CMakeLists.txt)
include(physics/CMakeLists.txt)
include(math/CMakeLists.txt)
include(renderer/CMakeLists.txt)
include(base/CMakeLists.txt)
include(deprecated/CMakeLists.txt)
include(network/CMakeLists.txt)
include(storage/CMakeLists.txt)

add_library(cocos2d SHARED
  ${COCOS_SRC}
  ${COCOS_2D_SRC}
  ${COCOS_3D_SRC}
  ${COCOS_PLATFORM_SRC}
  ${COCOS_BASE_SRC}
  ${COCOS_RENDERER_SRC}
  ${COCOS_MATH_SRC}
  ${COCOS_PHYSICS_SRC}
  ${COCOS_NETWORK_SRC}
  ${COCOS_STORAGE_SRC}
  ${COCOS_DEPRECATED_SRC}
)

if(WIN32)

if(MINGW)

set(COCOS_LINK
  z
  jpeg
  png
  webp
  tiff
  glfw3
  glew32
  opengl32
  iconv
  freetype
  bz2
  )

else()


set(COCOS_LINK
  libjpeg
  libpng
  libwebp
  libtiff
  freetype250
  glfw3
  glew32
  opengl32
  libiconv
  libzlib
  )

endif()

elseif(APPLE)
 FIND_PACKAGE(ZLIB)

 INCLUDE_DIRECTORIES ( /System/Library/Frameworks ${ZLIB_INCLUDE_DIR})

 FIND_LIBRARY(COCOA_LIBRARY Cocoa)
 FIND_LIBRARY(OPENGL_LIBRARY OpenGL )
 FIND_LIBRARY(AUDIOTOOLBOX_LIBRARY AudioToolbox)
 FIND_LIBRARY(OPENAL_LIBRARY OpenAL )
 FIND_LIBRARY(QUARTZCORE_LIBRARY QuartzCore )
 FIND_LIBRARY(APPLICATIONSERVICES_LIBRARY ApplicationServices)
 FIND_LIBRARY(IOKIT_LIBRARY IOKit)
 FIND_LIBRARY(FOUNDATION_LIBRARY Foundation)

set(COCOS_LINK
  libjpeg.a
  libwebp.a
  libtiff.a
  libfreetype.a
  #fontconfig.a
  libpng.a
  pthread.a
  libglfw3.a
  ${COCOA_LIBRARY}
  ${OPENGL_LIBRARY}
  ${OPENAL_LIBRARY}
  ${AUDIOTOOLBOX_LIBRARY}
  ${QUARTZCORE_LIBRARY}
  ${APPLICATIONSERVICES_LIBRARY}
  ${IOKIT_LIBRARY}
  ${FOUNDATION_LIBRARY}
  ${ZLIB_LIBRARY}
)

else()
set(COCOS_LINK
  jpeg
  webp
  tiff
  freetype
  fontconfig
  png
  pthread
  glfw
  GLEW
  GL
  X11
  rt
  z
)
endif()

target_link_libraries(cocos2d
  chipmunk_static
  tinyxml2
  unzip
  xxhash
  xxtea
  ${COCOS_STORAGE_LINK}
  ${COCOS_NETWORK_LINK}
  ${COCOS_LINK}
)

set_target_properties(cocos2d
    PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

