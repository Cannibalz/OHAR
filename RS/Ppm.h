// Ppm.h
// =====
// PPM Reader and Writer
// It reads 8-bit ASCII or binary PPM image from a file, and save a pixmap as
// 8-bit binary PPM format.
//
// Use read(const char* name) to load a PPM image.
// Use save(const char* name, int w, int h, const unsigned char* data) to save.
//
//  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// CREATED: 2006-06-06
// UPDATED: 2006-06-07
///////////////////////////////////////////////////////////////////////////////

#ifndef IMAGE_PPM_H
#define IMAGE_PPM_H

#include <fstream>
using std::ifstream;

namespace Image
{
    class Ppm
    {
    public:
        // ctor/dtor
        Ppm();
        Ppm(const Ppm& rhs);
        ~Ppm();

        Ppm& operator=(const Ppm &rhs);             // assignment operator

        // load image header and pixmap from file
        bool read(const char* fileName);

        // save an image as PPM
        static bool save(const char* fileName, int width, int height, const unsigned char* data);

        // getters
        int getWidth() const;                       // return image width
        int getHeight() const;                      // return image height
        int getMaxValue() const;                    // return the max value in data
        int getDataSize() const;                    // return data size in bytes
        const unsigned char* getData() const;       // return the pointer to image data

        void printSelf() const;                     // print itself for debug purpose


    private:
        // member functions
        void init();                                // clear all member data
        static bool readDataAscii(ifstream& file, int dataSize, unsigned char* data);
        static bool readDataBinary(ifstream& file, int dataSize, unsigned char* data);

        int width;
        int height;
        int maxValue;
        int dataSize;
        unsigned char* data;
    };



    ///////////////////////////////////////////////////////////////////////////
    // inline functions
    ///////////////////////////////////////////////////////////////////////////
    inline int Ppm::getWidth() const { return width; }
    inline int Ppm::getHeight() const { return height; }

    inline int Ppm::getMaxValue() const { return maxValue; }

    inline int Ppm::getDataSize() const { return dataSize; }
    inline const unsigned char* Ppm::getData() const { return data; }
}

#endif
