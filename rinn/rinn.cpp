#include "rinn.h"
#include "worldgen/worldgen.h"
#include <scroll.h>
#include <iostream>
#include <ctime>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <fstream>
#include <filesystem>
#include <string>

const int color_points		= 0xFFFFFF;
const int color_delaunay	= 0x303030;

int main()
{
	scl::System::init();
	rnn::WorldGen gen;
	scl::World world = gen.generate();

	std::cout << "Saving world.. ";
	std::string path = "../data/world";
	std::string sub_name = "world";
	int n_world = 1;
	for (auto & p : std::filesystem::directory_iterator(path)) {
		std::string f_name = std::filesystem::path(p).filename().string();
		std::size_t found = f_name.find(sub_name);
		if (found != std::string::npos)
			n_world++;
	}
	sub_name += std::to_string(n_world);
	path += "/" + sub_name;
	std::filesystem::create_directory(path);

	std::ofstream file(path + "/gen.dat", std::ios::binary | std::ios::out);
	cereal::BinaryOutputArchive archive(file);
	archive(gen);
	file.close();
	std::cout << "done" << std::endl;

	/*
	int imageSize = 1024;
	scl::file::BMP_Image image(imageSize, imageSize);

	std::cout << "Saving.. ";
	for (auto el : points)
	{
		int x = static_cast<int>(el.x() * imageSize);
		int y = static_cast<int>(el.y() * imageSize);
		image.put(x, y, 0xFFFFFF);
	}
	image.save("points.bmp");

	std::cout << "Saving.. ";
	for (auto f = t_all.faces_begin(); f != t_all.faces_end(); f++)
	{
		int x0 = static_cast<int>(t_all.triangle(f)[0].x() * imageSize);
		int y0 = static_cast<int>(t_all.triangle(f)[0].y() * imageSize);
		int x1 = static_cast<int>(t_all.triangle(f)[1].x() * imageSize);
		int y1 = static_cast<int>(t_all.triangle(f)[1].y() * imageSize);
		int x2 = static_cast<int>(t_all.triangle(f)[2].x() * imageSize);
		int y2 = static_cast<int>(t_all.triangle(f)[2].y() * imageSize);
		image.poly({ {x0, y0}, {x1, y1}, {x2, y2} }, color_delaunay, color_points);
	}
	image.save("delaunay.bmp");

	do {
		if (!t_all.is_infinite(line_face_circulator)) {
			auto f = line_face_circulator.handle();
			int x0 = static_cast<int>(t_all.triangle(f)[0].x() * imageSize);
			int y0 = static_cast<int>(t_all.triangle(f)[0].y() * imageSize);
			int x1 = static_cast<int>(t_all.triangle(f)[1].x() * imageSize);
			int y1 = static_cast<int>(t_all.triangle(f)[1].y() * imageSize);
			int x2 = static_cast<int>(t_all.triangle(f)[2].x() * imageSize);
			int y2 = static_cast<int>(t_all.triangle(f)[2].y() * imageSize);
			image.poly({ { x0, y0 },{ x1, y1 },{ x2, y2 } }, 0x0000FF);
			last_face = line_face_circulator;
		}
	} while (++line_face_circulator != first_face);

	std::cout << "Saving.. ";
	for (int i = 0; i < slope_path.size() - 1; i++)
	{
		int x0 = static_cast<int>(slope_path[i].x() * imageSize);
		int y0 = static_cast<int>(slope_path[i].y() * imageSize);
		int x1 = static_cast<int>(slope_path[i + 1].x() * imageSize);
		int y1 = static_cast<int>(slope_path[i + 1].y() * imageSize);
		image.line(x0, y0, x1, y1, 0x00FF00);
	}
	image.line((int)(slope_or.x() * imageSize), (int)(slope_or.y() * imageSize), (int)(slope_dr.x() * imageSize), (int)(slope_dr.y() * imageSize), 0xFF0000);

	image.save("random_slope.bmp");
	*/
	TCODConsole::root->flush();
	TCODConsole::root->waitForKeypress(true);

	return 0;
}