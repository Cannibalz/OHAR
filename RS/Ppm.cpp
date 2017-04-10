// Ppm.cpp
// =======
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

#include "Ppm.h"
#include <fstream>
#include <iostream>
#include <string>
#include <cstring>

using std::string;
using std::ifstream;
using std::ofstream;
using std::ios;
using std::cout;
using std::endl;
using namespace Image;



///////////////////////////////////////////////////////////////////////////////
// default ctor
///////////////////////////////////////////////////////////////////////////////
Ppm::Ppm() : width(0), height(0), maxValue(0), dataSize(0), data(0)
{
}




///////////////////////////////////////////////////////////////////////////////
// copy contructor
// We need DEEP COPY for dynamic memory variables because the compiler inserts
// default copy constructor automatically for you, BUT it is only SHALLOW COPY
///////////////////////////////////////////////////////////////////////////////
Ppm::Ppm(const Ppm& rhs)
{
    // copy member variables from right-hand-side object
    width = rhs.getWidth();
    height = rhs.getHeight();
    maxValue = rhs.getMaxValue();
    dataSize = rhs.getDataSize();

    if(rhs.getData())       // allocate memory only if the pointer is not NULL
    {
        data = new unsigned char[dataSize];
        memcpy(data, rhs.getData(), dataSize); // deep copy
    }
    else
        data = 0;           // array is not allocated yet, set to 0
}



///////////////////////////////////////////////////////////////////////////////
// destructor
///////////////////////////////////////////////////////////////////////////////
Ppm::~Ppm()
{
    // deallocate data array
    delete [] data;
    data = 0;
}



///////////////////////////////////////////////////////////////////////////////
// override assignment operator
///////////////////////////////////////////////////////////////////////////////
Ppm& Ppm::operator=(const Ppm &rhs)
{
    if(this == &rhs)        // avoid self-assignment (A = A)
        return *this;

    // copy member variables
    width = rhs.getWidth();
    height = rhs.getHeight();
    maxValue = rhs.getMaxValue();
    dataSize = rhs.getDataSize();

    if(rhs.getData())       // allocate memory only if the pointer is not NULL
    {
        data = new unsigned char[dataSize];
        memcpy(data, rhs.getData(), dataSize);
    }
    else
        data = 0;

    return *this;
}



///////////////////////////////////////////////////////////////////////////////
// print itself
///////////////////////////////////////////////////////////////////////////////
void Ppm::printSelf() const
{
    cout << "===== Ppm =====\n"
         << "Width: " << width << " pixels.\n"
         << "Height: " << height << " pixels.\n"
         << "Max Value: " << maxValue << "\n"
         << "Data Size: " << dataSize << " bytes.\n"
         << endl;
}



///////////////////////////////////////////////////////////////////////////////
// clear out the exsiting values
///////////////////////////////////////////////////////////////////////////////
void Ppm::init()
{
    width = height = maxValue = dataSize = 0;
    delete [] data;
    data = 0;
}



///////////////////////////////////////////////////////////////////////////////
// read a PPM image header infos and pixmap data from a file
///////////////////////////////////////////////////////////////////////////////
bool Ppm::read(const char* fileName)
{
    this->init();   // clear out all values

    // check NULL pointer
    if(!fileName)
        return false;

    // open a file as binary mode
    ifstream inFile;
    inFile.open(fileName, ios::binary);         // binary mode
    if(!inFile.good())
    {
        inFile.close();
        return false;                           // exit if failed
    }

    const int MAX_STRING = 256;
    string magicId;                             // magic number
    int headerInfos[3];                         // 3 header infos(width, height, maxValue)
    int index;
    char nextChar;                              // next character to peek
    bool isAscii;                               // is data stored ASCII or binary
    char comment[MAX_STRING];                   // comment line to remove

    // get and validate magic number
    inFile >> magicId;
    if(magicId == "P3")                         // color and ascii
        isAscii = true;
    else if(magicId == "P6")                    // color and binary
        isAscii = false;
    else                                        // unknown magic number
    {
        inFile.close();
        return false;
    }

    // loop until 3 header infos are found
    index = 0;
    while(!inFile.eof() && index < 3)
    {
        nextChar = inFile.peek();

        if(nextChar == '#')                     // remove comment line if there is
            inFile.getline(comment, MAX_STRING);
        else if(nextChar <= ' ')                // trim leading white spaces
            inFile.ignore();
        else                                    // copy header info
            inFile >> headerInfos[index++];
    }

    // store image width, height, maxValue
    width = headerInfos[0];
    height = headerInfos[1];
    maxValue = headerInfos[2];

    // compute data size, PPM has RGB components
    dataSize = width * height * 3;

    // allocate buffer for pixmap
    data = new unsigned char[dataSize];

    // load pixmap data
    bool returnValue = false;
    if(isAscii)
        returnValue = readDataAscii(inFile, dataSize, data);
    else
        returnValue = readDataBinary(inFile, dataSize, data);

    // close file after reading
    inFile.close();

    return returnValue;
}




///////////////////////////////////////////////////////////////////////////////
// read ASCII pixmap data
///////////////////////////////////////////////////////////////////////////////
bool Ppm::readDataAscii(ifstream& file, int dataSize, unsigned char* data)
{
    const int MAX_STRING = 256;
    char comment[MAX_STRING];
    char nextChar;
    short value;
    int index = 0;

    // The cursor should be point to the beginning of data, but there might be a comment line left
    // remove the following comment lines
    while(1)
    {
        nextChar = file.peek();
        if(nextChar <= ' ')                     // trim leading white spaces
            file.ignore();
        else if(nextChar =='#')
            file.getline(comment, MAX_STRING);
        else
            break;
    }

    while(index < dataSize)
    {
        file >> value;
        data[index++] = (unsigned char)value;
        if(file.eof())                          // make sure it is reached EOF
            return false;
    }

    return true;
}



///////////////////////////////////////////////////////////////////////////////
// read binary pixmap data
///////////////////////////////////////////////////////////////////////////////
bool Ppm::readDataBinary(ifstream& file, int dataSize, unsigned char* data)
{
    // compute the cursor position to read a block
    file.seekg(0, ios::end);
    std::streampos position = file.tellg() - std::streampos(dataSize);
    file.seekg(position);

    file.read((char*)data, dataSize);     // read a data block

    return true;
}



///////////////////////////////////////////////////////////////////////////////
// save a pixmap as PPM binary
///////////////////////////////////////////////////////////////////////////////
bool Ppm::save(const char* fileName, int width, int height, const unsigned char* data)
{
    if(!fileName || !data) return false;

    // open output file
    ofstream outFile;
    outFile.open(fileName, ios::binary);
    if(!outFile.good())
    {
        outFile.close();
        return false;   // exit if failed
    }

    // write header
    outFile << "P6" << endl
            << width << endl
            << height << endl
            << 255 << endl;

    // write data as binary
    outFile.write((char*)data, width*height*3);

    // close the opened file
    outFile.close();

    return true;
}
