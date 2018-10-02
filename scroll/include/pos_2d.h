#ifndef POS_2D_H
#define POS_2D_H

#include <memory>

namespace scl
{
	template<typename T>
	class Pos2D
	{
	public:
		Pos2D(T x, T y);
		Pos2D(const Pos2D &pos);
		~Pos2D();

		T distance(Pos2D pos);

		T _x;
		T _y;
	};

	template<typename T>
	using Pos2D_uptr = std::unique_ptr<scl::Pos2D<T>>;
	template<typename T>
	using Pos2D_sptr = std::shared_ptr<scl::Pos2D<T>>;
}

#endif