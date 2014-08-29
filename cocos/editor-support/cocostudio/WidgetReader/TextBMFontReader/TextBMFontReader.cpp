

#include "TextBMFontReader.h"
#include "ui/UITextBMFont.h"

USING_NS_CC;
using namespace ui;

namespace cocostudio
{
    static TextBMFontReader* instanceTextBMFontReader = NULL;
    
    IMPLEMENT_CLASS_WIDGET_READER_INFO(TextBMFontReader)
    
    TextBMFontReader::TextBMFontReader()
    {
        
    }
    
    TextBMFontReader::~TextBMFontReader()
    {
        
    }
    
    TextBMFontReader* TextBMFontReader::getInstance()
    {
        if (!instanceTextBMFontReader)
        {
            instanceTextBMFontReader = new TextBMFontReader();
        }
        return instanceTextBMFontReader;
    }
    
    void TextBMFontReader::setPropsFromJsonDictionary(Widget *widget, const rapidjson::Value &options)
    {
        WidgetReader::setPropsFromJsonDictionary(widget, options);
        
        
        std::string jsonPath = GUIReader::getInstance()->getFilePath();
        
        TextBMFont* labelBMFont = static_cast<TextBMFont*>(widget);
        
        const rapidjson::Value& cmftDic = DICTOOL->getSubDictionary_json(options, "fileNameData");
        int cmfType = DICTOOL->getIntValue_json(cmftDic, "resourceType");
        switch (cmfType)
        {
            case 0:
            {
				const char* cmfPath = DICTOOL->getStringValue_json(cmftDic, "path");
				std::string tp_c = cocos2d::FileUtils::getInstance()->fullPathForFilename(cmfPath);
                
				labelBMFont->setFntFile(tp_c);
                break;
            }
            case 1:
                CCLOG("Wrong res type of LabelAtlas!");
                break;
            default:
                break;
        }
        
        const char* text = DICTOOL->getStringValue_json(options, "text");
        labelBMFont->setString(text);
        
        
        WidgetReader::setColorPropsFromJsonDictionary(widget, options);
    }
}
