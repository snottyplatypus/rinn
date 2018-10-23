#include "rinn.h"
#include "scroll.h"
#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>
#include <set>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel					K;
typedef K::Point_2															Point_2;
typedef CGAL::Delaunay_triangulation_2<K>									Delaunay;

const int color_points		= 0xFFFFFF;
const int color_delaunay	= 0x303030;

int main()
{
	scl::System::init();
	scl::World world;
	scl::PoissonGenerator::DefaultPRNG PRNG;

	std::clock_t timer = clock();
	std::cout << "Generating points.. ";
	std::vector<Point_2> points = scl::PoissonGenerator::generatePoissonPoints(10000, PRNG, 10, true, 0.01f);
	std::cout << (clock() - timer) / (CLOCKS_PER_SEC / 1000) << "ms" << std::endl;
	std::cout << "Points generated : " << points.size() << std::endl;

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

	timer = clock();
	std::cout << "Triangulating points.. ";
	Delaunay t_all;
	t_all.insert(points.begin(), points.end());
	std::cout << (clock() - timer) / (CLOCKS_PER_SEC / 1000) << "ms" << std::endl;
	std::cout << "Faces generated : " << t_all.number_of_faces() << std::endl;

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

	timer = clock();
	std::cout << "Generating slope.. ";
	Point_2 slope_or(0.0f, 0.0f);
	Point_2 slope_dr(1.0f, 1.0f);
	auto line_face_circulator = t_all.line_walk(slope_or, slope_dr);
	auto first_face = line_face_circulator;
	auto last_face = line_face_circulator;
	std::set<Point_2> slope_graph_set;

	do {
		if (!t_all.is_infinite(line_face_circulator)) {
			auto f = line_face_circulator.handle();
			slope_graph_set.insert(t_all.triangle(f)[0]);
			slope_graph_set.insert(t_all.triangle(f)[1]);
			slope_graph_set.insert(t_all.triangle(f)[2]);
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

	std::vector<Point_2> slope_graph(slope_graph_set.begin(), slope_graph_set.end());
	Delaunay t_slope(slope_graph_set.begin(), slope_graph_set.end());
	std::vector<Point_2> slope_path = scl::dijkstra(t_slope, slope_graph);
	std::reverse(slope_path.begin(), slope_path.end());
	std::cout << (clock() - timer) / (CLOCKS_PER_SEC / 1000) << "ms" << std::endl;

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

	TCODConsole::root->flush();
	TCODConsole::root->waitForKeypress(true);

	return 0;
}