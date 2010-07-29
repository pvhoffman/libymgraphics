#ifndef LIBYMGRAPHICS_EPS_H
#define LIBYMGRAPHICS_EPS_H (1)

#include <stdio.h>

#include "ymgraphics.h"

YM_NS_DEF(eps)

class Eps
{
private:
        FILE* _file;
        char* _fileName;

        enum {
                EPSBufferSize = 61440
        };

        const char* nameOnly();

public:
        Eps();
        ~Eps();
        void open(const char* fileName);
        unsigned long getLength();
        unsigned long getPosition();
        void positionToBeginning();
        YM_NS_SCOPE(types)BoundingBox_t getBoundingBox();
        void rawSendToFile(FILE *fd);
        void sendToFile(FILE *fd, const float& scaleX, const float& scaleY, const float& angle);

};
YM_NS_END
#endif  /*LIBYMGRAPHICS_EPS_H*/
