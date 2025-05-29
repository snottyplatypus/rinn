#include "data_visualizer.h"
#include "worldgen/worldgen.h"
#include <scroll.h>
#include <filesystem>
#include <string>
#include <chrono>
#include <thread>
#include <fstream>
#include <cereal/archives/binary.hpp>

static void draw_points(const rnn::WorldGen& gen, scl::file::BMP_Image& image, int color)
{
	for (const auto& point : gen._point_cloud)
	{
		int x = static_cast<int>(point.x() * IMAGE_SIZE);
		int y = static_cast<int>(point.y() * IMAGE_SIZE);
		image.put(x, y, color);
	}
}

static void draw_delaunay(const rnn::WorldGen& gen, scl::file::BMP_Image& image, int color)
{
	for (auto f = gen._dl.faces_begin(); f != gen._dl.faces_end(); f++)
	{
		int x0 = static_cast<int>(gen._dl.triangle(f)[0].x() * IMAGE_SIZE);
		int y0 = static_cast<int>(gen._dl.triangle(f)[0].y() * IMAGE_SIZE);
		int x1 = static_cast<int>(gen._dl.triangle(f)[1].x() * IMAGE_SIZE);
		int y1 = static_cast<int>(gen._dl.triangle(f)[1].y() * IMAGE_SIZE);
		int x2 = static_cast<int>(gen._dl.triangle(f)[2].x() * IMAGE_SIZE);
		int y2 = static_cast<int>(gen._dl.triangle(f)[2].y() * IMAGE_SIZE);
		image.poly({ { x0, y0 },{ x1, y1 },{ x2, y2 } }, COLOR_DELAUNAY, color);
	}
}

static void draw_used_faces_by_slope(const rnn::WorldGen& gen, scl::file::BMP_Image& image, int color) {
	for (int i = 0; i < gen._point_cloud.size() - 1; i++)
	{
		auto line_face_circulator = gen._dl.line_walk(gen._slope[i], gen._slope[i + 1]);
		auto first_face = line_face_circulator;
		auto last_face = line_face_circulator;
		do {
			if (!gen._dl.is_infinite(line_face_circulator)) {
				auto f = line_face_circulator.handle();
				int x0 = static_cast<int>(gen._dl.triangle(f)[0].x() * IMAGE_SIZE);
				int y0 = static_cast<int>(gen._dl.triangle(f)[0].y() * IMAGE_SIZE);
				int x1 = static_cast<int>(gen._dl.triangle(f)[1].x() * IMAGE_SIZE);
				int y1 = static_cast<int>(gen._dl.triangle(f)[1].y() * IMAGE_SIZE);
				int x2 = static_cast<int>(gen._dl.triangle(f)[2].x() * IMAGE_SIZE);
				int y2 = static_cast<int>(gen._dl.triangle(f)[2].y() * IMAGE_SIZE);
				image.poly({ { x0, y0 },{ x1, y1 },{ x2, y2 } }, color);
				last_face = line_face_circulator;
			}
		} while (++line_face_circulator != first_face);
	}
}

static void draw_slope(const rnn::WorldGen& gen, scl::file::BMP_Image& image, int color)
{
	for (size_t i = 0; i < gen._slope.size() - 1; i++)
	{
		int x0 = static_cast<int>(gen._slope[i].x() * IMAGE_SIZE);
		int y0 = static_cast<int>(gen._slope[i].y() * IMAGE_SIZE);
		int x1 = static_cast<int>(gen._slope[i + 1].x() * IMAGE_SIZE);
		int y1 = static_cast<int>(gen._slope[i + 1].y() * IMAGE_SIZE);
		image.line(x0, y0, x1, y1, color);

	}
}

static void draw_slope_path(const rnn::WorldGen& gen, scl::file::BMP_Image& image, int color)
{
	for (int i = 0; i < gen._slope_path.size() - 1; i++)
	{
		int x0 = static_cast<int>(gen._slope_path[i].x() * IMAGE_SIZE);
		int y0 = static_cast<int>(gen._slope_path[i].y() * IMAGE_SIZE);
		int x1 = static_cast<int>(gen._slope_path[i + 1].x() * IMAGE_SIZE);
		int y1 = static_cast<int>(gen._slope_path[i + 1].y() * IMAGE_SIZE);
		image.line(x0, y0, x1, y1, color);
	}
}

static void draw_voronoi(const rnn::WorldGen& gen, scl::file::BMP_Image& image, int color)
{
	//Track which edges we've already drawn to avoid duplicates
	std::set<std::pair<Point_2, Point_2>> drawn_edges;

	//For each face in the Delaunay triangulation
	for (auto f = gen._dl.faces_begin(); f != gen._dl.faces_end(); ++f) {
		// Skip infinite faces
		if (gen._dl.is_infinite(f))
			continue;

		//Calculate circumcenter manually (Voronoi vertex)
		const Point_2& p1 = f->vertex(0)->point();
		const Point_2& p2 = f->vertex(1)->point();
		const Point_2& p3 = f->vertex(2)->point();
		//Simple arithmetic mean as an approximation for display purposes
		double center_x_d = (p1.x() + p2.x() + p3.x()) / 3.0;
		double center_y_d = (p1.y() + p2.y() + p3.y()) / 3.0;
		//Convert to image coordinates
		int center_x = static_cast<int>(center_x_d * IMAGE_SIZE);
		int center_y = static_cast<int>(center_y_d * IMAGE_SIZE);

		//Draw Voronoi edges to adjacent faces (which are connected by shared edges)
		for (int i = 0; i < 3; ++i) {
			auto neighbor = f->neighbor(i);
			//Skip edges to infinite faces
			if (gen._dl.is_infinite(neighbor))
				continue;

			//Calculate center of neighbor triangle (approximation)
			const Point_2& n1 = neighbor->vertex(0)->point();
			const Point_2& n2 = neighbor->vertex(1)->point();
			const Point_2& n3 = neighbor->vertex(2)->point();
			double neighbor_x_d = (n1.x() + n2.x() + n3.x()) / 3.0;
			double neighbor_y_d = (n1.y() + n2.y() + n3.y()) / 3.0;
			//Convert to image coordinates
			int neighbor_x = static_cast<int>(neighbor_x_d * IMAGE_SIZE);
			int neighbor_y = static_cast<int>(neighbor_y_d * IMAGE_SIZE);
			//Create a unique representation of this edge
			Point_2 p_center(center_x_d, center_y_d);
			Point_2 p_neighbor(neighbor_x_d, neighbor_y_d);

			//Ensure consistent ordering for the pair to avoid duplicates
			std::pair<Point_2, Point_2> edge;
			if (p_center < p_neighbor)
				edge = std::make_pair(p_center, p_neighbor);
			else
				edge = std::make_pair(p_neighbor, p_center);

			//Only draw if we haven't drawn this edge before
			if (drawn_edges.find(edge) == drawn_edges.end()) {
				//Check if points are within bounds of the image
				if (center_x >= 0 && center_x < IMAGE_SIZE &&
					center_y >= 0 && center_y < IMAGE_SIZE &&
					neighbor_x >= 0 && neighbor_x < IMAGE_SIZE &&
					neighbor_y >= 0 && neighbor_y < IMAGE_SIZE)
				{
					image.line(center_x, center_y, neighbor_x, neighbor_y, color);
					drawn_edges.insert(edge);
				}
			}
		}
	}
}

static void draw_points_terrain(rnn::WorldGen& gen, scl::file::BMP_Image& image)
{
	int color = WHITE;
	for (size_t i = 0; i < gen._point_cloud.size(); i++) {
		int x = static_cast<int>(gen._point_cloud[i].x() * IMAGE_SIZE);
		int y = static_cast<int>(gen._point_cloud[i].y() * IMAGE_SIZE);
		if (gen._terrain[i] == 1)
			color = GREEN;
		else if (gen._terrain[i] == 0)
			color = YELLOW;
		else if (gen._terrain[i] == -1)
			color = AQUA;
		else if (gen._terrain[i] == -2)
			color = BLUE;
		else
			color = WHITE;
		image.put(x, y, color);
	}
}

static void draw_basic_terrain(rnn::WorldGen& gen, scl::file::BMP_Image& image)
{
	#pragma omp parallel for collapse(2)
	for (size_t i = 0; i < IMAGE_SIZE; i++) {
		for (size_t j = 0; j < IMAGE_SIZE; j++) {
			Point_2 pos = Point_2(static_cast<float>(i) / IMAGE_SIZE, static_cast<float>(j) / IMAGE_SIZE);
			// Find the closest point in the point cloud
			auto neighbor = gen._dl.nearest_vertex(pos);
			size_t idx = gen._points_index_map[neighbor];
			int color;
			if (gen._terrain[idx] == 1)
				color = GREEN;
			else if (gen._terrain[idx] == 0)
				color = YELLOW;
			else if (gen._terrain[idx] == -1)
				color = AQUA;
			else if (gen._terrain[idx] == -2)
				color = BLUE;
			else
				color = WHITE;
			image.put(i, j, color);
		}
	}
}

int main()
{
	std::cout << "Select the world you want to generate data for :" << std::endl;
	std::string path = "../data/world";
	std::string sub_name = "world";
	int n_world = 0;
	//Display all sub names
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
	if (n_world > 0) { //Check that there is at least one world saved
		bool i = false;
		while (!i) {
			std::cin >> select;
			//i is false if select is not a number or if it's a number but not between 1 and n_world
			i = std::all_of(select.begin(), select.end(), [](unsigned char c) { return std::isdigit(c); });
			if (!i || i && (std::stoi(select) == 0 || std::stoi(select) > n_world)) {
				i = false;
				std::cout << "Invalid input, please enter a number between 1 and " << n_world << std::endl;
			}

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
		
		scl::file::BMP_Image image(IMAGE_SIZE, IMAGE_SIZE);

		draw_points(gen, image, WHITE);
		image.save(path + "/points.bmp");
		draw_delaunay(gen, image, WHITE);
		//draw_used_faces_by_slope(dl_all, gen._slope, path, image, BLUE);
		draw_slope(gen, image, RED);
		draw_slope_path(gen, image, YELLOW);
		image.save(path + "/path.bmp");
		image.clear();
		draw_delaunay(gen, image, WHITE);
		draw_slope_path(gen, image, YELLOW);
		draw_points_terrain(gen, image);
		image.save(path + "/terrain_points.bmp");
		draw_voronoi(gen, image, AQUA);
		image.save(path + "/voronoi.bmp");
		image.clear();
		draw_basic_terrain(gen, image);
		image.save(path + "/terrain.bmp");
	}
	else 
	{
		if(select.size() > 1)
			std::cout << "Data not found for world" << select << "." << std::endl;
	}

	std::cout << "Closing in 5 seconds..";
	std::this_thread::sleep_for(std::chrono::seconds(5));
}