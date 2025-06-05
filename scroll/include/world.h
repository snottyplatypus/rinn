#ifndef WORLD_H
#define WORLD_H

#include "geometry/space_2d.h"
#include <memory>
#include <vector>
#include <string>
#include <cereal/types/string.hpp>
#include <cereal/types/memory.hpp>

namespace scl
{
	class World
	{
	public:
		World();
		~World();
		void saveTo(const std::string& path) const;

		std::string _path;
		std::shared_ptr<Space2D<int>> _chunks;

        template<class Archive>
        void serialize(Archive& ar) const
        {
            ar(_path);
            ar(_chunks);
        }
	};
}

#endif