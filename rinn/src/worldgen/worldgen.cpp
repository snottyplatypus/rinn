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

scl::World rnn::WorldGen::generate()
{
	scl::PoissonGenerator::DefaultPRNG PRNG;

	std::cout << "Generating points.. ";
	std::clock_t timer = clock();
	if(_point_cloud.size() == 0)
		_point_cloud = scl::PoissonGenerator::generatePoissonPoints(10000, PRNG, 10, true, 0.01f);
	std::cout << (clock() - timer) / (CLOCKS_PER_SEC / 1000) << "ms" << std::endl;
	std::cout << "Points generated : " << _point_cloud.size() << std::endl;

	std::cout << "Triangulating points.. ";
	timer = clock();
	Delaunay dl_all;
	dl_all.insert(_point_cloud.begin(), _point_cloud.end());
	std::cout << (clock() - timer) / (CLOCKS_PER_SEC / 1000) << "ms" << std::endl;
	std::cout << "Faces generated : " << dl_all.number_of_faces() << std::endl;

	std::cout << "Generating slope.. ";
	timer = clock();
	Point_2 slope_or(0.0f, 0.0f);
	Point_2 slope_dr(1.0f, 1.0f);
	auto line_face_circulator = dl_all.line_walk(slope_or, slope_dr);
	auto first_face = line_face_circulator;
	auto last_face = line_face_circulator;
	std::set<Point_2> slope_graph_set;

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
	std::vector<Point_2> slope_path = scl::dijkstra(dl_slope, slope_graph);
	std::reverse(slope_path.begin(), slope_path.end());
	std::cout << (clock() - timer) / (CLOCKS_PER_SEC / 1000) << "ms" << std::endl;

	return scl::World();
}

