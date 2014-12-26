/****************************************************************************
 Copyright (c) 2013      Zynga Inc.
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
#include "2d/CCFontAtlasCache.h"

#include <iostream>
#include <sstream>

#include "2d/CCFontFNT.h"
#include "2d/CCFontFreeType.h"
#include "CCFontCharMap.h"
#include "base/CCDirector.h"

NS_CC_BEGIN

std::unordered_map<std::string, FontAtlasSwitch *> FontAtlasCache::_atlasMap;
std::unordered_map<std::string, std::string> FontAtlasCache::_fontMapping;
std::unordered_map<std::string, FontAtlasSwitch *>::iterator FontAtlasCache::_currentReloadFont = _atlasMap.end();
int FontAtlasCache::_currentReloadStep = 0;

void FontAtlasCache::purgeCachedData()
{
    for (auto & atlas:_atlasMap)
    {
        atlas.second->getAtlas()->purgeTexturesAtlas();
    }
}

void FontAtlasCache::reloadCachedData()
{
	for (auto atlas = _atlasMap.begin(); atlas != _atlasMap.end(); ++atlas)
	{
		reloadCachedData(atlas);
	}
}

void FontAtlasCache::forceEvent()
{
	for (auto atlas = _atlasMap.begin(); atlas != _atlasMap.end(); ++atlas)
	{
		auto eventDispatcher = cocos2d::Director::getInstance()->getEventDispatcher();
		eventDispatcher->dispatchCustomEvent(cocos2d::FontAtlasSwitch::EVENT_SWITCH_ATLAS, atlas->second);
	}
}

void FontAtlasCache::incrementalReloadCachedData()
{
	reloadCachedData(_currentReloadFont);
	++_currentReloadFont;
	++_currentReloadStep;
}

void FontAtlasCache::incrementalReloadStart()
{
	_currentReloadFont = _atlasMap.begin();
	_currentReloadStep = 0;
}

bool FontAtlasCache::incrementalReloadDone()
{
	return _currentReloadFont == _atlasMap.end();
}

float FontAtlasCache::incrementalReloadCompletion()
{
	return static_cast<float>(_currentReloadStep )/ _atlasMap.size();
}

void FontAtlasCache::reloadCachedData(std::unordered_map<std::string, FontAtlasSwitch *>::iterator reloadData)
{
	auto it = findInFontMap(reloadData->first);
	if (it != _fontMapping.end())
	{
		reloadData->second->getAtlas()->purgeTexturesAtlas();
		Font* font = nullptr;
		if (const FontFNT* bmp = dynamic_cast<const FontFNT*>(reloadData->second->getAtlas()->getFont()))
		{
			font = FontFNT::create(it->second, bmp->getOffset());
		}
		else if (const FontFreeType* ttf = dynamic_cast<const FontFreeType*>(reloadData->second->getAtlas()->getFont()))
		{
			ttf->getFontSize();
			font = FontFreeType::create(it->second, ttf->getFontSize(), ttf->getGlyphType(),
				ttf->getGlyphCustom(), ttf->isDistanceFieldEnabled(), ttf->getOutlineSize());
		}

		if (font)
		{
			auto tempAtlas = font->createFontAtlas();
			if (tempAtlas)
			{
				_atlasMap[reloadData->first]->switchAtlas(tempAtlas);
				tempAtlas->release();
			}
		}
	}
}

FontAtlasSwitch * FontAtlasCache::getFontAtlasTTF(const TTFConfig & config)
{  
    bool useDistanceField = config.distanceFieldEnabled;
    if(config.outlineSize > 0)
    {
        useDistanceField = false;
    }
    int fontSize = config.fontSize;
    auto contentScaleFactor = CC_CONTENT_SCALE_FACTOR();

    if (useDistanceField)
    {
        fontSize = Label::DistanceFieldFontSize / contentScaleFactor;
    }

    auto atlasName = generateFontName(config.fontFilePath, fontSize, GlyphCollection::DYNAMIC, useDistanceField);
    atlasName.append("_outline_");
    std::stringstream ss;
    ss << config.outlineSize;
    atlasName.append(ss.str());

    auto it = _atlasMap.find(atlasName);

    if ( it == _atlasMap.end() )
	{
		std::string fontMapName = config.fontFilePath;
		auto it = findInFontMap(fontMapName);
		if (it != _fontMapping.end())
		{
			fontMapName = it->second;
		}

		auto font = FontFreeType::create(fontMapName, fontSize, config.glyphs,
            config.customGlyphs, useDistanceField, config.outlineSize);
        if (font)
        {
			auto switchFnt = FontAtlasSwitch::create();
            auto tempAtlas = font->createFontAtlas();
            if (tempAtlas)
            {
				switchFnt->switchAtlas(tempAtlas);
				tempAtlas->release();
				_atlasMap[atlasName] = switchFnt;
                return _atlasMap[atlasName];
            }
        }
    }
    else
    {
        _atlasMap[atlasName]->retain();
        return _atlasMap[atlasName];
    }

    return nullptr;
}

FontAtlasSwitch * FontAtlasCache::getFontAtlasFNT(const std::string& fontFileName, const Vec2& imageOffset /* = Vec2::ZERO */)
{
    std::string atlasName = generateFontName(fontFileName, 0, GlyphCollection::CUSTOM,false);
    auto it = _atlasMap.find(atlasName);

    if ( it == _atlasMap.end() )
	{
		std::string fontMapName = fontFileName;
		auto it = findInFontMap(fontFileName);
		if (it != _fontMapping.end())
		{
			fontMapName = it->second;
		}

		auto font = FontFNT::create(fontMapName, imageOffset);
        if(font)
		{
			auto switchFnt = FontAtlasSwitch::create();
            auto tempAtlas = font->createFontAtlas();
            if (tempAtlas)
            {
				switchFnt->switchAtlas(tempAtlas);
				tempAtlas->release();
				_atlasMap[atlasName] = switchFnt;
                return _atlasMap[atlasName];
            }
        }
    }
    else
    {
        _atlasMap[atlasName]->retain();
        return _atlasMap[atlasName];
    }
    
    return nullptr;
}

FontAtlasSwitch * FontAtlasCache::getFontAtlasCharMap(const std::string& plistFile)
{
    std::string atlasName = generateFontName(plistFile, 0, GlyphCollection::CUSTOM,false);
    auto it = _atlasMap.find(atlasName);

    if ( it == _atlasMap.end() )
    {
        auto font = FontCharMap::create(plistFile);

        if(font)
		{
			auto switchFnt = FontAtlasSwitch::create();
            auto tempAtlas = font->createFontAtlas();
            if (tempAtlas)
			{
				switchFnt->switchAtlas(tempAtlas);
				tempAtlas->release();
				_atlasMap[atlasName] = switchFnt;
                return _atlasMap[atlasName];
            }
        }
    }
    else
    {
        _atlasMap[atlasName]->retain();
        return _atlasMap[atlasName];
    }

    return nullptr;
}

FontAtlasSwitch * FontAtlasCache::getFontAtlasCharMap(Texture2D* texture, int itemWidth, int itemHeight, int startCharMap)
{
    char tmp[30];
    sprintf(tmp,"name:%u_%d_%d_%d",texture->getName(),itemWidth,itemHeight,startCharMap);
    std::string atlasName = generateFontName(tmp, 0, GlyphCollection::CUSTOM,false);

    auto it = _atlasMap.find(atlasName);
    if ( it == _atlasMap.end() )
    {
        auto font = FontCharMap::create(texture,itemWidth,itemHeight,startCharMap);

        if(font)
		{
			auto switchFnt = FontAtlasSwitch::create();
            auto tempAtlas = font->createFontAtlas();
            if (tempAtlas)
			{
				switchFnt->switchAtlas(tempAtlas);
				tempAtlas->release();
				_atlasMap[atlasName] = switchFnt;
                return _atlasMap[atlasName];
            }
        }
    }
    else
    {
        _atlasMap[atlasName]->retain();
        return _atlasMap[atlasName];
    }

    return nullptr;
}

FontAtlasSwitch * FontAtlasCache::getFontAtlasCharMap(const std::string& charMapFile, int itemWidth, int itemHeight, int startCharMap)
{
    char tmp[255];
    snprintf(tmp,250,"name:%s_%d_%d_%d",charMapFile.c_str(),itemWidth,itemHeight,startCharMap);

    std::string atlasName = generateFontName(tmp, 0, GlyphCollection::CUSTOM,false);

    auto it = _atlasMap.find(atlasName);
    if ( it == _atlasMap.end() )
    {
        auto font = FontCharMap::create(charMapFile,itemWidth,itemHeight,startCharMap);

        if(font)
		{
			auto switchFnt = FontAtlasSwitch::create();
            auto tempAtlas = font->createFontAtlas();
            if (tempAtlas)
			{
				switchFnt->switchAtlas(tempAtlas);
				tempAtlas->release();
				_atlasMap[atlasName] = switchFnt;
                return _atlasMap[atlasName];
            }
        }
    }
    else
    {
        _atlasMap[atlasName]->retain();
        return _atlasMap[atlasName];
    }

    return nullptr;
}

std::string FontAtlasCache::generateFontName(const std::string& fontFileName, int size, GlyphCollection theGlyphs, bool useDistanceField)
{
    std::string tempName(fontFileName);
    
    switch (theGlyphs)
    {
        case GlyphCollection::DYNAMIC:
            tempName.append("_DYNAMIC_");
        break;
            
        case GlyphCollection::NEHE:
            tempName.append("_NEHE_");
            break;
            
        case GlyphCollection::ASCII:
            tempName.append("_ASCII_");
            break;
            
        case GlyphCollection::CUSTOM:
            tempName.append("_CUSTOM_");
            break;
            
        default:
            break;
    }
    if(useDistanceField)
        tempName.append("df");
    // std::to_string is not supported on android, using std::stringstream instead.
    std::stringstream ss;
    ss << size;
    return  tempName.append(ss.str());
}

bool FontAtlasCache::releaseFontAtlas(FontAtlasSwitch *atlas)
{
    if (nullptr != atlas)
    {
        for( auto &item: _atlasMap )
        {
            if ( item.second == atlas )
            {
                if (atlas->getReferenceCount() == 1)
                {
                  _atlasMap.erase(item.first);
                }
                
                atlas->release();
                
                return true;
            }
        }
    }
    
    return false;
}

void FontAtlasCache::addFontMap(const std::string& originalName, const std::string& mapPath)
{
	// just keep the name with no path
	//std::string::size_type anchor = originalName.find_last_of('/');
	//if (anchor != std::string::npos)
	//{
	//	originalName.erase(0, anchor);
	//}
	//anchor = originalName.find_last_of('\\');
	//if (anchor != std::string::npos)
	//{
	//	originalName.erase(0, anchor);
	//}
	//anchor = originalName.find_last_of('.');
	//if (anchor != std::string::npos)
	//{
	//	originalName.erase(anchor);
	//}

	_fontMapping[originalName] = mapPath;
}

std::unordered_map<std::string, std::string>::const_iterator FontAtlasCache::findInFontMap(const std::string& name)
{
	for (auto curFont = _fontMapping.begin(); curFont != _fontMapping.end(); ++curFont)
	{
		if (name.find(curFont->first) != std::string::npos)
		{
			return curFont;
		}
	}
	return _fontMapping.end();
}

void FontAtlasCache::purgeFontMap()
{
	_fontMapping.clear();
}

NS_CC_END
