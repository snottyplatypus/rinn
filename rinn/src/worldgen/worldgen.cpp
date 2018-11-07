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
	scl::DefaultPRNG PRNG;

	std::cout << "Generating points.. ";
	std::clock_t timer = clock();
	if(_point_cloud.size() == 0)
		_point_cloud = scl::PoissonGenerator::generatePoissonPoints(10000, PRNG, 10, true, 0.01f);
	std::cout << "done " << (clock() - timer) / (CLOCKS_PER_SEC / 1000) << "ms" << std::endl;
	std::cout << "Points generated : " << _point_cloud.size() << std::endl;

	std::cout << "Triangulating points.. ";
	timer = clock();
	Delaunay dl_all;
	dl_all.insert(_point_cloud.begin(), _point_cloud.end());
	std::cout << "done " << (clock() - timer) / (CLOCKS_PER_SEC / 1000) << "ms" << std::endl;
	std::cout << "Faces generated : " << dl_all.number_of_faces() << std::endl;

	std::cout << "Generating slope.. ";
	timer = clock();
	if (PRNG.randomFloat() > 0.5f)
		_slope.push_back(Point_2(PRNG.randomFloat() / 2, 0.0f));
	else
		_slope.push_back(Point_2(0.0f, PRNG.randomFloat() / 2));

	for (int i = 0; i < PRNG.randomInt(4); i++)
		_slope.push_back(Point_2(PRNG.randomFloat(), PRNG.randomFloat()));

	if (PRNG.randomFloat() > 0.5f)
		_slope.push_back(Point_2(PRNG.randomFloat() / 2 + 0.5f, 1.0f));
	else
		_slope.push_back(Point_2(1.0f, PRNG.randomFloat() / 2 + 0.5f));

	std::sort(_slope.begin(), _slope.end(), [&](const Point_2 & a, const Point_2 & b) -> bool
	{
		return CGAL::squared_distance(_slope[0], a) < CGAL::squared_distance(_slope[0], b);
	});

	for (auto i : _slope)
		std::cout << i << std::endl;

	auto line_face_circulator = dl_all.line_walk(_slope[0], _slope[1]);
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
	_slope_path = scl::dijkstra(dl_slope, slope_graph);
	std::reverse(_slope_path.begin(), _slope_path.end());
	std::cout << "done " << (clock() - timer) / (CLOCKS_PER_SEC / 1000) << "ms" << std::endl;

	return scl::World();
}

