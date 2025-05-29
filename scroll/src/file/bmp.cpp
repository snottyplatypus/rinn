#include "file/bmp.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <limits>

scl::file::BMP_Image::BMP_Image(int width, int height) : _width(width), _height(height)
{
	_buffer.resize((_width + 1) * (_height + 1) * 3);
	std::fill(_buffer.begin(), _buffer.end(), 0x00);
}

scl::file::BMP_Image::~BMP_Image()
{
}

void scl::file::BMP_Image::clear()
{
	std::fill(_buffer.begin(), _buffer.end(), 0x00);
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
	const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
	if (steep)
	{
		std::swap(x1, y1);
		std::swap(x2, y2);
	}

	if (x1 > x2)
	{
		std::swap(x1, x2);
		std::swap(y1, y2);
	}

	const double dx = static_cast<double>(x2 - x1);
	const double dy = fabs(y2 - y1);

	double error = dx / 2.0f;
	const int ystep = (y1 < y2) ? 1 : -1;
	int y = (int)y1;

	const int maxX = (int)x2;

	for (int x = (int)x1; x < maxX; x++)
	{
		if (steep)
		{
			put(y, x, rgb);
		}
		else
		{
			put(x, y, rgb);
		}

		error -= dy;
		if (error < 0)
		{
			y += ystep;
			error += dx;
		}
	}
}

void scl::file::BMP_Image::poly(std::vector<std::pair<int, int>> points, int rgb_edges)
{
	size_t l = points.size() - 1;
	for (size_t i = 0; i < l; i++)
		line(points[i].first, points[i].second, points[i + 1].first, points[i + 1].second, rgb_edges);
	line(points[l].first, points[l].second, points[0].first, points[0].second, rgb_edges);
}

void scl::file::BMP_Image::poly(std::vector<std::pair<int, int>> points, int rgb_edges, int rgb_vertices)
{
	size_t l = points.size() - 1;
	for (size_t i = 0; i < l; i++)
		line(points[i].first, points[i].second, points[i + 1].first, points[i + 1].second, rgb_edges);
	line(points[l].first, points[l].second, points[0].first, points[0].second, rgb_edges);

	for (auto p : points)
		put(p.first, p.second, rgb_vertices);
}

void scl::file::BMP_Image::poly_fill(std::vector<std::pair<int, int>> points, int rgb_fill)
{
	//Find bounding box
	int min_y = std::numeric_limits<int>::max();
	int max_y = std::numeric_limits<int>::min();

	for (const auto& point : points) {
		min_y = std::min(min_y, point.second);
		max_y = std::max(max_y, point.second);
	}

	//Clamp to image bounds
	min_y = std::max(0, min_y);
	max_y = std::min(_height - 1, max_y);

	//For each scanline
	for (int y = min_y; y <= max_y; y++) {
		std::vector<int> intersections;

		//Find intersections of scanline with polygon edges
		size_t n = points.size();
		for (size_t i = 0; i < n; i++) {
			size_t j = (i + 1) % n;

			int y1 = points[i].second;
			int y2 = points[j].second;
			int x1 = points[i].first;
			int x2 = points[j].first;

			//Skip horizontal edges
			if (y1 == y2) continue;

			//Check if scanline intersects this edge
			if ((y >= y1 && y < y2) || (y >= y2 && y < y1)) {
				//Calculate intersection x-coordinate
				double x_intersect = x1 + (double)(y - y1) * (x2 - x1) / (y2 - y1);
				intersections.push_back((int)x_intersect);
			}
		}

		//Sort intersections
		std::sort(intersections.begin(), intersections.end());

		//Fill between pairs of intersections
		for (size_t i = 0; i < intersections.size(); i += 2) {
			if (i + 1 < intersections.size()) {
				int x_start = std::max(0, intersections[i]);
				int x_end = std::min(_width - 1, intersections[i + 1]);

				for (int x = x_start; x <= x_end; x++) {
					put(x, y, rgb_fill);
				}
			}
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