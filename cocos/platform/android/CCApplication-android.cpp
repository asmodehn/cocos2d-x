/****************************************************************************
Copyright (c) 2010-2012 cocos2d-x.org
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

#include "jni/JniHelper.h"
#include "jni/Java_org_cocos2dx_lib_Cocos2dxHelper.h"
#include "CCApplication.h"
#include "base/CCDirector.h"
#include "base/CCEventType.h"
#include "base/CCEventListenerCustom.h"
#include "renderer/CCTextureCache.h"
#include <android/log.h>
#include <jni.h>
#include <cstring>

#define  LOG_TAG    "CCApplication_android Debug"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

NS_CC_BEGIN

// sharedApplication pointer
Application * Application::sm_pSharedApplication = 0;

Application::Application()
#if CC_ENABLE_CACHE_TEXTURE_DATA
: m_rendererRecreatedListener(nullptr)
#endif //CC_ENABLE_CACHE_TEXTURE_DATA
{
    CCAssert(! sm_pSharedApplication, "");
    sm_pSharedApplication = this;
}

Application::~Application()
{

#if CC_ENABLE_CACHE_TEXTURE_DATA
		if (m_rendererRecreatedListener)
		{
			auto eventDispatcher = cocos2d::Director::getInstance()->getEventDispatcher();
			eventDispatcher->removeEventListener(m_rendererRecreatedListener);
			m_rendererRecreatedListener = nullptr;
		}
#endif //CC_ENABLE_CACHE_TEXTURE_DATA

    CCAssert(this == sm_pSharedApplication, "");
    sm_pSharedApplication = NULL;
}

int Application::run()
{
    // Initialize instance and cocos2d.
    if (! applicationDidFinishLaunching())
    {
        return 0;
    }

	// call applicationRendererRecreated when needed
    #if CC_ENABLE_CACHE_TEXTURE_DATA
            auto eventDispatcher = cocos2d::Director::getInstance()->getEventDispatcher();
            m_rendererRecreatedListener = cocos2d::EventListenerCustom::create(EVENT_RENDERER_RECREATED, CC_CALLBACK_1(Application::applicationRendererRecreated, this));
            eventDispatcher->addEventListenerWithFixedPriority(m_rendererRecreatedListener, 1);
    #endif //CC_ENABLE_CACHE_TEXTURE_DATA

    return -1;
}

void Application::applicationRendererRecreated(cocos2d::EventCustom* evt)
{
    //by default recreating all textures.
    //can be overidden by application implementation
    cocos2d::VolatileTextureMgr::reloadAllTextures();
}

void Application::setAnimationInterval(double interval)
{
  JniMethodInfo methodInfo;
  if (! JniHelper::getStaticMethodInfo(methodInfo, "org/cocos2dx/lib/Cocos2dxRenderer", "setAnimationInterval",
                                       "(D)V"))
  {
    CCLOG("%s %d: error to get methodInfo", __FILE__, __LINE__);
  }
  else
  {
    methodInfo.env->CallStaticVoidMethod(methodInfo.classID, methodInfo.methodID, interval);
  }
}

//////////////////////////////////////////////////////////////////////////
// static member function
//////////////////////////////////////////////////////////////////////////
Application* Application::getInstance()
{
    CCAssert(sm_pSharedApplication, "");
    return sm_pSharedApplication;
}

// @deprecated Use getInstance() instead
Application* Application::sharedApplication()
{
    return Application::getInstance();
}

const char * Application::getCurrentLanguageCode()
{
    static char code[3]={0};
    strncpy(code,getCurrentLanguageJNI().c_str(),2);
    code[2]='\0';
    return code;
}

LanguageType Application::getCurrentLanguage()
{
    std::string languageName = getCurrentLanguageJNI();
    const char* pLanguageName = languageName.c_str();
    LanguageType ret = LanguageType::ENGLISH;

    if (0 == strcmp("zh", pLanguageName))
    {
        ret = LanguageType::CHINESE;
    }
    else if (0 == strcmp("en", pLanguageName))
    {
        ret = LanguageType::ENGLISH;
    }
    else if (0 == strcmp("fr", pLanguageName))
    {
        ret = LanguageType::FRENCH;
    }
    else if (0 == strcmp("it", pLanguageName))
    {
        ret = LanguageType::ITALIAN;
    }
    else if (0 == strcmp("de", pLanguageName))
    {
        ret = LanguageType::GERMAN;
    }
    else if (0 == strcmp("es", pLanguageName))
    {
        ret = LanguageType::SPANISH;
    }
    else if (0 == strcmp("ru", pLanguageName))
    {
        ret = LanguageType::RUSSIAN;
    }
    else if (0 == strcmp("nl", pLanguageName))
    {
        ret = LanguageType::DUTCH;
    }
    else if (0 == strcmp("ko", pLanguageName))
    {
        ret = LanguageType::KOREAN;
    }
    else if (0 == strcmp("ja", pLanguageName))
    {
        ret = LanguageType::JAPANESE;
    }
    else if (0 == strcmp("hu", pLanguageName))
    {
        ret = LanguageType::HUNGARIAN;
    }
    else if (0 == strcmp("pt", pLanguageName))
    {
        ret = LanguageType::PORTUGUESE;
    }
    else if (0 == strcmp("ar", pLanguageName))
    {
        ret = LanguageType::ARABIC;
    }
    else if (0 == strcmp("nb", pLanguageName))
    {
        ret = LanguageType::NORWEGIAN;
    }
    else if (0 == strcmp("pl", pLanguageName))
    {
        ret = LanguageType::POLISH;
    }
    return ret;
}

Application::Platform Application::getTargetPlatform()
{
    return Platform::OS_ANDROID;
}

void Application::applicationScreenSizeChanged(int newWidth, int newHeight) {

}

NS_CC_END

#endif // CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

