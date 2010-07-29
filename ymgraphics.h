#ifndef LIBYMGRAPHICS_YMGRAPHICS_H
#define LIBYMGRAPHICS_YMGRAPHICS_H (1)

#include <stdio.h>

#define YM_NS_DEF(sub) namespace ym{ namespace graphics{ namespace sub{

#define YM_NS_END }}}

#define YM_NS_USE(sub) using namespace ym::graphics::sub;

#define YM_NS_SCOPE(sub) ym::graphics::sub::

//---------------------------------------------------------------------------
// types namespace
// for type classes
YM_NS_DEF(types)
//---------------------------------------------------------------------------
// EPS Bounding Box
struct BoundingBox_t {
        float left;
        float top;
        float right;
        float bottom;

        BoundingBox_t() : left(0.0f), top(0.0f), right(0.0f), bottom(0.0f) {
        }
        BoundingBox_t(const BoundingBox_t &rhs) : left(rhs.left), top(rhs.top)
                        , right(rhs.right), bottom(rhs.bottom) {
        }

        float width() const {
                return (right - left);
        }
        float height() const {
                return (top - bottom);
        }
};
//---------------------------------------------------------------------------
// Ghotscript callback
typedef int (* GhostscriptCallback_t)(int message, char *str, unsigned long count);
//---------------------------------------------------------------------------
// Bitmap bit counts
enum {
        bmpBitCount0 = 0
        , bmpBitCount1 = 1
        , bmpBitCount4 = 4
        , bmpBitCount8 = 8
        , bmpBitCount16 = 16
        , bmpBitCount24 = 24
        , bmpBitCount32 = 32
};
//---------------------------------------------------------------------------
// Bitmap padding options
enum {
        bmpWithPadding
        , bmpWithoutPadding
};
//---------------------------------------------------------------------------
// Ghostscript messages
enum {
        GSDLL_STDIN    = 1
        , GSDLL_STDOUT = 2
        , GSDLL_DEVICE = 3
        , GSDLL_SYNC   = 4
        , GSDLL_PAGE   = 5
        , GSDLL_SIZE   = 6
        , GSDLL_POLL   = 7
};
//---------------------------------------------------------------------------
// Ghostscript conversion types - not all are supported
enum {
        rtBmp16 = 1
        , rtBmp16m
        , rtBmp256
        , rtBmp32b
        , rtBmpgray
        , rtBmpmono
        , rtJpeg
        , rtJpegcmyk
        , rtJpeggray
        , rtPdfwrite
        , rtPng16
        , rtPng16m
        , rtPng256
        , rtPngalpha
        , rtPnggray
        , rtPngmono
        , rtTxtwrite
        , rtPostscript
};
//---------------------------------------------------------------------------
// Supported fonts for postscript character widthing
enum {
  fntHelvetica = 1
  , fntHelveticaBlackCondensed
};

//---------------------------------------------------------------------------
YM_NS_END // types

//---------------------------------------------------------------------------
// global namespace
// for global variables
YM_NS_DEF(global)
extern const unsigned long kTiffHeader;
extern const unsigned short helveticaCharWidth[255];
extern const unsigned short helveticaBlackCondensedCharWidth[255];
YM_NS_END
//---------------------------------------------------------------------------
// util namespace
// for utility functions
YM_NS_DEF(util)
//---------------------------------------------------------------------------
// Read a line from a file
int readLine(FILE* fd, char* buffer, const int& length);
//---------------------------------------------------------------------------
// Convert a postscript file to one of the above (rtBmp16 - rtTxtwrite) types
void convertPostscript(const char* psfn, const char* ofn, const float& sx, const float& sy, const int& outputType);
void convertEps(const char* epsin, const char* ofn, const float& sx, const float &sy, const int& outputType);
YM_NS_END
//---------------------------------------------------------------------------
#endif /*LIBYMGRAPHICS_YMGRAPHICS_H*/
