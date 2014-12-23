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

#ifndef _CCFontAtlasCache_h_
#define _CCFontAtlasCache_h_

#include <unordered_map>

#include "2d/CCFontAtlas.h"
#include "2d/CCLabel.h"

NS_CC_BEGIN

class CC_DLL FontAtlasCache
{  
public:
	static FontAtlasSwitch * getFontAtlasTTF(const TTFConfig & config);
	static FontAtlasSwitch * getFontAtlasFNT(const std::string& fontFileName, const Vec2& imageOffset = Vec2::ZERO);

	static FontAtlasSwitch * getFontAtlasCharMap(const std::string& charMapFile, int itemWidth, int itemHeight, int startCharMap);
	static FontAtlasSwitch * getFontAtlasCharMap(Texture2D* texture, int itemWidth, int itemHeight, int startCharMap);
	static FontAtlasSwitch * getFontAtlasCharMap(const std::string& plistFile);
    
	static bool releaseFontAtlas(FontAtlasSwitch *atlas);

    /** Removes cached data.
     It will purge the textures atlas and if multiple texture exist in one FontAtlas.
     */
	static void purgeCachedData();

	/**
	*
	*/
	static void reloadCachedData();
	static void incrementalReloadCachedData();
	static void incrementalReloadStart();
	static bool incrementalReloadDone();
	static float incrementalReloadCompletion();

	static void addFontMap(const std::string& originalName, const std::string& mapPath);
	static std::unordered_map<std::string, std::string>::const_iterator findInFontMap(const std::string& name);
	static void purgeFontMap();

    
private: 
	static std::string generateFontName(const std::string& fontFileName, int size, GlyphCollection theGlyphs, bool useDistanceField);
	static void reloadCachedData(std::unordered_map<std::string, FontAtlasSwitch *>::iterator reloadData);

	static std::unordered_map<std::string, FontAtlasSwitch *> _atlasMap;

	static std::unordered_map<std::string, std::string> _fontMapping;

	static std::unordered_map<std::string, FontAtlasSwitch *>::iterator _currentReloadFont;
	static int _currentReloadStep;
};

NS_CC_END

#endif
