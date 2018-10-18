#include "rinn.h"
#include "scroll.h"
#include <iostream>
#include <vector>
#include <ctime>
#include <algorithm>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_2.h>
#include <CGAL/Triangulation_line_face_circulator_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel					K;
typedef K::Point_2															Point_2;
typedef CGAL::Delaunay_triangulation_2<K>									Delaunay;
typedef CGAL::Triangulation_2<K>											Triangulation;
typedef CGAL::Triangulation_line_face_circulator_2<Triangulation>			Line_face_circulator;
typedef CGAL::Container_from_circulator<Line_face_circulator>				Line_face_circulator_container;
typedef Line_face_circulator_container::iterator							Line_face_circulator_iterator;

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

	for (auto el : points)
	{
		int x = static_cast<int>(el.x() * imageSize);
		int y = static_cast<int>(el.y() * imageSize);
		image.put(x, y, 0xFFFFFF);
	}
	std::cout << "Saving.. ";
	image.save("points.bmp");

	timer = clock();
	std::cout << "Triangulating points.. ";
	Delaunay t;
	t.insert(points.begin(), points.end());
	std::cout << (clock() - timer) / (CLOCKS_PER_SEC / 1000) << "ms" << std::endl;
	std::cout << "Faces generated : " << t.number_of_faces() << std::endl;

	image.clear();
	for (auto f = t.faces_begin(); f != t.faces_end(); f++)
	{
		int x0 = static_cast<int>(t.triangle(f)[0].x() * imageSize);
		int y0 = static_cast<int>(t.triangle(f)[0].y() * imageSize);
		int x1 = static_cast<int>(t.triangle(f)[1].x() * imageSize);
		int y1 = static_cast<int>(t.triangle(f)[1].y() * imageSize);
		int x2 = static_cast<int>(t.triangle(f)[2].x() * imageSize);
		int y2 = static_cast<int>(t.triangle(f)[2].y() * imageSize);
		image.line(x0, y0, x1, y1, color_delaunay);
		image.line(x1, y1, x2, y2, color_delaunay);
		image.line(x2, y2, x0, y0, color_delaunay);
		image.put(x0, y0, color_points);
		image.put(x1, y1, color_points);
		image.put(x2, y2, color_points);
	}
	std::cout << "Saving.. ";
	image.save("delaunay.bmp");

	auto line_face_circulator = t.line_walk(Point_2(0, 0), Point_2(1, 1));
	auto first = line_face_circulator;
	auto prev = line_face_circulator;
	do {
		if (!t.is_infinite(line_face_circulator)) {
			auto f = line_face_circulator.handle();
			int x0_f = static_cast<int>(t.triangle(f)[0].x() * imageSize);
			int y0_f = static_cast<int>(t.triangle(f)[0].y() * imageSize);
			int x1_f = static_cast<int>(t.triangle(f)[1].x() * imageSize);
			int y1_f = static_cast<int>(t.triangle(f)[1].y() * imageSize);
			int x2_f = static_cast<int>(t.triangle(f)[2].x() * imageSize);
			int y2_f = static_cast<int>(t.triangle(f)[2].y() * imageSize);
			image.line(x0_f, y0_f, x1_f, y1_f, 0x0000FF);
			image.line(x1_f, y1_f, x2_f, y2_f, 0x0000FF);
			image.line(x2_f, y2_f, x0_f, y0_f, 0x0000FF);
		}
	} while (++line_face_circulator != first);
	
	image.line(0, 0, imageSize, imageSize, 0xFF0000);
	image.save("random_slope.bmp");

	TCODConsole::root->flush();
	TCODConsole::root->waitForKeypress(true);

	return 0;
}