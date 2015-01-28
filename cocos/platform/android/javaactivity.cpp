/****************************************************************************
Copyright (c) 2013-2014 Chukong Technologies Inc.

http://www.cocos2d-x.org

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
****************************************************************************/

#include "base/CCPlatformConfig.h"
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

#include "CCApplication.h"
#include "base/CCDirector.h"
#include "base/CCEventCustom.h"
#include "base/CCEventType.h"
#include "renderer/CCGLProgramCache.h"
#include "renderer/CCTextureCache.h"
#include "2d/CCDrawingPrimitives.h"
#include "CCGLViewImpl-android.h"
#include "platform/android/jni/JniHelper.h"

#ifdef CC_USE_GOOGLE_PLAY_GAME_SERVICES
#include "gpg/gpg.h"
#endif

#include <android/log.h>
#include <jni.h>

#define  LOG_TAG    "main"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

void cocos_android_app_init(JNIEnv* env, jobject thiz) __attribute__((weak));

using namespace cocos2d;

extern "C"
{

jint JNI_OnLoad(JavaVM *vm, void *reserved)
{
    JniHelper::setJavaVM(vm);

//if we are using GPG
#ifdef CC_USE_GOOGLE_PLAY_GAME_SERVICES
    LOGD("Calling gpg::AndroidInitialization::JNI_OnLoad(vm)");
    gpg::AndroidInitialization::JNI_OnLoad(vm);
#endif

    return JNI_VERSION_1_4;
}

void Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeInit(JNIEnv*  env, jobject thiz, jint w, jint h)
{
    auto director = cocos2d::Director::getInstance();
    auto glview = director->getOpenGLView();
    if (!glview)
    {
	    LOGD("nativeInit Calling glview = cocos2d::GLViewImpl::create('Android app');");
        glview = cocos2d::GLViewImpl::create("Android app");
        glview->setFrameSize(w, h);
        director->setOpenGLView(glview);

        if(! cocos2d::Application::getInstance())
        {
            LOGD("nativeInit Calling cocos_android_app_init(env, thiz);");
            cocos_android_app_init(env, thiz);
        }
        cocos2d::Application::getInstance()->run();
    }
    else
    {
		LOGD("nativeInit Calling cocos2d::GL::invalidateStateCache();");
        cocos2d::GL::invalidateStateCache();
        cocos2d::GLProgramCache::getInstance()->reloadDefaultGLPrograms();
        cocos2d::DrawPrimitives::init();

        cocos2d::EventCustom recreatedEvent(EVENT_RENDERER_RECREATED);
        director->getEventDispatcher()->dispatchEvent(&recreatedEvent);
        director->setGLDefaultValues();
    }

}

jintArray Java_org_cocos2dx_lib_Cocos2dxActivity_getGLContextAttrs(JNIEnv* env, jobject thiz)
{
    auto director = cocos2d::Director::getInstance();
    auto glview = director->getOpenGLView();
    if (!glview)
    {
        if(! cocos2d::Application::getInstance())
        {
            LOGD("getGLContextAttrs Calling cocos_android_app_init(env, thiz);");
            cocos_android_app_init(env, thiz);
        }
        LOGD("getGLContextAttrs Calling cocos2d::Application::getInstance()->initGLContextAttrs();");
        cocos2d::Application::getInstance()->initGLContextAttrs();
    }
    GLContextAttrs _glContextAttrs = GLView::getGLContextAttrs();

    int tmp[6] = {_glContextAttrs.redBits, _glContextAttrs.greenBits, _glContextAttrs.blueBits,
        _glContextAttrs.alphaBits, _glContextAttrs.depthBits, _glContextAttrs.stencilBits};

	LOGD("getGLContextAttrs Calling env->SetIntArrayRegion(glContextAttrsJava, 0, 6, tmp);");
    jintArray glContextAttrsJava = env->NewIntArray(6);
    env->SetIntArrayRegion(glContextAttrsJava, 0, 6, tmp);

    return glContextAttrsJava;
}

void Java_org_cocos2dx_lib_Cocos2dxRenderer_nativeOnSurfaceChanged(JNIEnv*  env, jobject thiz, jint w, jint h)
{
    cocos2d::Application::getInstance()->applicationScreenSizeChanged(w, h);
}

}

#endif // CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

