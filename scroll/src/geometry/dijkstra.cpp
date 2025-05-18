#include "geometry/dijkstra.h"
#include <iostream>
#include <limits>

std::vector<Point_2> scl::dijkstra(const Delaunay t, const std::vector<Point_2> graph, Point_2 appr_or, Point_2 appr_dr)
{
	if (t.number_of_vertices() == 0 || graph.empty())
		return {};

	//Snap input points to nearest vertices in the triangulation
	Point_2 or = t.nearest_vertex(appr_or)->point();
	Point_2 dr = t.nearest_vertex(appr_dr)->point();
	auto or_iterator = std::find(graph.begin(), graph.end(), or);
	auto dr_iterator = std::find(graph.begin(), graph.end(), dr);
	
	//Ensure correct iteration range
	if (or_iterator > dr_iterator)
		std::swap(or_iterator, dr_iterator);
	//If origin and destination are the same, return single-point path
	if (or_iterator == dr_iterator)
		return { *or_iterator };

	//Extract the subgraphs between origin and destination
	std::vector<Point_2> nodes(or_iterator, dr_iterator);
	std::vector<double> dist(nodes.size(), std::numeric_limits<double>::max());
	std::vector<int> parent(nodes.size(), -1);
	std::vector<bool> visited(nodes.size(), false);

	//Distance to source is 0
	dist[0] = 0;

	for(int i = 0; i < nodes.size() - 1; i++) {
		//Find the unvisited node with the smallest distance
		int current_idx = -1;
		double min = std::numeric_limits<double>::max();
		for (int v = 0; v < dist.size(); v++)
		{
			if (!visited[v] && dist[v] < min) {
				min = dist[v];
				current_idx = v;
			}
		}
		if (current_idx == -1) //All remaining nodes are inaccessible
			break; 
		visited[current_idx] = true;

		//Explore neighbors in the Delaunay triangulation
		auto vertex_handle = t.nearest_vertex(nodes[current_idx]);
		auto neighbour = t.incident_vertices(vertex_handle);
		auto first = neighbour;

		do {
			//Check if the neighbor is in the subgraph
			auto it = std::find(nodes.begin(), nodes.end(), neighbour->point());
			if (it != nodes.end()) {
				int neighbour_idx = static_cast<int>(it - nodes.begin());
				double sq_dist = CGAL::squared_distance(nodes[current_idx], nodes[neighbour_idx]);
				//Relax the edge
				if (!visited[neighbour_idx] && dist[neighbour_idx] > dist[current_idx] + sq_dist) {
					dist[neighbour_idx] = dist[current_idx] + sq_dist;
					parent[neighbour_idx] = current_idx;
				}
			}
		} while (++neighbour != first); //Loop over all incident vertices
	}

	//Reconstruct the path from destination back to source using parent indices
	std::vector<Point_2> path;
	int v = static_cast<int>(nodes.size() - 1);
	if (parent[v] == -1 && v != 0) //If destination has no parent and isn't in the start, no path was found
		return {};

	//Follow the parent chain backward
	while (v != -1) {
		path.push_back(nodes[v]);
		v = parent[v];
	}

	//Reverse to get path from source to destination
	std::reverse(path.begin(), path.end());
	return path;
}