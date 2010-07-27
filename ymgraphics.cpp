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
YM_NS_END
//---------------------------------------------------------------------------


