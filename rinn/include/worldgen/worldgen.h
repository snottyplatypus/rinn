#ifndef RINN_WORLDGEN_H
#define RINN_WORLDGEN_H

#include <scroll.h>
#include <cereal/types/vector.hpp>
#include <cereal/types/utility.hpp>

namespace rnn
{
	class WorldGen : public scl::WorldGen
	{
	public:
		WorldGen();
		~WorldGen();

		scl::World generate() override;
		Point_2 random_point(scl::DefaultPRNG& PRNG);
		Point_2 random_point_on_edge(scl::DefaultPRNG& PRNG);
		std::vector<int> mark_points_by_path_proximity(const Delaunay& dl, const std::vector<Point_2>& points, const std::vector<Point_2>& path, scl::DefaultPRNG& PRNG);

		std::vector<Point_2> _slope;
		std::vector<Point_2> _slope_path;
		std::vector<int> _terrain;
		
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
	};
}

#endif