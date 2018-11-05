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

		template<class Archive>
		void save(Archive & ar) const
		{
			std::vector<std::pair<double, double>> pos_pair;
			for (auto p : _point_cloud)
				pos_pair.push_back(std::pair<double, double>(p.x(), p.y()));
			ar(pos_pair);
		}

		template<class Archive>
		void load(Archive & ar)
		{
			std::vector<std::pair<double, double>> pos_pair;
			ar(pos_pair);
			for (auto p : pos_pair)
				_point_cloud.push_back(Point_2(p.first, p.second));
		}
	};
}

#endif