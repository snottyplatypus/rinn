#ifndef WORLD_H
#define WORLD_H

#include "geometry/space_2d.h"

#include <memory>
namespace scl
{
	class World
	{
	public:
		World();
		~World();

		std::shared_ptr<Space2D<int>> _chunks;
	};
}

#endif