#ifndef LIBYMGRAPHICS_YMGRAPHICS_H
#define LIBYMGRAPHICS_YMGRAPHICS_H (1)

#include <stdio.h>

#define YM_NS_DEF(sub) namespace ym{ namespace graphics{ namespace sub{

#define YM_NS_END }}}

#define YM_NS_USE(sub) using namespace ym::graphics::sub;

//---------------------------------------------------------------------------
// global namespace
// for global variables
YM_NS_DEF(global)
extern const unsigned long kTiffHeader;
YM_NS_END
//---------------------------------------------------------------------------
// util namespace
// for utility functions
YM_NS_DEF(util)
int readLine(FILE* fd, char* buffer, const int& length);
YM_NS_END
//---------------------------------------------------------------------------
#endif /*LIBYMGRAPHICS_YMGRAPHICS_H*/
