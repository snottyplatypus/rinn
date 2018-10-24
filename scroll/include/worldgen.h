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

	protected:
		std::vector<Point_2> _point_cloud;
	};
}

#endif