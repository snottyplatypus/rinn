#include "worldgen/worldgen.h"
#include <ctime>
#include <utility>
#include <algorithm>
#include <set>
#include <iostream>
#include <fstream>

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
*/
scl::World rnn::WorldGen::generate()
{
	//Initializing PRNG and load config file
	scl::DefaultPRNG PRNG;
	std::ifstream file("../data/config/worldgen.json");
	cereal::JSONInputArchive archive(file);
	archive(cereal::make_nvp("worldgen", _data_config));
	if (_data_config._seed >= 0) {
		PRNG.seed(static_cast<uint32_t>(_data_config._seed));
		std::cout << "Using seed : " << _data_config._seed << std::endl;
	}

	std::cout << "Generating points.. ";
	std::clock_t timer = clock();
	//Generate points using Poisson Generation
	if(_point_cloud.size() == 0)
		_point_cloud = scl::PoissonGenerator::generatePoissonPoints(10000, PRNG, 10, false, _data_config._points_min_dist);
	std::cout << "done " << (clock() - timer) / (CLOCKS_PER_SEC / 1000) << "ms" << std::endl;
	std::cout << "Points generated : " << _point_cloud.size() << std::endl;

	std::cout << "Triangulating points.. ";
	timer = clock();
	Delaunay dl_all;
	//Triangulate all points using Delaunay triangulation
	dl_all.insert(_point_cloud.begin(), _point_cloud.end());
	std::cout << "done " << (clock() - timer) / (CLOCKS_PER_SEC / 1000) << "ms" << std::endl;
	std::cout << "Faces generated : " << dl_all.number_of_faces() << std::endl;

	//Generate slope from one edge to another
	std::cout << "Generating slope.. ";
	timer = clock();
	
	std::vector<Point_2> unsorted_slope;
	unsorted_slope.push_back(random_point_on_edge(PRNG));
	for (int i = 0; i < _data_config._slope_min + PRNG.randomInt(_data_config._slope_max - _data_config._slope_min); i++)
		unsorted_slope.push_back(random_point(PRNG));
	unsorted_slope.push_back(random_point_on_edge(PRNG));
	
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
		auto line_face_circulator = dl_all.line_walk(_slope[i], _slope[i + 1]);
		auto first_face = line_face_circulator;
		auto last_face = line_face_circulator;
		try {
			do {
				if (line_face_circulator != nullptr) {
					if (!dl_all.is_infinite(line_face_circulator)) {
						auto f = line_face_circulator.handle();
						slope_graph_set.insert(dl_all.triangle(f)[0]);
						slope_graph_set.insert(dl_all.triangle(f)[1]);
						slope_graph_set.insert(dl_all.triangle(f)[2]);
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
	_terrain = mark_points_by_path_proximity(dl_all, _point_cloud, _slope_path, PRNG);
	std::cout << "done " << (clock() - timer) / (CLOCKS_PER_SEC / 1000) << "ms" << std::endl;

	return scl::World();
}

/*
	This function generate a random point in the generated world inside boundaries
*/
Point_2 rnn::WorldGen::random_point(scl::DefaultPRNG& PRNG)
{
	float r_x_min = 0.0f + _data_config._boundaries._x_min;
	float r_x_max = 1.0f - _data_config._boundaries._x_max;
	float r_y_min = 0.0f + _data_config._boundaries._y_min;
	float r_y_max = 1.0f - _data_config._boundaries._y_max;
	return Point_2(PRNG.randomFloat(r_x_min, r_x_max), PRNG.randomFloat(r_y_min, r_y_max));
}

/*
	This function generate a random point on the edge of the generated world inside boundaries
*/
Point_2 rnn::WorldGen::random_point_on_edge(scl::DefaultPRNG& PRNG)
{
	float r_x = PRNG.randomFloat(0.0f + _data_config._boundaries._x_min, 1.0f - _data_config._boundaries._x_max);
	float r_y = PRNG.randomFloat(0.0f + _data_config._boundaries._y_min, 1.0f - _data_config._boundaries._y_max);
	float p = PRNG.randomFloat();
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
std::vector<int> rnn::WorldGen::mark_points_by_path_proximity(const Delaunay& dl, const std::vector<Point_2>& points, const std::vector<Point_2>& path, scl::DefaultPRNG& PRNG)
{
	std::vector<int> point_marks(points.size(), -1); // Default mark is -1 (void)

	//Create a set of path points for faster lookup
	std::set<Point_2> path_set(path.begin(), path.end());

	//Mark all path points as 2
	for (size_t i = 0; i < points.size(); ++i) {
		if (path_set.find(points[i]) != path_set.end())
			point_marks[i] = 2; //Mark path points as land
	}

	//Create a queue of point indices to process (excluding path points)
	std::vector<size_t> queue;
	queue.reserve(points.size());

	//Fill queue with indices of all points except path points
	for (size_t i = 0; i < points.size(); ++i) {
		if (path_set.find(points[i]) != path_set.end()) // Skip points that are already in the path
			continue;
		queue.push_back(i);
	}

	while (!queue.empty()) {
		size_t random_index = PRNG.randomInt(queue.size() - 1);
		size_t point_idx = queue[random_index];

		//Remove the point from the queue by swapping with the last element and popping
		std::swap(queue[random_index], queue.back());
		queue.pop_back();
		auto vertex_handle = dl.nearest_vertex(points[point_idx]);
		// Check all neighbors
		auto neighbor = dl.incident_vertices(vertex_handle);
		auto first = neighbor;
		do {
			if (dl.is_infinite(neighbor))
				continue;

			auto it = std::find(points.begin(), points.end(), neighbor->point());
			size_t neighbor_idx = std::distance(points.begin(), it);
			//If any neighbor is in the path or already terrain, mark current point as land
			if (path_set.find(neighbor->point()) != path_set.end() || (it != points.end() && point_marks[neighbor_idx] == 2)) {
				point_marks[point_idx] = 2; break;
			} else //Mark as sea 
				point_marks[point_idx] = 0;

			for (size_t i = 0; i < points.size(); ++i) {
				//Skip if not terrain or if it's the current point
				if (point_marks[i] != 2 || i == point_idx)
					continue;

				//If within threshold distance, mark as land
				auto sq_dist = CGAL::squared_distance(points[point_idx], points[i]);
				bool land_pb = PRNG.randomFloat(0.0f, 1.0f) < _data_config._land_probability;
				if (sq_dist < (_data_config._points_min_dist * _data_config._points_min_dist * _data_config._land_threshold) && land_pb) {
					point_marks[point_idx] = 2; break;
				}
			}

		} while (++neighbor != first);
	}

	return point_marks;
}
