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
		int x = static_cast<int>(point.x() * _IMAGE_SIZE);
		int y = static_cast<int>(point.y() * _IMAGE_SIZE);
		image.put(x, y, color);
	}
}

static void draw_delaunay(const rnn::WorldGen& gen, scl::file::BMP_Image& image, int color)
{
	for (auto f = gen._dl.faces_begin(); f != gen._dl.faces_end(); f++)
	{
		int x0 = static_cast<int>(gen._dl.triangle(f)[0].x() * _IMAGE_SIZE);
		int y0 = static_cast<int>(gen._dl.triangle(f)[0].y() * _IMAGE_SIZE);
		int x1 = static_cast<int>(gen._dl.triangle(f)[1].x() * _IMAGE_SIZE);
		int y1 = static_cast<int>(gen._dl.triangle(f)[1].y() * _IMAGE_SIZE);
		int x2 = static_cast<int>(gen._dl.triangle(f)[2].x() * _IMAGE_SIZE);
		int y2 = static_cast<int>(gen._dl.triangle(f)[2].y() * _IMAGE_SIZE);
		image.poly({ { x0, y0 },{ x1, y1 },{ x2, y2 } }, _COLOR_DELAUNAY, color);
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
				int x0 = static_cast<int>(gen._dl.triangle(f)[0].x() * _IMAGE_SIZE);
				int y0 = static_cast<int>(gen._dl.triangle(f)[0].y() * _IMAGE_SIZE);
				int x1 = static_cast<int>(gen._dl.triangle(f)[1].x() * _IMAGE_SIZE);
				int y1 = static_cast<int>(gen._dl.triangle(f)[1].y() * _IMAGE_SIZE);
				int x2 = static_cast<int>(gen._dl.triangle(f)[2].x() * _IMAGE_SIZE);
				int y2 = static_cast<int>(gen._dl.triangle(f)[2].y() * _IMAGE_SIZE);
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
		int x0 = static_cast<int>(gen._slope[i].x() * _IMAGE_SIZE);
		int y0 = static_cast<int>(gen._slope[i].y() * _IMAGE_SIZE);
		int x1 = static_cast<int>(gen._slope[i + 1].x() * _IMAGE_SIZE);
		int y1 = static_cast<int>(gen._slope[i + 1].y() * _IMAGE_SIZE);
		image.line(x0, y0, x1, y1, color);

	}
}

static void draw_slope_path(const rnn::WorldGen& gen, scl::file::BMP_Image& image, int color)
{
	for (int i = 0; i < gen._slope_path.size() - 1; i++)
	{
		int x0 = static_cast<int>(gen._slope_path[i].x() * _IMAGE_SIZE);
		int y0 = static_cast<int>(gen._slope_path[i].y() * _IMAGE_SIZE);
		int x1 = static_cast<int>(gen._slope_path[i + 1].x() * _IMAGE_SIZE);
		int y1 = static_cast<int>(gen._slope_path[i + 1].y() * _IMAGE_SIZE);
		image.line(x0, y0, x1, y1, color);
	}
}

static void draw_voronoi(const rnn::WorldGen& gen, scl::file::BMP_Image& image, int color) {
	for (auto edge = gen._dl.finite_edges_begin(); edge != gen._dl.finite_edges_end(); ++edge) {
		auto face1 = edge->first;
		int edge_index = edge->second;
		auto face2 = face1->neighbor(edge_index);

		Point_2 voronoi_vertex1, voronoi_vertex2;
		bool has_vertex1 = false, has_vertex2 = false;

		//Get first Voronoi vertex (circumcenter of face1)
		if (!gen._dl.is_infinite(face1)) {
			voronoi_vertex1 = CGAL::circumcenter(face1->vertex(0)->point(), face1->vertex(1)->point(), face1->vertex(2)->point());
			has_vertex1 = true;
		}

		//Get second Voronoi vertex (circumcenter of face2)
		if (!gen._dl.is_infinite(face2)) {
			voronoi_vertex2 = CGAL::circumcenter(face2->vertex(0)->point(), face2->vertex(1)->point(), face2->vertex(2)->point());
			has_vertex2 = true;
		}

		if (has_vertex1 && has_vertex2) {
			//Finite Voronoi edge
			int x1 = static_cast<int>(voronoi_vertex1.x() * _IMAGE_SIZE);
			int y1 = static_cast<int>(voronoi_vertex1.y() * _IMAGE_SIZE);
			int x2 = static_cast<int>(voronoi_vertex2.x() * _IMAGE_SIZE);
			int y2 = static_cast<int>(voronoi_vertex2.y() * _IMAGE_SIZE);

			if (x1 >= 0 && x1 < _IMAGE_SIZE && y1 >= 0 && y1 < _IMAGE_SIZE &&
				x2 >= 0 && x2 < _IMAGE_SIZE && y2 >= 0 && y2 < _IMAGE_SIZE) {
				image.line(x1, y1, x2, y2, color);
			}
		}
	}
}

static int get_color_by_terrain(int terrain_type) {
	const int SEA_LEVELS = 10; //Number of levels for sea gradient
	const int LAND_LEVELS = 10; //Number of levels for land gradient
	//Sea gradient (negative numbers)
	if (terrain_type < 0) {
		//Map terrain values from -inf to 0 to a gradient from aqua to very dark blue
		int level = std::min(SEA_LEVELS - 1, static_cast<int>((-terrain_type * SEA_LEVELS) / SEA_LEVELS));
		float normalized = static_cast<float>(level) / (SEA_LEVELS - 1);
		int r = static_cast<int>(0 * normalized + 0 * (1 - normalized));  // 0 to 0
		int g = static_cast<int>(0 * normalized + 192 * (1 - normalized));  // 0 to 255
		int b = static_cast<int>(128 * normalized + 255 * (1 - normalized));  // 128 to 255
		return (r << 16) | (g << 8) | b;
	}
	else if (terrain_type == 0)
		return _YELLOW; //Color for coast (0)
	//Land gradient (positive numbers)
	else {
		//Map terrain values from 0 to inf to a gradient from green to dark green
		int level = std::min(LAND_LEVELS - 1, static_cast<int>((terrain_type * LAND_LEVELS) / LAND_LEVELS));
		float normalized = static_cast<float>(level) / (LAND_LEVELS - 1);
		int r = static_cast<int>(0 * (1 - normalized));  //0 to 0
		int g = static_cast<int>(255 * (1 - normalized) + 128 * normalized);  //255 to 128
		int b = static_cast<int>(0 * (1 - normalized));  //0 to 0
		return (r << 16) | (g << 8) | b;
	}
}

static void draw_points_terrain(rnn::WorldGen& gen, scl::file::BMP_Image& image)
{
	for (size_t i = 0; i < gen._point_cloud.size(); i++) {
		int x = static_cast<int>(gen._point_cloud[i].x() * _IMAGE_SIZE);
		int y = static_cast<int>(gen._point_cloud[i].y() * _IMAGE_SIZE);
		int color = get_color_by_terrain(gen._terrain[i]);
		image.put(x, y, color);
	}
}

static void draw_basic_terrain(rnn::WorldGen& gen, scl::file::BMP_Image& image)
{
	#pragma omp parallel for collapse(2)
	for (size_t i = 0; i < _IMAGE_SIZE; i++) {
		for (size_t j = 0; j < _IMAGE_SIZE; j++) {
			Point_2 pos = Point_2(static_cast<float>(i) / _IMAGE_SIZE, static_cast<float>(j) / _IMAGE_SIZE);
			//Find the closest point in the point cloud
			auto neighbor = gen._dl.nearest_vertex(pos);
			size_t idx = gen._points_index_map[neighbor];
			int color = get_color_by_terrain(gen._terrain[idx]);
			image.put(i, j, color);
		}
	}
}

static void draw_terrain(const rnn::WorldGen& gen, scl::file::BMP_Image& image) {
	for (auto vertex = gen._dl.finite_vertices_begin(); vertex != gen._dl.finite_vertices_end(); ++vertex) {
		std::vector<Point_2> voronoi_vertices;

		//Get all faces (triangles) incident to this vertex
		auto face_circulator = gen._dl.incident_faces(vertex);
		auto start = face_circulator;

		if (face_circulator != nullptr) {
			do {
				//Skip infinite faces
				if (!gen._dl.is_infinite(face_circulator)) {
					//Calculate circumcenter of the triangle (this is a Voronoi vertex)
					Point_2 circumcenter = CGAL::circumcenter(
						face_circulator->vertex(0)->point(),
						face_circulator->vertex(1)->point(),
						face_circulator->vertex(2)->point()
					);
					voronoi_vertices.push_back(circumcenter);
				}
				++face_circulator;
			} while (face_circulator != start);
		}

		//Get the terrain value for this vertex and determine the fill color
		auto it = gen._points_index_map.find(vertex);
		int terrain_value = gen._terrain[it->second];
		int fill_color = get_color_by_terrain(terrain_value);

		//Convert to screen coordinates and fill the polygon
		if (voronoi_vertices.size() >= 3) {
			std::vector<std::pair<int, int>> screen_vertices;

			for (const auto& v : voronoi_vertices) {
				int x = static_cast<int>(v.x() * _IMAGE_SIZE);
				int y = static_cast<int>(v.y() * _IMAGE_SIZE);

				//Clamp to image bounds
				x = std::max(0, std::min(_IMAGE_SIZE - 1, x));
				y = std::max(0, std::min(_IMAGE_SIZE - 1, y));

				screen_vertices.push_back({ x, y });
			}

			if (screen_vertices.size() >= 3) {
				image.poly_fill(screen_vertices, fill_color);
			}
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

		scl::file::BMP_Image image(_IMAGE_SIZE, _IMAGE_SIZE);

		draw_points(gen, image, _WHITE);
		draw_delaunay(gen, image, _WHITE);
		//draw_used_faces_by_slope(dl_all, gen._slope, path, image, BLUE);
		draw_slope(gen, image, _RED);
		draw_slope_path(gen, image, _PURPLE);
		image.save(path + "/path.bmp");
		image.clear();

		draw_delaunay(gen, image, _WHITE);
		draw_slope_path(gen, image, _PURPLE);
		draw_points_terrain(gen, image);
		image.save(path + "/terrain_points.bmp");
		draw_voronoi(gen, image, _AQUA);
		image.save(path + "/voronoi.bmp");
		draw_terrain(gen, image);
		image.save(path + "/terrain.bmp");
		draw_delaunay(gen, image, _WHITE);
		draw_slope_path(gen, image, _PURPLE);
		image.save(path + "/terrain_delaunay.bmp");
	}
	else 
	{
		if(select.size() > 1)
			std::cout << "Data not found for world" << select << "." << std::endl;
	}

	std::cout << "Closing in 5 seconds..";
	std::this_thread::sleep_for(std::chrono::seconds(5));
}