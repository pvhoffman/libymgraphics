#ifndef LIBYMGRAPHICS_EPS_H
#define LIBYMGRAPHICS_EPS_H (1)

#include <stdio.h>

#include "ymgraphics.h"

YM_NS_DEF(eps)

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
                return (bottom - top);
        }
};
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
        BoundingBox_t getBoundingBox();
        void rawSendToFile(FILE *fd);
        void sendToFile(FILE *fd, const float& scaleX, const float& scaleY, const float& angle);

};
YM_NS_END
#endif  /*LIBYMGRAPHICS_EPS_H*/
