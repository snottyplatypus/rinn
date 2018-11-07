#include "data_visualizer.h"
#include "worldgen/worldgen.h"
#include <scroll.h>
#include <filesystem>
#include <string>
#include <chrono>
#include <thread>
#include <fstream>
#include <cereal/archives/binary.hpp>

int main()
{
	std::cout << "Select the world you want to generate data for :" << std::endl;
	std::string path = "../data/world";
	std::string sub_name = "world";
	int n_world = 0;
	for (auto & p : std::filesystem::directory_iterator(path)) {
		std::string f_name = std::filesystem::path(p).filename().string();
		std::size_t found = f_name.find(sub_name);
		if (found != std::string::npos) {
			std::cout << "* " << f_name << std::endl;
			n_world++;
		}
	}

	bool data_found = false;
	std::string data = "gen.dat";
	std::string select = "";
	if (n_world > 0) {
		std::cin >> select;
		while (std::stoi(select) == 0 || std::stoi(select) > n_world) {
			std::cout << "Invalid input.." << std::endl;
			std::cin >> select;
		}
		path += "/" + sub_name + select;

		for (auto & p : std::filesystem::directory_iterator(path)) {
			std::string f_name = std::filesystem::path(p).filename().string();
			std::size_t found = f_name.find(data);
			if (found != std::string::npos) {
				std::cout << "Opening data.." << std::endl;
				data_found = true;
			}
		}
	}
	else {
		std::cout << "No world found." << std::endl;
		data_found = false;
	}

	if (data_found) 
	{
		std::ifstream file(path + "/" + data, std::ios::binary | std::ios::out);
		cereal::BinaryInputArchive archive(file);
		rnn::WorldGen gen;
		archive(gen);

		path += "/visualizer";
		std::filesystem::create_directory(path);
		
		scl::file::BMP_Image image(imageSize, imageSize);

		for (auto el : gen._point_cloud)
		{
			int x = static_cast<int>(el.x() * imageSize);
			int y = static_cast<int>(el.y() * imageSize);
			image.put(x, y, 0xFFFFFF);
		}
		image.save(path + "/points.bmp");

		Delaunay dl_all;
		dl_all.insert(gen._point_cloud.begin(), gen._point_cloud.end());
		for (auto f = dl_all.faces_begin(); f != dl_all.faces_end(); f++)
		{
			int x0 = static_cast<int>(dl_all.triangle(f)[0].x() * imageSize);
			int y0 = static_cast<int>(dl_all.triangle(f)[0].y() * imageSize);
			int x1 = static_cast<int>(dl_all.triangle(f)[1].x() * imageSize);
			int y1 = static_cast<int>(dl_all.triangle(f)[1].y() * imageSize);
			int x2 = static_cast<int>(dl_all.triangle(f)[2].x() * imageSize);
			int y2 = static_cast<int>(dl_all.triangle(f)[2].y() * imageSize);
			image.poly({ { x0, y0 },{ x1, y1 },{ x2, y2 } }, color_delaunay, color_points);
		}
		image.save(path + "/delaunay.bmp");

		auto line_face_circulator = dl_all.line_walk(gen._slope[0], gen._slope[1]);
		auto first_face = line_face_circulator;
		auto last_face = line_face_circulator;
		do {
			if (!dl_all.is_infinite(line_face_circulator)) {
				auto f = line_face_circulator.handle();
				int x0 = static_cast<int>(dl_all.triangle(f)[0].x() * imageSize);
				int y0 = static_cast<int>(dl_all.triangle(f)[0].y() * imageSize);
				int x1 = static_cast<int>(dl_all.triangle(f)[1].x() * imageSize);
				int y1 = static_cast<int>(dl_all.triangle(f)[1].y() * imageSize);
				int x2 = static_cast<int>(dl_all.triangle(f)[2].x() * imageSize);
				int y2 = static_cast<int>(dl_all.triangle(f)[2].y() * imageSize);
				image.poly({ { x0, y0 }, { x1, y1 }, { x2, y2 } }, 0x0000FF);
				last_face = line_face_circulator;
			}
		} while (++line_face_circulator != first_face);

		for (int i = 0; i < gen._slope_path.size() - 1; i++)
		{
			int x0 = static_cast<int>(gen._slope_path[i].x() * imageSize);
			int y0 = static_cast<int>(gen._slope_path[i].y() * imageSize);
			int x1 = static_cast<int>(gen._slope_path[i + 1].x() * imageSize);
			int y1 = static_cast<int>(gen._slope_path[i + 1].y() * imageSize);
			image.line(x0, y0, x1, y1, 0x00FF00);
		}

		for(size_t i = 0; i < gen._slope.size() - 1; i++) 
		image.line((int)(gen._slope[i].x() * imageSize), (int)(gen._slope[i].y() * imageSize), (int)(gen._slope[i + 1].x() * imageSize), (int)(gen._slope[i + 1].y() * imageSize), 0xFF0000);

		image.save(path + "/slope.bmp");
	}
	else 
	{
		if(select.size() > 1)
			std::cout << "Data not found for world" << select << "." << std::endl;
	}

	std::cout << "Closing in 5 seconds..";
	std::this_thread::sleep_for(std::chrono::seconds(5));
}