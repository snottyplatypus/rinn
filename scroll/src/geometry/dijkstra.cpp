#include "geometry/dijkstra.h"
#include <iostream>
#include <limits>

static int find_min_dist(std::vector<double> dist, std::vector<bool> ptSet)
{
	double min = std::numeric_limits<double>::max();
	int min_index = 0;
	for (int v = 0; v < dist.size(); v++)
	{
		if (ptSet[v] == false && dist[v] < min) {
			min = dist[v];
			min_index = v;
		}
	}
	return min_index;
}

std::vector<Point_2> scl::dijkstra(Delaunay t, std::vector<Point_2> graph, Point_2 appr_or, Point_2 appr_dr)
{
	if (t.number_of_vertices() == 0 || graph.empty())
		return {};

	Point_2 or = t.nearest_vertex(appr_or)->point();
	Point_2 dr = t.nearest_vertex(appr_dr)->point();
	auto or_iterator = std::find(graph.begin(), graph.end(), or);
	auto dr_iterator = std::find(graph.begin(), graph.end(), dr);

	if (or_iterator > dr_iterator)
		std::swap(or_iterator, dr_iterator);

	std::vector<Point_2> nodes(or_iterator, dr_iterator);
	std::vector<double> dist(nodes.size(), std::numeric_limits<double>::max());
	std::vector<int> parent(nodes.size());
	std::vector<bool> ptSet(nodes.size(), false);
	dist[0] = 0;

	for(int i = 0; i < nodes.size() - 1; i++) {
		int v1 = find_min_dist(dist, ptSet);
		ptSet[v1] = true;

		auto vertex_handle = t.faces_begin()->vertex(0)->handle();
		for (auto f = t.faces_begin(); f != t.faces_end(); f++) {
			for (int i = 0; i < 3; i++) {
				if (f->vertex(i)->point() == nodes[v1]) {
					vertex_handle = f->vertex(i)->handle();
					goto done;
				}
			}
		}
		done:

		auto neighbour = t.incident_vertices(vertex_handle);
		auto first = neighbour;
		do {
			int v2 = static_cast<int>(std::find(nodes.begin(), nodes.end(), neighbour->point()) - nodes.begin());
			if (v2 < nodes.size()) {
				double sq_dist = CGAL::squared_distance(nodes[v1], nodes[v2]);
				if (!ptSet[v2] && dist[v1] + sq_dist) {
					if (dist[v2] > dist[v1] + sq_dist) {
						dist[v2] = dist[v1] + sq_dist;
						parent[v2] = v1;
					}
				}
			}
		} while (++neighbour != first);
	}

	std::vector<Point_2> path;
	int v = static_cast<int>(nodes.size() - 1);
	path.push_back(nodes[v]);
	while (v > 0) {
		path.push_back(nodes[parent[v]]);
		v = parent[v];
	}
	
	return path;
}