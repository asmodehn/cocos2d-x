#/****************************************************************************
# Copyright (c) 2013 cocos2d-x.org
# Copyright (c) 2012-2013 martell malone
# 
# http://www.cocos2d-x.org
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# ****************************************************************************/

cmake_minimum_required(VERSION 2.8)
project (Cocos2dx)

# The version number
set(Cocos2dxSamples_VERSION_MAJOR 3)
set(Cocos2dxSamples_VERSION_MINOR 0)

include(build/BuildHelpers.CMakeLists.txt)

option(USE_CHIPMUNK "Use chipmunk for physics library" ON)
option(USE_BOX2D "Use box2d for physics library" OFF)
option(DEBUG_MODE "Debug or release?" ON)
option(BUILD_LIBS_LUA "Build lua libraries" OFF)
option(BUILD_EDITOR_SPINE "Build editor support for spine" ON)
option(BUILD_EDITOR_COCOSTUDIO "Build editor support for cocostudio" ON)
option(BUILD_EDITOR_COCOSBUILDER "Build editor support for cocosbuilder" ON)

option(BUILD_CppTests "Only build TestCpp sample" ON)
option(BUILD_LuaTests "Only build TestLua sample" OFF)

#Needed on everything but windows because we build shared libraries from static convenience libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#This is ignored on multi configuration IDE like MSVC
if(DEBUG_MODE)
  set(CMAKE_BUILD_TYPE DEBUG)
else(DEBUG_MODE)
  set(CMAKE_BUILD_TYPE RELEASE)
endif(DEBUG_MODE)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DCOCOS2D_DEBUG=1")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DCOCOS2D_DEBUG=1")

if(WIN32)
  ADD_DEFINITIONS (-DIGNORE_EXPORT -D_WINDOWS -DWIN32)
  if(MSVC)
    ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS 
					-D_SCL_SECURE_NO_WARNINGS 
					-wd4251 -wd4244 -wd4334 
					-wd4005 -wd4820 -wd4710
					-wd4514 -wd4056 -wd4996 
					-wd4099 -wd4819
					-MP -Qpar)
  else(MSVC)#MINGW
  
  endif(MSVC)

elseif(APPLE)
  ADD_DEFINITIONS(-DCC_TARGET_OS_MAC)
  ADD_DEFINITIONS (-DUSE_FILE32API)
else()#Linux
  ADD_DEFINITIONS(-DLINUX)
endif()


if(NOT MSVC)# all gcc
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -Wall -DCOCOS2D_DEBUG=1")
  set(CMAKE_CXX_FLAGS_DEBUG ${CMAKE_C_FLAGS_DEBUG})
  set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-std=c99")
  set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11")
endif()

if(MINGW)
  add_definitions(-DGLEW_STATIC)
  add_definitions(-D__SSIZE_T)
  set(CMAKE_C_STANDARD_LIBRARIES "${CMAKE_C_STANDARD_LIBRARIES} -lws2_32")
  set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} -lws2_32")
endif()


if(USE_CHIPMUNK)
  message("Using chipmunk ...")
  add_definitions(-DCC_ENABLE_CHIPMUNK_INTEGRATION=1)
elseif(USE_BOX2D)
  message("Using box2d ...")
  add_definitions(-DCC_ENABLE_BOX2D_INTEGRATION=1)
else(USE_CHIPMUNK)
  message(FATAL_ERROR "Must choose a physics library.")
endif(USE_CHIPMUNK)

# architecture
if ( CMAKE_SIZEOF_VOID_P EQUAL 8 )
set(ARCH_DIR "64-bit")
else()
set(ARCH_DIR "32-bit")
endif()

if(WIN32) # Win32
  set(PLATFORM_FOLDER win32)
elseif(APPLE)# osx or ios
  set(PLATFORM_FOLDER mac)
else() # Assume Linux
  set(PLATFORM_FOLDER linux)
endif()

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/cocos
  ${CMAKE_CURRENT_SOURCE_DIR}/cocos/platform/desktop
  ${CMAKE_CURRENT_SOURCE_DIR}/cocos/platform/${PLATFORM_FOLDER}
  ${CMAKE_CURRENT_SOURCE_DIR}/cocos/audio/include
  ${CMAKE_CURRENT_SOURCE_DIR}/cocos/editor-support
  ${CMAKE_CURRENT_SOURCE_DIR}/cocos/scripting/lua-bindings/auto
  ${CMAKE_CURRENT_SOURCE_DIR}/cocos/scripting/lua-bindings/manual
  ${CMAKE_CURRENT_SOURCE_DIR}/extensions
  ${CMAKE_CURRENT_SOURCE_DIR}/external
  ${CMAKE_CURRENT_SOURCE_DIR}/external-bin
  ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/tinyxml2
  ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/unzip
  ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/sqlite3/include
  ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/edtaa3func
  ${CMAKE_CURRENT_SOURCE_DIR}/external/chipmunk/include/chipmunk
  ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/jpeg/include/${PLATFORM_FOLDER}
  ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/png/include/${PLATFORM_FOLDER}
  ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/tiff/include/${PLATFORM_FOLDER}
  ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/webp/include/${PLATFORM_FOLDER}
  ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/glfw3/include/${PLATFORM_FOLDER}
  ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/freetype2/include/${PLATFORM_FOLDER}
  ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/websockets/include/${PLATFORM_FOLDER}
  ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/xxhash
  ${CMAKE_CURRENT_SOURCE_DIR}/external/xxtea
)

if(WIN32 AND NOT MINGW)
  include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/${PLATFORM_FOLDER}-specific/gles/include/OGLES
    ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/${PLATFORM_FOLDER}-specific/icon/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/${PLATFORM_FOLDER}-specific/zlib/include
    external-bin/curl/include/${PLATFORM_FOLDER}
  )
elseif(APPLE)
  include_directories(
  	${CMAKE_CURRENT_SOURCE_DIR}/external-bin/curl/include/${PLATFORM_FOLDER}
  )
else()
  include_directories(
    /usr/include
    /usr/include/GLFW
    /usr/local/include/GLFW
    ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/${PLATFORM_FOLDER}-specific/fmod/include/${ARCH_DIR}
  )
endif()


if(WIN32)

  if(NOT MINGW)
  link_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/jpeg/prebuilt/${PLATFORM_FOLDER}
    ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/tiff/prebuilt/${PLATFORM_FOLDER}
    ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/png/prebuilt/${PLATFORM_FOLDER}
    ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/sqlite3/libraries/${PLATFORM_FOLDER}
    ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/freetype2/prebuilt/${PLATFORM_FOLDER}
    ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/curl/prebuilt/${PLATFORM_FOLDER}
    ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/websockets/prebuilt/${PLATFORM_FOLDER}
    ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/webp/prebuilt/${PLATFORM_FOLDER}
    ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/glfw3/prebuilt/${PLATFORM_FOLDER}
    ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/${PLATFORM_FOLDER}-specific/icon/prebuilt
    ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/${PLATFORM_FOLDER}-specific/zlib/prebuilt
    ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/${PLATFORM_FOLDER}-specific/gles/prebuilt
  )
  endif()

elseif(APPLE)
  link_directories(
    /usr/local/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/jpeg/prebuilt/${PLATFORM_FOLDER}
    ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/tiff/prebuilt/${PLATFORM_FOLDER}
    ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/webp/prebuilt/${PLATFORM_FOLDER}
    ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/png/prebuilt/${PLATFORM_FOLDER}
    ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/freetype2/prebuilt/${PLATFORM_FOLDER}
    ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/glfw3/prebuilt/${PLATFORM_FOLDER}
    ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/websockets/prebuilt/${PLATFORM_FOLDER}
  )

else()

  link_directories(
    /usr/local/lib
    ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/jpeg/prebuilt/${PLATFORM_FOLDER}/${ARCH_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/tiff/prebuilt/${PLATFORM_FOLDER}/${ARCH_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/webp/prebuilt/${PLATFORM_FOLDER}/${ARCH_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/png/prebuilt/${PLATFORM_FOLDER}/${ARCH_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/freetype2/prebuilt/${PLATFORM_FOLDER}/${ARCH_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/websockets/prebuilt/${PLATFORM_FOLDER}/${ARCH_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/external-bin/${PLATFORM_FOLDER}-specific/fmod/prebuilt/${ARCH_DIR}
  )

endif()

# chipmunk library
if(USE_CHIPMUNK)
add_subdirectory(external/chipmunk/src)
endif(USE_CHIPMUNK)

# box2d library
if(USE_BOX2D)
add_subdirectory(external/Box2D)
endif(USE_BOX2D)

# unzip library
add_subdirectory(external-bin/unzip)

# tinyxml2 library
add_subdirectory(external-bin/tinyxml2)

# xxhash library
add_subdirectory(external-bin/xxhash)

# xxtea library
add_subdirectory(external/xxtea)

# audio
add_subdirectory(cocos/audio)

# cocos2d library
add_subdirectory(cocos)

# cocos2d ui
add_subdirectory(cocos/ui)

# extensions
add_subdirectory(extensions)

## Editor Support

if(BUILD_EDITOR_SPINE)
# spine
add_subdirectory(cocos/editor-support/spine)
endif(BUILD_EDITOR_SPINE)

if(BUILD_EDITOR_COCOSBUILDER)
# cocosbuilder
add_subdirectory(cocos/editor-support/cocosbuilder)
endif(BUILD_EDITOR_COCOSBUILDER)

if(BUILD_EDITOR_COCOSTUDIO)
# cocostudio
add_subdirectory(cocos/editor-support/cocostudio)
endif(BUILD_EDITOR_COCOSTUDIO)

## Scripting
if(BUILD_LIBS_LUA)
# lua
add_subdirectory(external-bin/lua/lua)

# tolua
add_subdirectory(external-bin/lua/tolua)

# luabinding
add_subdirectory(cocos/scripting/lua-bindings)

endif(BUILD_LIBS_LUA)

# build tests 
if(BUILD_CppTests)
  add_subdirectory(tests/cpp-empty-test)
  add_subdirectory(tests/cpp-tests)
endif(BUILD_CppTests)

if(BUILD_LuaTests)
  add_subdirectory(tests/lua-tests/project)
  add_subdirectory(tests/lua-empty-test/project)
endif(BUILD_LuaTests)
