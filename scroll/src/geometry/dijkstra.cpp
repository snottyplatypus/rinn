#include "geometry/dijkstra.h"
#include <iostream>
#include <limits>

static int find_min_dist(std::vector<double> dist, std::vector<bool> ptSet)
{
	double min = std::numeric_limits<double>::max();
	int min_index;
	for (int v = 0; v < dist.size(); v++)
	{
		if (ptSet[v] == false && dist[v] < min) {
			min = dist[v];
			min_index = v;
		}
	}
	return min_index;
}

std::vector<Point_2> scl::dijkstra(Delaunay t, std::vector<Point_2> graph)
{
	std::vector<double> dist(graph.size(), std::numeric_limits<double>::max());
	std::vector<int> parent(graph.size());
	std::vector<bool> ptSet(graph.size(), false);
	dist[0] = 0;

	std::vector<Point_2> nodes(graph.begin(), graph.end());
	for(int i = 0; i < nodes.size() - 1; i++) {
		int v1 = find_min_dist(dist, ptSet);
		ptSet[v1] = true;

		auto vertex_handle = t.faces_begin()->vertex(0)->handle();
		for (auto f = t.faces_begin(); f != t.faces_end(); f++) {
			for (int i = 0; i < 3; i++) {
				if (f->vertex(i)->point() == graph[v1]) {
					vertex_handle = f->vertex(i)->handle();
					goto done;
				}
			}
		}
		done:

		auto neighbour = t.incident_vertices(vertex_handle);
		auto first = neighbour;
		do {
			int v2 = static_cast<int>(std::find(graph.begin(), graph.end(), neighbour->point()) - graph.begin());
			if (v2 < graph.size()) {
				double sq_dist = CGAL::squared_distance(graph[v1], graph[v2]);
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
	int v = static_cast<int>(graph.size() - 1);
	path.push_back(graph[v]);
	while (v > 0) {
		path.push_back(graph[parent[v]]);
		v = parent[v];
	}
	
	return path;
}