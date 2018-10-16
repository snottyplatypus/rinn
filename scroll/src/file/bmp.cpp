#include "file/bmp.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>

scl::file::BMP_Image::BMP_Image(int width, int height) : _width(width), _height(height)
{
	_buffer.resize(_width * _height * 3);
	std::fill(_buffer.begin(), _buffer.end(), 0x00);
}

scl::file::BMP_Image::~BMP_Image()
{
}

void scl::file::BMP_Image::save(std::string fileName)
{
	saveBMP(fileName.c_str(), _buffer.data(), _width, _height);
}

void scl::file::BMP_Image::put(int x, int y, int rgb)
{
	int pos = (x + y * _width) * 3;
	int r = (rgb & 0xFF0000) >> 16;
	int g = (rgb & 0x00FF00) >> 8;
	int b = (rgb) & 0x0000FF;
	_buffer[pos + 0] = b;
	_buffer[pos + 1] = g;
	_buffer[pos + 2] = r;
}

void scl::file::BMP_Image::line(int x1, int y1, int x2, int y2, int rgb)
{
	float dx = x2 - x1;
	float dy = y2 - y1;
	float derr = std::abs(dy / dx);
	float err = 0.0f;
	int y = y1;
	for (int x = x1; x <= x2; x++)
	{
		put(x, y, rgb);
		err += derr;
		if (err >= 0.5f) {
			int sign_dy = (dy > 0) ? 1 : ((dy < 0) ? -1 : 0);
			y = y + sign_dy;
			err -= 1.0f;
		}
	}
}

std::vector<unsigned char> scl::file::loadBMP(const char* fileName, int* outWidth, int* outHeight)
{
	sBMPHeader header;

	std::ifstream file(fileName, std::ifstream::binary);

	file.read((char*)&header, sizeof(header));

	*outWidth = header.biWidth;
	*outHeight = header.biHeight;

	size_t dataSize = 3 * header.biWidth * header.biHeight;

	std::vector<unsigned char> img(dataSize);
	unsigned char* buffer = new unsigned char[dataSize];

	file.read((char*)buffer, dataSize);

	img.assign(buffer, buffer + dataSize);
	delete[] buffer;

	return img;
}

void scl::file::saveBMP(const char* fileName, const void* rawBGRImage, int width, int height)
{
	sBMPHeader header;

	int imageSize = width * height * 3;

	header.bfType = 0x4D * 256 + 0x42;
	header.bfSize = imageSize + sizeof(sBMPHeader);
	header.bfReserved1 = 0;
	header.bfReserved2 = 0;
	header.bfOffBits = 0x36;
	header.biSize = 40;
	header.biWidth = width;
	header.biHeight = height;
	header.biPlanes = 1;
	header.biBitCount = 24;
	header.biCompression = 0;
	header.biSizeImage = imageSize;
	header.biXPelsPerMeter = 6000;
	header.biYPelsPerMeter = 6000;
	header.biClrUsed = 0;
	header.biClrImportant = 0;

	std::ofstream file(fileName, std::ios::out | std::ios::binary);

	file.write((const char*)&header, sizeof(header));
	file.write((const char*)rawBGRImage, imageSize);

	std::cout << "Saved " << fileName << std::endl;
}