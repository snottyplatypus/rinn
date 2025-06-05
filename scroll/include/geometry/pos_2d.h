#ifndef POS_2D_H
#define POS_2D_H

#include <memory>
#include <cmath>

namespace scl
{
	template<typename T>
	class Pos2D
	{
	public:
		Pos2D(T x, T y) : _x(x), _y(y) {}
		Pos2D(const Pos2D &pos) : _x(pos._x), _y(pos._y) {}
		~Pos2D() {}

		T distance(Pos2D pos)
		{
			return static_cast<T>(std::sqrt((_x - pos._x) * (_x - pos._x) + (_y - pos._y) * (_y - pos._y)));
		}

		T _x;
		T _y;

		template<class Archive>
		void serialize(Archive& ar) const
		{
			ar(_x, _y);
		}
	};

	template<typename T>
	using Pos2D_uptr = std::unique_ptr<scl::Pos2D<T>>;
	template<typename T>
	using Pos2D_sptr = std::shared_ptr<scl::Pos2D<T>>;
}

#endif