#ifndef RINN_WORLDGEN_H
#define RINN_WORLDGEN_H

#include <scroll.h>
#include <string>
#include <unordered_map>
#include <cereal/types/vector.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/access.hpp>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel					K;
typedef K::Point_2															Point_2;
typedef CGAL::Delaunay_triangulation_2<K>									Delaunay;

namespace rnn
{
	class WorldGen
	{
	public:
		WorldGen();
		~WorldGen();

		void generate(scl::World& wrld);
		Point_2 random_point(scl::DefaultPRNG& PRNG);
		Point_2 random_point_on_edge(scl::DefaultPRNG& PRNG);

		std::vector<Point_2> _point_cloud;
		std::unordered_map<Delaunay::Vertex_handle, size_t> _points_index_map;
		Delaunay _dl;
		std::vector<Point_2> _slope;
		std::vector<Point_2> _slope_path;
		std::vector<int> _terrain;

		void saveTo(std::string& path);
		void saveNew(const scl::World& wrld);
		friend class cereal::access;

		template<class Archive>
		void save(Archive & ar) const
		{
			std::vector<std::pair<double, double>> point_pair;
			for (auto p : _point_cloud)
				point_pair.push_back(std::pair<double, double>(p.x(), p.y()));
			ar(point_pair);

			std::vector<std::pair<double, double>> slope_pair;
			for (auto p : _slope)
				slope_pair.push_back(std::pair<double, double>(p.x(), p.y()));
			ar(slope_pair);

			std::vector<std::pair<double, double>> path_pair;
			for (auto p : _slope_path)
				path_pair.push_back(std::pair<double, double>(p.x(), p.y()));
			ar(path_pair);

			ar(_terrain);
		}

		template<class Archive>
		void load(Archive & ar)
		{
			std::vector<std::pair<double, double>> pos_pair;
			ar(pos_pair);
			for (auto p : pos_pair)
				_point_cloud.push_back(Point_2(p.first, p.second));

			_dl.insert(_point_cloud.begin(), _point_cloud.end());

			for (auto& vh : _dl.finite_vertex_handles()) {
				auto it = std::find(_point_cloud.begin(), _point_cloud.end(), vh->point());
				size_t idx = std::distance(_point_cloud.begin(), it);
				_points_index_map[vh] = idx;
			}

			std::vector<std::pair<double, double>> slope_pair;
			ar(slope_pair);
			for (auto p : slope_pair)
				_slope.push_back(Point_2(p.first, p.second));

			std::vector<std::pair<double, double>> path_pair;
			ar(path_pair);
			for (auto p : path_pair)
				_slope_path.push_back(Point_2(p.first, p.second));

			ar(_terrain);
		}

	protected:
		std::vector<int> mark_points_by_path_proximity(scl::DefaultPRNG& PRNG);
		void create_elevation();

		struct data_config
		{
			int _seed;
			float _points_min_dist;
			int _slope_min;
			int _slope_max;
			float _land_threshold;
			float _land_probability;

			struct _boundaries
			{
				float _x_min;
				float _x_max;
				float _y_min;
				float _y_max;
				template <class Archive> void serialize(Archive& ar)
				{
					ar(_x_min, _x_max, _y_min, _y_max);
				}
			} _boundaries;

			template <class Archive> void serialize(Archive& ar)
			{
				ar(_seed, _boundaries, _points_min_dist, _slope_min, _slope_max, _land_threshold, _land_probability);
			}
		} _data_config;
	};
}

#endif