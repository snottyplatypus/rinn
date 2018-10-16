#ifndef BMP_H
#define BMP_H

#include <cstdint>
#include <vector>
#include <string>

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

namespace scl
{
	namespace file
	{
		class BMP_Image
		{
		public:
			BMP_Image(int width = 0, int height = 0);
			~BMP_Image();

			void save(std::string fileName);

			void put(int x, int y, int rgb);
			void line(int x1, int y1, int x2, int y2, int rgb);

		private:
			std::vector<unsigned char> _buffer;
			int _width;
			int _height;
		};
		std::vector<unsigned char> loadBMP(const char* fileName, int* outWidth, int* outHeight);
		void saveBMP(const char* fileName, const void* rawBGRImage, int width, int height);
	}
}

#endif