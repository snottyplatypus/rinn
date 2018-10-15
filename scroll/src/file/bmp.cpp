#include "file/bmp.h"
#include <fstream>
#include <iostream>

unsigned char* LoadBMP(const char* FileName, int* OutWidth, int* OutHeight)
{
	sBMPHeader Header;

	std::ifstream File(FileName, std::ifstream::binary);

	File.read((char*)&Header, sizeof(Header));

	*OutWidth = Header.biWidth;
	*OutHeight = Header.biHeight;

	size_t DataSize = 3 * Header.biWidth * Header.biHeight;

	unsigned char* Img = new unsigned char[DataSize];

	File.read((char*)Img, DataSize);

	return Img;
}

void SaveBMP(const char* FileName, const void* RawBGRImage, int Width, int Height)
{
	sBMPHeader Header;

	int ImageSize = Width * Height * 3;

	Header.bfType = 0x4D * 256 + 0x42;
	Header.bfSize = ImageSize + sizeof(sBMPHeader);
	Header.bfReserved1 = 0;
	Header.bfReserved2 = 0;
	Header.bfOffBits = 0x36;
	Header.biSize = 40;
	Header.biWidth = Width;
	Header.biHeight = Height;
	Header.biPlanes = 1;
	Header.biBitCount = 24;
	Header.biCompression = 0;
	Header.biSizeImage = ImageSize;
	Header.biXPelsPerMeter = 6000;
	Header.biYPelsPerMeter = 6000;
	Header.biClrUsed = 0;
	Header.biClrImportant = 0;

	std::ofstream File(FileName, std::ios::out | std::ios::binary);

	File.write((const char*)&Header, sizeof(Header));
	File.write((const char*)RawBGRImage, ImageSize);

	std::cout << "Saved " << FileName << std::endl;
}