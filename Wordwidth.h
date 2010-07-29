#ifndef LIBYMGRAPHICS_WORDWIDTH_H
#define LIBYMGRAPHICS_WORDWIDTH_H (1)
//---------------------------------------------------------------------------
#include <stdio.h>
#include "ymgraphics.h"
//---------------------------------------------------------------------------
YM_NS_DEF(wordwidth)
//---------------------------------------------------------------------------
class Wordwidth
{
private:
        char* _wordBoundries;
public:
        Wordwidth();
        ~Wordwidth();

        float getWordWidth(const int& font, const char* str, const float& swidth);
        const char* getWordBoundries();
        void setWordBoundries(const char* boundries);

};
//---------------------------------------------------------------------------
YM_NS_END
//---------------------------------------------------------------------------

#endif /*LIBYMGRAPHICS_CHARWIDTH_H*/
