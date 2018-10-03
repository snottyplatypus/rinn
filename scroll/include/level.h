#ifndef LEVEL_H
#define LEVEL_H

#include "geometry/space_3d.h"
#include "terrain.h"
#include <memory>

namespace scl
{
	class Level
	{
	public:
		Level();
		~Level();

		std::shared_ptr<Space3D<Terrain>> _terrain;
	};
}

#endif