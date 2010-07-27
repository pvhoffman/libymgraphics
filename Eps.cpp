//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdexcept>
#include <sstream>
#pragma hdrstop
#include "Eps.h"
//---------------------------------------------------------------------------
YM_NS_DEF(eps)
//---------------------------------------------------------------------------
Eps::Eps()
{
        _file = 0;
        _fileName = 0;
}
//---------------------------------------------------------------------------
Eps::~Eps()
{
        if (_file) fclose(_file);
        _file = 0;

        if (_fileName) free(_fileName);
        _fileName = 0;
}
//---------------------------------------------------------------------------
void Eps::open(const char* fileName)
{
        if (!fileName || !(*fileName))
                throw std::runtime_error("Invalid file name.");
        _file = fopen(fileName, "rb");
        if (!_file)
                throw std::runtime_error(std::string("Cannot open file ") + std::string(fileName));

}
//---------------------------------------------------------------------------
unsigned long Eps::getLength()
{
        YM_NS_USE(global);
        unsigned long l = 0UL;
        if (!_file)
                throw std::runtime_error("No eps file open.");
        fseek(_file, 0, SEEK_SET);
        fread(&l, sizeof(unsigned long), 1, _file);
        if (l == kTiffHeader) {
                fseek(_file, sizeof(unsigned long), SEEK_CUR);
                fread(&l, sizeof(unsigned long), 1, _file);
        } else {
                fseek(_file, 0, SEEK_END);
                l = ftell(_file);
        }
        return l;
}
//---------------------------------------------------------------------------
unsigned long Eps::getPosition()
{
        YM_NS_USE(global);
        unsigned long p = 0UL;
        if (!_file)
                throw std::runtime_error("No eps file open.");
        fseek(_file, 0, SEEK_SET);
        fread(&p, sizeof(unsigned long), 1, _file);
        if (p == kTiffHeader) {
                fread(&p, sizeof(unsigned long), 1, _file);
        }
        return p;
}
//---------------------------------------------------------------------------
void Eps::positionToBeginning()
{
        if (!_file)
                throw std::runtime_error("No eps file open.");
        unsigned long pos = getPosition();
        fseek(_file, pos, SEEK_SET);
}
//---------------------------------------------------------------------------
BoundingBox_t Eps::getBoundingBox()
{
        YM_NS_USE(util);
        BoundingBox_t bbox;
        char buffer[EPSBufferSize];
        const char *sbox = "BoundingBox:";
        const char *cptr;

        if (!_file)
                throw std::runtime_error("No eps file open.");
        positionToBeginning();
        while (readLine(_file, buffer, EPSBufferSize) != EOF) {
                if (buffer[0] && buffer[0] != '%') break; //past the comment section
                if ((cptr = strstr(buffer, sbox)) != NULL) {
                        cptr = (cptr + strlen(sbox));
                        int llx, lly, urx, ury;
                        if (sscanf(cptr, "%d %d %d %d", &llx, &lly, &urx, &ury) == 4) {
                                bbox.left   = llx;
                                bbox.right  = urx;
                                bbox.top    = ury;
                                bbox.bottom = lly;
                                break;
                        }
                }
        }
        if (bbox.width() <= 0.0f || bbox.height() <= 0.0f)
                throw std::runtime_error("Cannot locate bounding box in eps file.");
        return bbox;
}
//---------------------------------------------------------------------------
void Eps::rawSendToFile(FILE *fd)
{
        unsigned long length, i, j, k;
        char buffer[EPSBufferSize];

        if (!_file)
                throw std::runtime_error("No eps file open.");
        length = getLength();
        positionToBeginning();
        j = (length / EPSBufferSize);
        for (i=0;i<j;i++) {
                k = fread(buffer, 1, EPSBufferSize, _file);
                fwrite(buffer, 1, k, fd);
        }
        j = (length % EPSBufferSize);
        if (j) {
                k = fread(buffer, 1, j, _file);
                fwrite(buffer, 1, k, fd);
        }
}
//---------------------------------------------------------------------------
void Eps::sendToFile(FILE *fd, const float& scaleX, const float& scaleY, const float& angle)
{
        std::stringstream ss;
        std::string outs;
        BoundingBox_t bbox = getBoundingBox();

        ss << "\n gsave"
        << "\n mark"
        << "\n /level0 save def"
        << "\n /showpage {} def"
        << "\n currentpoint translate " << bbox.left << " " << bbox.bottom << " neg translate"
        << "\n " << scaleX << " " << scaleY << " scale"
        << "\n " << angle << " rotate"
        << "\n false setoverprint"
        << "\n%%BeginDocument: " << nameOnly() << "\n";
        outs.assign(ss.str());
        fwrite(outs.c_str(), 1, outs.length(), fd);

        rawSendToFile(fd);

        ss << "\n%%EndDocument"
        << "\n cleartomark"
        << "\n level0 restore\n";
        outs.assign(ss.str());
        fwrite(outs.c_str(), 1, outs.length(), fd);
}
//---------------------------------------------------------------------------
const char* Eps::nameOnly()
{
        const char *cptr = (_fileName + (strlen(_fileName) - 1));
        static char buffer[MAX_PATH];

        while (cptr >= _fileName) {
                if (*(cptr-1) == '\\') break;
                cptr--;
        }
        strcpy(buffer, cptr);
        return &buffer[0];
}
//---------------------------------------------------------------------------
YM_NS_END
