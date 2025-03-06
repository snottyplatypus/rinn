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

	std::cout << "Generating points.. ";
	std::clock_t timer = clock();
	//Generate points using Poisson Generation
	if(_point_cloud.size() == 0)
		_point_cloud = scl::PoissonGenerator::generatePoissonPoints(10000, PRNG, 10, false, 0.01f);
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
	
	_slope.push_back(random_point_on_edge(PRNG));
	for (int i = 0; i < _data_config._slope_min + PRNG.randomInt(_data_config._slope_max - _data_config._slope_min); i++)
		_slope.push_back(Point_2(PRNG.randomFloat(), PRNG.randomFloat()));
	_slope.push_back(random_point_on_edge(PRNG));

	//Sort slope points by distance to the first point
	std::sort(_slope.begin(), _slope.end(), [&](const Point_2 & a, const Point_2 & b) -> bool
	{
		return CGAL::squared_distance(_slope[0], a) < CGAL::squared_distance(_slope[0], b);
	});

	//Generate slope path
	for (size_t i = 0; i < _slope.size() - 1; i++) 
	{
		std::set<Point_2> slope_graph_set;
		auto line_face_circulator = dl_all.line_walk(_slope[i], _slope[i + 1]);
		auto first_face = line_face_circulator;
		auto last_face = line_face_circulator;
		do {
			if (!dl_all.is_infinite(line_face_circulator)) {
				auto f = line_face_circulator.handle();
				slope_graph_set.insert(dl_all.triangle(f)[0]);
				slope_graph_set.insert(dl_all.triangle(f)[1]);
				slope_graph_set.insert(dl_all.triangle(f)[2]);
				last_face = line_face_circulator;
			}
		} while (++line_face_circulator != first_face);
		std::vector<Point_2> slope_graph(slope_graph_set.begin(), slope_graph_set.end());
		Delaunay dl_slope(slope_graph_set.begin(), slope_graph_set.end());
		std::vector<Point_2> temp_slope_path = scl::dijkstra(dl_slope, slope_graph, _slope[i], _slope[i + 1]);
		std::reverse(temp_slope_path.begin(), temp_slope_path.end());
		_slope_path.insert(_slope_path.end(), temp_slope_path.begin(), temp_slope_path.end());
	}
	std::cout << "done " << (clock() - timer) / (CLOCKS_PER_SEC / 1000) << "ms" << std::endl;

	return scl::World();
}

/*
	This function generate a random point on the edge of the generated world
*/
Point_2 rnn::WorldGen::random_point_on_edge(scl::DefaultPRNG PRNG)
{
	if (PRNG.randomFloat() < 0.5f)
		if (PRNG.randomFloat() < 0.25f)
			return Point_2(PRNG.randomFloat(), 0.0f);
		else
			return Point_2(PRNG.randomFloat(), 1.0f);
	else
		if (PRNG.randomFloat() < 0.75f)
			return Point_2(0.0f, PRNG.randomFloat());
		else
			return Point_2(1.0f, PRNG.randomFloat());
}

