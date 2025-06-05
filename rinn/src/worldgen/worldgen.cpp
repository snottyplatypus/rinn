#include "worldgen/worldgen.h"
#include <ctime>
#include <utility>
#include <algorithm>
#include <set>
#include <iostream>
#include <fstream>
#include <queue>
#include <filesystem>
#include <cereal/archives/binary.hpp>

static const int LAND = 1;
static const int SEA = -2;

rnn::WorldGen::WorldGen()
{
}

rnn::WorldGen::~WorldGen()
{
}

/*
	This function generate a world using Poisson Generation and Delaunay triangulation :
	1- Generate points using Poisson Generation
	2- Triangulate all points using Delaunay triangulation
	3- Generate a slope that will be used to generate terrain around it
	4- Generation terrain around slope path based on proximity to the path
	5- Create elevation based on the terrain
*/
void rnn::WorldGen::generate(scl::World& wrld)
{
	//Load config file and initialize random with seed
	std::ifstream file("../data/config/worldgen.json");
	cereal::JSONInputArchive archive(file);
	archive(cereal::make_nvp("worldgen", _data_config));
	if (_data_config._seed >= 0) {
		scl::rand->seed(static_cast<uint32_t>(_data_config._seed));
		std::cout << "Using seed : " << _data_config._seed << std::endl;
	}

	std::cout << "Generating points.. ";
	std::clock_t timer = clock();
	//Generate points using Poisson Generation
	if(_point_cloud.size() == 0)
		_point_cloud = scl::PoissonGenerator::generatePoissonPoints(std::numeric_limits<int>::max(), *scl::rand, 10, false, _data_config._points_min_dist);
	std::cout << "done " << (clock() - timer) / (CLOCKS_PER_SEC / 1000) << "ms" << std::endl;
	std::cout << "Points generated : " << _point_cloud.size() << std::endl;

	std::cout << "Triangulating points.. ";
	timer = clock();
	//Triangulate all points using Delaunay triangulation
	_dl.insert(_point_cloud.begin(), _point_cloud.end());
	std::cout << "done " << (clock() - timer) / (CLOCKS_PER_SEC / 1000) << "ms" << std::endl;
	std::cout << "Faces generated : " << _dl.number_of_faces() << std::endl;

	std::cout << "Indexing points.. ";
	timer = clock();
	//Create points index map for future usage
	for (auto& vh : _dl.finite_vertex_handles()) {
		auto it = std::find(_point_cloud.begin(), _point_cloud.end(), vh->point());
		size_t idx = std::distance(_point_cloud.begin(), it);
		_points_index_map[vh] = idx;
	}
	std::cout << "done " << (clock() - timer) / (CLOCKS_PER_SEC / 1000) << "ms" << std::endl;

	//Generate slope from one edge to another
	std::cout << "Generating slope.. ";
	timer = clock();
	std::vector<Point_2> unsorted_slope;
	unsorted_slope.push_back(random_point_on_edge());
	for (int i = 0; i < _data_config._slope_min + scl::rand->randomInt(_data_config._slope_max - _data_config._slope_min); i++)
		unsorted_slope.push_back(random_point());
	unsorted_slope.push_back(random_point_on_edge());
	
	//Sort slope points by distance to the first point and do it for all points
	while(!unsorted_slope.empty()) 
	{
		std::sort(unsorted_slope.begin(), unsorted_slope.end(), [&](const Point_2& a, const Point_2& b) -> bool
			{
				return CGAL::squared_distance(unsorted_slope[0], a) < CGAL::squared_distance(unsorted_slope[0], b);
			});
		_slope.push_back(unsorted_slope[0]);
		unsorted_slope.erase(unsorted_slope.begin());
	}

	//Generate slope path
	for (size_t i = 0; i < _slope.size() - 1; i++)
	{
		std::set<Point_2> slope_graph_set;
		auto line_face_circulator = _dl.line_walk(_slope[i], _slope[i + 1]);
		auto first_face = line_face_circulator;
		auto last_face = line_face_circulator;
		try {
			do {
				if (line_face_circulator != nullptr) {
					if (!_dl.is_infinite(line_face_circulator)) {
						auto f = line_face_circulator.handle();
						slope_graph_set.insert(_dl.triangle(f)[0]);
						slope_graph_set.insert(_dl.triangle(f)[1]);
						slope_graph_set.insert(_dl.triangle(f)[2]);
						last_face = line_face_circulator;
					}
				}
			} while (++line_face_circulator != first_face);
		}
		catch (CGAL::Precondition_exception& e) {
			std::cerr << "Error: " << e.what() << std::endl;
		}
		std::vector<Point_2> slope_graph(slope_graph_set.begin(), slope_graph_set.end());
		Delaunay dl_slope(slope_graph_set.begin(), slope_graph_set.end());
		std::vector<Point_2> temp_slope_path = scl::dijkstra(dl_slope, slope_graph, _slope[i], _slope[i + 1]);
		_slope_path.insert(_slope_path.end(), temp_slope_path.begin(), temp_slope_path.end());
	}
	std::cout << "done " << (clock() - timer) / (CLOCKS_PER_SEC / 1000) << "ms" << std::endl;

	//Generate terrain around path
	std::cout << "Generating terrain.. ";
	timer = clock();
	_terrain = mark_points_by_path_proximity();
	std::cout << "done " << (clock() - timer) / (CLOCKS_PER_SEC / 1000) << "ms" << std::endl;

	//Create elevation
	std::cout << "Generating elevation.. ";
	timer = clock();
	create_elevation();
	std::cout << "done " << (clock() - timer) / (CLOCKS_PER_SEC / 1000) << "ms" << std::endl;

	saveNew(wrld);
}

/*
	This function generate a random point in the generated world inside boundaries
*/
Point_2 rnn::WorldGen::random_point()
{
	float r_x_min = 0.0f + _data_config._boundaries._x_min;
	float r_x_max = 1.0f - _data_config._boundaries._x_max;
	float r_y_min = 0.0f + _data_config._boundaries._y_min;
	float r_y_max = 1.0f - _data_config._boundaries._y_max;
	return Point_2(scl::rand->randomFloat(r_x_min, r_x_max), scl::rand->randomFloat(r_y_min, r_y_max));
}

/*
	This function generate a random point on the edge of the generated world inside boundaries
*/
Point_2 rnn::WorldGen::random_point_on_edge()
{
	float r_x = scl::rand->randomFloat(0.0f + _data_config._boundaries._x_min, 1.0f - _data_config._boundaries._x_max);
	float r_y = scl::rand->randomFloat(0.0f + _data_config._boundaries._y_min, 1.0f - _data_config._boundaries._y_max);
	float p = scl::rand->randomFloat();
	if (p < 0.5f)
		if (p < 0.25f)
			return Point_2(r_x, 0.0f + _data_config._boundaries._y_min);
		else
			return Point_2(r_x, 1.0f - _data_config._boundaries._x_max);
	else
		if (p < 0.75f)
			return Point_2(0.0f + _data_config._boundaries._x_min, r_y);
		else
			return Point_2(1.0f - _data_config._boundaries._x_max, r_y);
}

/*
	This function mark points as land or water based on their proximity to the path
	- Points that are part of the path are marked as 2 (land)
	- Points that are within a certain distance from the path are marked as 2 (land) depending on a fixed probability and threshold
	- Points that are not within distance are marked as 1 (sea)
*/
std::vector<int> rnn::WorldGen::mark_points_by_path_proximity()
{
	std::vector<int> point_marks(_point_cloud.size(), -3); // Default mark is -3 (void)

	//Create a set of path points for faster lookup
	std::set<Point_2> path_set(_slope_path.begin(), _slope_path.end());

	//Mark all path points as land
	for (size_t i = 0; i < _point_cloud.size(); ++i) {
		if (path_set.find(_point_cloud[i]) != path_set.end())
			point_marks[i] = LAND; //Mark path points as land
	}

	//Create a queue of point indices to process (excluding path points)
	std::vector<size_t> queue;
	queue.reserve(_point_cloud.size());

	//Fill queue with indices of all points except path points
	for (size_t i = 0; i < _point_cloud.size(); ++i) {
		if (path_set.find(_point_cloud[i]) != path_set.end()) //Skip points that are already in the path
			continue;
		queue.push_back(i);
	}

	while (!queue.empty()) {
		size_t random_index = scl::rand->randomInt(queue.size() - 1);
		size_t point_idx = queue[random_index];

		//Remove the point from the queue by swapping with the last element and popping
		std::swap(queue[random_index], queue.back());
		queue.pop_back();
		auto vertex_handle = _dl.nearest_vertex(_point_cloud[point_idx]);
		//Check all neighbors
		auto neighbor_vertices = _dl.incident_vertices(vertex_handle);
		auto first = neighbor_vertices;
		do {
			if (_dl.is_infinite(neighbor_vertices))
				continue;

			size_t neighbor_idx = _points_index_map[neighbor_vertices];
			//If any neighbor is in the path or already land, mark current point as land
			if (path_set.find(neighbor_vertices->point()) != path_set.end() || point_marks[neighbor_idx] == LAND) {
				point_marks[point_idx] = LAND; break;
			} else //Mark as sea 
				point_marks[point_idx] = SEA;
			for (size_t i = 0; i < _point_cloud.size(); ++i) {
				//Skip if not land or if it's the current point
				if (point_marks[i] != 1 || i == point_idx)
					continue;

				//If within threshold distance, mark as land with a fixed probability
				auto sq_dist = CGAL::squared_distance(_point_cloud[point_idx], _point_cloud[i]);
				bool land_pb = scl::rand->randomFloat(0.0f, 1.0f) < _data_config._land_probability;
				if (sq_dist < (CGAL::square(_data_config._points_min_dist) * CGAL::square(_data_config._land_threshold)) && land_pb) {
					point_marks[point_idx] = LAND; break;
				}
			}

		} while (++neighbor_vertices != first);
	}
	return point_marks;
}

/*
	This function creates elevation based on the terrain and proximity to coast points
	- Coast points are identified as land points with sea neighbors
	- BFS is used to compute hop distances from coast points
	- Elevation is set based on hop distances, with land points getting positive values and sea points negative values
*/
void rnn::WorldGen::create_elevation()
{
	//Find all coast points
	std::vector<bool> is_coast(_point_cloud.size(), false);
	
	for (size_t i = 0; i < _terrain.size(); i++) {
		if (_terrain[i] >= 0) { //Only check land points
			auto vertex_handle = _dl.nearest_vertex(_point_cloud[i]);
			auto neighbor = _dl.incident_vertices(vertex_handle);
			auto first = neighbor;
			do {
				if (_dl.is_infinite(neighbor))
					continue;

				size_t neighbor_idx = _points_index_map[neighbor];
				if (_terrain[neighbor_idx] < 0) { //If neighbor is sea
					is_coast[i] = true;
					break;
				}
			} while (++neighbor != first);
		}
	}

	std::vector<int> hop_distances(_point_cloud.size(), std::numeric_limits<int>::max());	
	std::queue<size_t> bfs_queue;
	
	//Initialize queue with coast points and their neighbors
	for (size_t i = 0; i < _point_cloud.size(); i++) {
		if (is_coast[i]) {
			hop_distances[i] = 0;
			bfs_queue.push(i);
			
			//Add sea neighbors of coast points
			auto vertex_handle = _dl.nearest_vertex(_point_cloud[i]);
			auto neighbors = _dl.incident_vertices(vertex_handle);
			auto first = neighbors;
			do {
				if (_dl.is_infinite(neighbors))
					continue;

				size_t neighbor_idx = _points_index_map[neighbors];
				if (_terrain[neighbor_idx] < 0) { //If neighbor is sea
					hop_distances[neighbor_idx] = 1;
					bfs_queue.push(neighbor_idx);
				}
			} while (++neighbors != first);
		}
	}
	
	//BFS to compute hop distances for all points
	while (!bfs_queue.empty()) {
		size_t current_idx = bfs_queue.front();
		bfs_queue.pop();
		
		auto vertex_handle = _dl.nearest_vertex(_point_cloud[current_idx]);
		auto neighbors = _dl.incident_vertices(vertex_handle);
		auto first = neighbors;
		
		do {
			if (_dl.is_infinite(neighbors))
				continue;
				
			size_t neighbor_idx = _points_index_map[neighbors];
			
			//Only process points of the same type (land or sea)
			if ((_terrain[current_idx] >= 0 && _terrain[neighbor_idx] >= 0) || 
				(_terrain[current_idx] < 0 && _terrain[neighbor_idx] < 0)) {
				
				//If we found a shorter path to this neighbor
				if (hop_distances[neighbor_idx] > hop_distances[current_idx] + 1) {
					hop_distances[neighbor_idx] = hop_distances[current_idx] + 1;
					bfs_queue.push(neighbor_idx);
				}
			}
		} while (++neighbors != first);
	}
	
	//Set elevations based on hop distances
	for (size_t i = 0; i < _point_cloud.size(); i++) {
		if (_terrain[i] >= 0) //Land points get elevation based on hop distance from coast
			_terrain[i] = hop_distances[i];
		else //Sea points get negative elevation based on hop distance from coast
			_terrain[i] = -hop_distances[i];
	}
}

void rnn::WorldGen::saveTo(std::string& path)
{
	std::cout << "Saving generation data.. ";
	std::ofstream file(path + "/gen.dat", std::ios::binary | std::ios::out);
	cereal::BinaryOutputArchive archive(file);
	archive(*this);
	file.close();
	std::cout << "done at " << path << std::endl;
}

void rnn::WorldGen::saveNew(const scl::World& wrld)
{
	std::string path = "../data/world";
	std::string sub_name = "world";
	int n_world = 1;
	//Check the number of world already saved and add 1 to this number
	for (auto& p : std::filesystem::directory_iterator(path)) {
		std::string f_name = std::filesystem::path(p).filename().string();
		std::size_t found = f_name.find(sub_name);
		if (found != std::string::npos)
			n_world++;
	}
	//Number of the new world saved
	sub_name += std::to_string(n_world);
	//Create the directory
	path += "/" + sub_name;
	std::filesystem::create_directory(path);
	saveTo(path);
	wrld.saveTo(path);
}