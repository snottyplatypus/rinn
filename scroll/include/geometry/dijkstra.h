#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <vector>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_2.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel					K;
typedef K::Point_2															Point_2;
typedef CGAL::Delaunay_triangulation_2<K>									Delaunay;

namespace scl 
{
	std::vector<Point_2> dijkstra(const Delaunay t, const std::vector<Point_2> graph, Point_2 appr_or, Point_2 appr_dr);
}

#endif