#ifndef BMP_H
#define BMP_H

#include <cstdint>

#if defined( __GNUC__ )
#	define GCC_PACK(n) __attribute__((packed,aligned(n)))
#else
#	define GCC_PACK(n) __declspec(align(n))
#endif // __GNUC__

#pragma pack(push, 1)
struct sBMPHeader
{
	// BITMAPFILEHEADER
	unsigned short    bfType;
	uint32_t          bfSize;
	unsigned short    bfReserved1;
	unsigned short    bfReserved2;
	uint32_t          bfOffBits;
	// BITMAPINFOHEADER
	uint32_t          biSize;
	uint32_t          biWidth;
	uint32_t          biHeight;
	unsigned short    biPlanes;
	unsigned short    biBitCount;
	uint32_t          biCompression;
	uint32_t          biSizeImage;
	uint32_t          biXPelsPerMeter;
	uint32_t          biYPelsPerMeter;
	uint32_t          biClrUsed;
	uint32_t          biClrImportant;
};
#pragma pack(pop)

unsigned char* LoadBMP(const char* fileName, int* outWidth, int* outHeight);
void SaveBMP(const char* fileName, const void* rawBGRImage, int width, int height);

#endif