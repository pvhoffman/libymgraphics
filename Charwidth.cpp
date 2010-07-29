//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdexcept>
#include <sstream>
#pragma hdrstop
#include "Charwidth.h"
//---------------------------------------------------------------------------
YM_NS_DEF(charwidth)
//---------------------------------------------------------------------------
Charwidth::Charwidth()
{
}
//---------------------------------------------------------------------------
Charwidth::~Charwidth()
{
}
//---------------------------------------------------------------------------
float Charwidth::getCharWidth(const int& font, const char& c, const float& swidth)
{
        float width = 0.0f;
        const unsigned short *widthData = 0;

        switch (font) {
        case YM_NS_SCOPE(types)fntHelvetica:
                widthData = YM_NS_SCOPE(global)helveticaCharWidth;
                break;
        case YM_NS_SCOPE(types)fntHelveticaBlackCondensed:
                widthData = YM_NS_SCOPE(global)helveticaBlackCondensedCharWidth;
                break;
        default:
                throw std::runtime_error("Cannot width unsupported font.");
        }
        int letter = (unsigned char)c - 32;
        if (letter < 0 || letter > 255) letter = 0;
        width = ((widthData[letter] / 1000.0f) * swidth);
        return width;
}
//---------------------------------------------------------------------------
YM_NS_END
//---------------------------------------------------------------------------

