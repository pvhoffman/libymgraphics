//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdexcept>
#include <sstream>
#pragma hdrstop
#include "Wordwidth.h"
#include "Charwidth.h"
//---------------------------------------------------------------------------
YM_NS_DEF(wordwidth)
//---------------------------------------------------------------------------
Wordwidth::Wordwidth()
{
        _wordBoundries = strdup(YM_NS_SCOPE(global)defaultWordBoundries);
}
//---------------------------------------------------------------------------
Wordwidth::~Wordwidth()
{
        if (_wordBoundries) free(_wordBoundries);
        _wordBoundries = 0;
}
//---------------------------------------------------------------------------
float Wordwidth::getWordWidth(const int& font, const char* str, const float& swidth)
{
        YM_NS_USE(charwidth);

        float width = 0.0f;
        Charwidth charWidth;

        while (str && *str) {
                if (strchr(_wordBoundries, *str) != NULL)
                        break;
                width += charWidth.getCharWidth(font, *str, swidth);
                str++;
        }
        while (str && *str && strchr(_wordBoundries, *str) != NULL) {
                width += charWidth.getCharWidth(font, *str, swidth);
                str++;
        }
        return width;
}
const char* Wordwidth::getWordBoundries()
{
        return _wordBoundries;
}
void Wordwidth::setWordBoundries(const char* boundries)
{
        if (_wordBoundries) free(_wordBoundries);
        _wordBoundries = strdup(boundries);
}

//---------------------------------------------------------------------------
YM_NS_END
//---------------------------------------------------------------------------

