#include "geometry/dijkstra.h"
#include <iostream>
#include <limits>
#include <set>
#include <map>

std::vector<Point_2> scl::dijkstra(const Delaunay t, const std::vector<Point_2> graph, Point_2 appr_or, Point_2 appr_dr)
{
	if (t.number_of_vertices() == 0 || graph.empty())
		return {};

	//Snap input points to nearest vertices in the triangulation
	Point_2 or = t.nearest_vertex(appr_or)->point();
	Point_2 dr = t.nearest_vertex(appr_dr)->point();
	//Check if origin and destination exist in graph
	auto or_it = std::find(graph.begin(), graph.end(), or );
	auto dr_it = std::find(graph.begin(), graph.end(), dr);

	if (or_it == graph.end() || dr_it == graph.end())
		return {}; //Either origin or destination not in graph

	if (or == dr)//If origin and destination are the same, return single-point path
		return { or };

	// Using all graph vertices rather than a subset
	std::map<Point_2, double> dist;
	std::map<Point_2, Point_2> parent;
	std::map<Point_2, bool> visited;

	//Custom comparator for the priority queue to order by distance
	auto cmp = [](const std::pair<double, Point_2>& a, const std::pair<double, Point_2>& b) {
		return a.first < b.first || (a.first == b.first && a.second < b.second);
		};

	//Priority queue to efficiently select node with minimum distance
	std::set<std::pair<double, Point_2>, decltype(cmp)> queue(cmp);

	//Initialize distances
	for (const auto& point : graph) {
		dist[point] = std::numeric_limits<double>::max();
		parent[point] = Point_2(); //Default initialization
		visited[point] = false;
	}

	//Distance to origin is 0
	dist[or ] = 0;
	queue.insert({ 0, or });

	while (!queue.empty()) {
		//Get node with minimum distance
		auto current = *queue.begin();
		Point_2 current_point = current.second;
		queue.erase(queue.begin());

		//Skip if already visited
		if (visited[current_point])
			continue;

		visited[current_point] = true;

		//If we've reached the destination, we can stop
		if (current_point == dr)
			break;

		//Explore neighbors in the Delaunay triangulation
		auto vertex_handle = t.nearest_vertex(current_point);
		auto neighbour = t.incident_vertices(vertex_handle);
		auto first = neighbour;

		if (neighbour == nullptr) //Skip if no neighbors (shouldn't happen in valid triangulation)
			continue;

		do {
			//Make sure the neighbor isn't at infinity and exists in our graph
			if (t.is_infinite(neighbour))
				continue;

			Point_2 neighbor_point = neighbour->point();
			auto it = std::find(graph.begin(), graph.end(), neighbor_point);

			if (it != graph.end() && !visited[neighbor_point]) {
				double sq_dist = CGAL::squared_distance(current_point, neighbor_point);
				double new_dist = dist[current_point] + sq_dist;

				//Relax the edge
				if (new_dist < dist[neighbor_point]) {
					//If already in queue, remove old distance
					if (dist[neighbor_point] != std::numeric_limits<double>::max())
						queue.erase({ dist[neighbor_point], neighbor_point });

					//Update distance and parent
					dist[neighbor_point] = new_dist;
					parent[neighbor_point] = current_point;

					//Add with new distance to queue
					queue.insert({ new_dist, neighbor_point });
				}
			}
		} while (++neighbour != first); //Loop over all incident vertices
	}

	//Reconstruct the path from destination back to source
	std::vector<Point_2> path;
	Point_2 current = dr;

	//Check if destination was reachable
	if (dist[dr] == std::numeric_limits<double>::max())
		return {};

	//Follow the parent chain backward
	while (current != or) {
		path.push_back(current);
		current = parent[current];
	}
	path.push_back(or); //Add the origin

	//Reverse to get path from source to destination
	std::reverse(path.begin(), path.end());
	return path;
}