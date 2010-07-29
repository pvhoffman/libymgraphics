//---------------------------------------------------------------------------
#include <vcl.h>
#include <stdexcept>
#include <sstream>
#pragma hdrstop

#include "Bitmap.h"
//---------------------------------------------------------------------------
YM_NS_DEF(bitmap)
Bitmap::Bitmap()
{
        init();
}
//---------------------------------------------------------------------------
Bitmap::~Bitmap()
{
        dealloc();
}
//---------------------------------------------------------------------------
void Bitmap::dealloc()
{
        if (_file) fclose(_file);
        _file = 0;

        if (_fileName) free(_fileName);
        _fileName = 0;
}
//---------------------------------------------------------------------------
void Bitmap::init()
{
        // Remove any resources currently in use
        dealloc();

        // Set up the structs
        ::ZeroMemory((void*)&_bmpfileheader, sizeof(BITMAPFILEHEADER));
        ::ZeroMemory((void*)&_bmpinfoheader, sizeof(BITMAPINFOHEADER));
        _bmpfileheader.bfType = 0x4D42;
        _bmpinfoheader.biSize = sizeof(BITMAPINFOHEADER);
}
//---------------------------------------------------------------------------
// By reference accessors
BITMAPFILEHEADER& Bitmap::getBitmapFileHeader()
{
        return _bmpfileheader;
}
//---------------------------------------------------------------------------
BITMAPINFOHEADER& Bitmap::getBitmapInfoHeader()
{
        return _bmpinfoheader;
}
//---------------------------------------------------------------------------
// Open an existing bitmap
void Bitmap::open(const char* fileName)
{
        // Reset the object
        init();

        if (!fileName || !(*fileName))
                throw std::runtime_error("Invalid file name.");
        _fileName = strdup(fileName);

        _file = fopen(fileName, "rb");
        if (!(_file))
                throw std::runtime_error(std::string("Cannot open file ") + std::string(fileName));

        // Read the file header
        if (!fread((void*)&_bmpfileheader, sizeof(BITMAPFILEHEADER), 1, _file))
                throw std::runtime_error("Cannot read bitmap file header.");

        // Read the info header
        if (!fread((void*)&_bmpinfoheader, sizeof(BITMAPINFOHEADER), 1, _file))
                throw std::runtime_error("Cannot read bitmap info header.");

        // Check the info header for supported types.
        if (_bmpinfoheader.biBitCount != YM_NS_SCOPE(types)bmpBitCount24)
                throw std::runtime_error("Unsupported bitmap type");
}
//---------------------------------------------------------------------------
// Get the number of padding bytes per row
unsigned int Bitmap::getPadding() const
{
        unsigned int padding = 4 - ((_bmpinfoheader.biWidth * 3) % 4);
        if (padding == 4) padding = 0;
        return padding;

}
//---------------------------------------------------------------------------
// Get the bitmap bits
unsigned char* Bitmap::getBitmapBits(const int& pad) const
{
        unsigned char *bits = 0;
        unsigned int byteCount;
        unsigned int padding = getPadding();

        switch (pad) {
        case YM_NS_SCOPE(types)bmpWithPadding:
                byteCount = ((_bmpinfoheader.biWidth * 3) + padding) * _bmpinfoheader.biHeight;
                break;
        case YM_NS_SCOPE(types)bmpWithoutPadding:
                byteCount = ((_bmpinfoheader.biWidth * 3) + 0) * _bmpinfoheader.biHeight;
                break;
        default:
                throw std::runtime_error("Unknown bitmap padding option.");
        }

        bits = (unsigned char*)malloc(byteCount);
        if (!bits)
                throw std::runtime_error("Cannot allocate memory for bitmap bits.");
        fseek(_file, _bmpfileheader.bfOffBits, SEEK_SET);

        if (pad == YM_NS_SCOPE(types)bmpWithPadding || padding == 0) {
                // Read in one operation if padding is included or padding is 0
                if (byteCount != fread(bits, 1, byteCount, _file))
                        throw std::runtime_error("Bitmap read operation failed");
        } else {
                // Read a row at a time
                unsigned int rowSize = _bmpinfoheader.biWidth * 3;
                for (int y = 0;y < _bmpinfoheader.biHeight;y++) {
                        fread(&bits[rowSize*y], rowSize, 1, _file);
                        fseek(_file, padding, SEEK_CUR);
                }
        }
        return bits;
}
YM_NS_END
