#ifndef LIBYMGRAPHICS_CHARWIDTH_H
#define LIBYMGRAPHICS_CHARWIDTH_H (1)
//---------------------------------------------------------------------------
#include <stdio.h>
#include "ymgraphics.h"
//---------------------------------------------------------------------------
YM_NS_DEF(charwidth)
//---------------------------------------------------------------------------
class Charwidth
{
private:
  
public:
        Charwidth();
        ~Charwidth();

        float getCharWidth(const int& font, const char& c, const float& swidth);

};
//---------------------------------------------------------------------------
YM_NS_END
//---------------------------------------------------------------------------

#endif /*LIBYMGRAPHICS_CHARWIDTH_H*/
