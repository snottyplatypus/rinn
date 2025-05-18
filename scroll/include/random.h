#ifndef RANDOM_H
#define RANDOM_H

#include <random> 

namespace scl
{
	class DefaultPRNG
	{
	public:
		DefaultPRNG() : _gen(std::random_device()()), _dis(0.0f, 1.0f)
		{
			_gen.seed(static_cast<unsigned int>(time(nullptr)));
		}

		explicit DefaultPRNG(uint32_t seed) : _gen(seed), _dis(0.0f, 1.0f)
		{
		}

		void seed(uint32_t seed)
		{
			_gen.seed(seed);
		}

		float randomFloat()
		{
			return static_cast<float>(_dis(_gen));
		}

		float randomFloat(float min, float max)
		{
			std::uniform_real_distribution<float> dis(min, max);
			return dis(_gen);
		}

		int randomInt(int max)
		{
			std::uniform_int_distribution<> disInt(0, max);
			return disInt(_gen);
		}

	private:
		std::mt19937 _gen;
		std::uniform_real_distribution<float> _dis;
	};
}

#endif