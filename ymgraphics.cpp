#include <vcl.h>
#include <stdexcept>
#include <sstream>
#pragma hdrstop


#include "Eps.h"
#include "Bitmap.h"
#include "Ghostscript.h"

#include "ymgraphics.h"

//---------------------------------------------------------------------------
// global namespace
// for global variables
YM_NS_DEF(global)
const unsigned long kTiffHeader = 0xC6D3D0C5UL;
YM_NS_END
//---------------------------------------------------------------------------
// util namespace
// for utility functions
YM_NS_DEF(util)
//---------------------------------------------------------------------------
// Return the ghostscript device name from the requested type
static const char* getGhostscriptDeviceName(const int& device)
{
        static char buffer[1024];

        buffer[0] = 0;
        switch (device) {
        case YM_NS_SCOPE(types)rtBmp16:
                strcpy(buffer, "-sDEVICE=bmp16");
                break;
        case YM_NS_SCOPE(types)rtBmp16m:
        default:
                strcpy(buffer, "-sDEVICE=bmp16m");
                break;
        case YM_NS_SCOPE(types)rtBmp256:
                strcpy(buffer, "-sDEVICE=bmp256");
                break;
        case YM_NS_SCOPE(types)rtBmp32b:
                strcpy(buffer, "-sDEVICE=bmp32b");
                break;
        case YM_NS_SCOPE(types)rtBmpgray:
                strcpy(buffer, "-sDEVICE=bmpgray");
                break;
        case YM_NS_SCOPE(types)rtBmpmono:
                strcpy(buffer, "-sDEVICE=bmpmono");
                break;
        case YM_NS_SCOPE(types)rtJpeg:
                strcpy(buffer, "-sDEVICE=jpeg");
                break;
        case YM_NS_SCOPE(types)rtJpegcmyk:
                strcpy(buffer, "-sDEVICE=jpegcmyk");
                break;
        case YM_NS_SCOPE(types)rtJpeggray:
                strcpy(buffer, "-sDEVICE=jpeggray");
                break;
        case YM_NS_SCOPE(types)rtPdfwrite:
                strcpy(buffer, "-sDEVICE=pdfwrite");
                break;
        case YM_NS_SCOPE(types)rtPng16:
                strcpy(buffer, "-sDEVICE=png16");
                break;
        case YM_NS_SCOPE(types)rtPng16m:
                strcpy(buffer, "-sDEVICE=png16m");
                break;
        case YM_NS_SCOPE(types)rtPng256:
                strcpy(buffer, "-sDEVICE=png256");
                break;
        case YM_NS_SCOPE(types)rtPngalpha:
                strcpy(buffer, "-sDEVICE=pngalpha");
                break;
        case YM_NS_SCOPE(types)rtPnggray:
                strcpy(buffer, "-sDEVICE=pnggray");
                break;
        case YM_NS_SCOPE(types)rtPngmono:
                strcpy(buffer, "-sDEVICE=pngmono");
                break;
        case YM_NS_SCOPE(types)rtTxtwrite:
                strcpy(buffer, "-sDEVICE=txtwrite");
                break;
        }
        return &buffer[0];
}
//---------------------------------------------------------------------------
// True if raster graphic type
static bool isRasterType(const int& t)
{
        switch (t) {
        case YM_NS_SCOPE(types)rtBmp16:
        case YM_NS_SCOPE(types)rtBmp16m:
        case YM_NS_SCOPE(types)rtBmp256:
        case YM_NS_SCOPE(types)rtBmp32b:
        case YM_NS_SCOPE(types)rtBmpgray:
        case YM_NS_SCOPE(types)rtBmpmono:
        case YM_NS_SCOPE(types)rtJpeg:
        case YM_NS_SCOPE(types)rtJpegcmyk:
        case YM_NS_SCOPE(types)rtJpeggray:
        case YM_NS_SCOPE(types)rtPng16:
        case YM_NS_SCOPE(types)rtPng16m:
        case YM_NS_SCOPE(types)rtPng256:
        case YM_NS_SCOPE(types)rtPngalpha:
        case YM_NS_SCOPE(types)rtPnggray:
        case YM_NS_SCOPE(types)rtPngmono:
                return true;

        }
        return false;
}
//---------------------------------------------------------------------------
int readLine(FILE* fd, char* buffer, const int& length)
{
        int i = 0, j;

        buffer[0] = 0;
        while (true) {
                if ( i >= (length-1)) {
                        buffer[i] = 0;
                        return i;
                }
                j = fgetc(fd);
                switch (j) {
                case EOF:
                        if (i) {
                                buffer[i] = 0;
                                return i;
                        }
                        return EOF;
                case '\n':
                        if (i) {
                                if (buffer[i-1] == '\r') buffer[--i] = 0;
                                else buffer[i] = 0;
                                return i;
                        }
                        break;
                case '\r':
                        buffer[i] = 0;
                        return i;
                default:
                        buffer[i++] = j;
                        break;

                }
        }
}
//---------------------------------------------------------------------------
// Convert a postscript file to a supported type (see rtBmp16 - rtTxtwrite)
void convertPostscript(const char* psfn, const char* ofn, const float& sx
                       , const float& sy, const int& outputType)
{
        enum {
                kMaxGhostscriptArgs = 25
                , kMaxGhostscriptArgLength = 260
        };

        int gsargc = 0, i;
        char* gsargv[kMaxGhostscriptArgs] = {NULL,};
        YM_NS_SCOPE(ghostscript)Ghostscript *gs = 0;

        try {
                gsargv[gsargc++] = strdup("ym");
                gsargv[gsargc++] = strdup("-dBATCH");
                gsargv[gsargc++] = strdup("-dNOPAUSE");
                gsargv[gsargc++] = strdup("-dSAFER");

                gsargv[gsargc]   = (char*)malloc(kMaxGhostscriptArgLength);
                sprintf(gsargv[gsargc++], "-r%dx%d"
                        , ((int)(72.0f * sx))
                        , ((int)(72.0f * sy)));

                if (isRasterType(outputType)) {
                        gsargv[gsargc++] = strdup("-dTextAlphaBits=4");
                        gsargv[gsargc++] = strdup("-dGraphicsAlphaBits=4");
                }

                gsargv[gsargc++] = strdup( getGhostscriptDeviceName(outputType) );

                gsargv[gsargc]   = (char*)malloc(kMaxGhostscriptArgLength);
                sprintf(gsargv[gsargc++], "-sOutputFile=%s", ofn);

                gsargv[gsargc++] = strdup(psfn);
                gsargv[gsargc]   = NULL;

                gs = new YM_NS_SCOPE(ghostscript)Ghostscript();
                i = gs->gsdll_init( YM_NS_SCOPE(ghostscript)gsdll_callback
                                    , NULL
                                    , gsargc
                                    , gsargv);

                if (!i)
                        gs->gsdll_exit();
                else
                        throw std::runtime_error(std::string("Ghostscript execution failed on file "
                                                             + std::string(psfn)));
        }
        __finally {
                if (gs) delete gs;
                gs = 0;
                for (i=0;gsargv[i];i++) {
                        free(gsargv[i]);
                        gsargv[i] = 0;
                }
        }
}
//---------------------------------------------------------------------------
// Convert an eps object to a postscript file
// output type is
static void convertEpsToPostscript(YM_NS_SCOPE(eps)Eps *eps, const char* psout
                                   , const float &sx, const float& sy)
{
        FILE *fd = 0;

        try {
                fd = fopen(psout, "wb");
                fputs("\n 0 0 moveto", fd);
                eps->sendToFile(fd, sx, sy, 0.0f);
                fputs("\n showpage\n", fd);
        }
        __finally {
                if (fd) fclose(fd);
                fd = 0;
        }
}
//---------------------------------------------------------------------------
// Convert an eps file to outputType
// output type is
void convertEps(const char* epsin, const char* ofn, const float& sx, const float &sy
                , const int& outputType)
{
        enum {
                kMaxGhostscriptArgs = 25
                , kMaxGhostscriptArgLength = 260
                , kMaxBufferLength = 1024
        };
        YM_NS_SCOPE(eps)Eps *eps = 0;
        YM_NS_SCOPE(types)BoundingBox_t boundingBox;
        char buff[kMaxBufferLength];
        const char *cptr;

        try {
                eps = new YM_NS_SCOPE(eps)Eps();
                eps->open(epsin);

                boundingBox = eps->getBoundingBox();

                //convert the eps to postscript
                cptr = getenv("temp");
                if (!cptr || !(*cptr))
                        throw std::runtime_error("TEMP environment variable not set.");
                strcpy(buff, cptr);
                if (buff[strlen(buff)-1] != '\\') strcat(buff, "\\");
                strcat(buff, "out.ps");


                if (outputType == YM_NS_SCOPE(types)rtPostscript) { // no need to go any further
                        convertEpsToPostscript(eps, buff, sx, sy);
                        ::MoveFileEx(buff, ofn, (MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH));
                } else {
                        int gsargc = 0, i;
                        char *gsargv[kMaxGhostscriptArgs] = {NULL,};
                        YM_NS_SCOPE(ghostscript)Ghostscript *gs = 0;

                        try {
                                convertEpsToPostscript(eps, buff, 1.0f, 1.0f);

                                gsargv[gsargc++] = strdup("ym");
                                gsargv[gsargc++] = strdup("-dBATCH");
                                gsargv[gsargc++] = strdup("-dNOPAUSE");
                                gsargv[gsargc++] = strdup("-dSAFER");

                                gsargv[gsargc]   = (char*)malloc(kMaxGhostscriptArgLength);
                                sprintf(gsargv[gsargc++], "-r%dx%d"
                                        , ((int)(72.0f * sx))
                                        , ((int)(72.0f * sy)));

                                gsargv[gsargc]   = (char*)malloc(kMaxGhostscriptArgLength);
                                sprintf(gsargv[gsargc++], "-g%dx%d"
                                        , ((int)(boundingBox.width() * sx))
                                        , ((int)(boundingBox.height() * sy)));

                                if (isRasterType(outputType)) {
                                        gsargv[gsargc++] = strdup("-dTextAlphaBits=4");
                                        gsargv[gsargc++] = strdup("-dGraphicsAlphaBits=4");
                                }

                                gsargv[gsargc++] = strdup( getGhostscriptDeviceName(outputType) );

                                gsargv[gsargc]   = (char*)malloc(kMaxGhostscriptArgLength);
                                sprintf(gsargv[gsargc++], "-sOutputFile=%s", ofn);

                                gsargv[gsargc++] = strdup(buff);

                                gsargv[gsargc]   = NULL;

                                gs = new YM_NS_SCOPE(ghostscript)Ghostscript();
                                i = gs->gsdll_init( YM_NS_SCOPE(ghostscript)gsdll_callback
                                                    , NULL
                                                    , gsargc
                                                    , gsargv);

                                if (!i)
                                        gs->gsdll_exit();
                                else
                                        throw std::runtime_error(std::string("Ghostscript execution failed on file "
                                                                             + std::string(epsin)));




                        }
                        __finally {
                                for (i=0;gsargv[i];i++) {
                                        free(gsargv[i]);
                                        gsargv[i] = 0;
                                }
                                if (gs) delete gs;
                                gs = 0;

                        }
                }
        }
        __finally {
                if (eps) delete eps;
                eps = 0;
        }

}
//---------------------------------------------------------------------------
YM_NS_END
//---------------------------------------------------------------------------


