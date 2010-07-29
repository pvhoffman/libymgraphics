#ifndef LIBYMGRAPHICS_BITMAP_H
#define LIBYMGRAPHICS_BITMAP_H (1)
//---------------------------------------------------------------------------
#include <stdio.h>
#include "ymgraphics.h"
//---------------------------------------------------------------------------
YM_NS_DEF(bitmap)
class Bitmap
{
private:
        FILE* _file;
        char* _fileName;
        BITMAPFILEHEADER _bmpfileheader;
        BITMAPINFOHEADER _bmpinfoheader;

        void init();
        void dealloc();

public:
        Bitmap();
        ~Bitmap();

        BITMAPFILEHEADER& getBitmapFileHeader();
        BITMAPINFOHEADER& getBitmapInfoHeader();
        unsigned int getPadding() const;
        unsigned char* getBitmapBits(const int& pad) const;

        void open(const char* fileName);

};
YM_NS_END
//---------------------------------------------------------------------------
#endif /*LIBYMGRAPHICS_BITMAP_H*/
