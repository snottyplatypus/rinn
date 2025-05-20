#ifndef WORLDGEN_H
#define WORLDGEN_H

#include "world.h"
#include <vector>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel					K;
typedef K::Point_2															Point_2;

namespace scl
{
	class WorldGen
	{
	public:
		virtual ~WorldGen() {}
		virtual scl::World generate() = 0;

		std::vector<Point_2> _point_cloud;

	protected:
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