#include "geometry/pos_2d.h"
#include <cmath>

template<typename T>
scl::Pos2D<T>::Pos2D(T x, T y) : _x(x), _y(y)
{
}

template<typename T>
scl::Pos2D<T>::Pos2D(const Pos2D &pos) : _x(pos._x), _y(pos._y)
{
}

template<typename T>
scl::Pos2D<T>::~Pos2D()
{
}

template<typename T>
T scl::Pos2D<T>::distance(Pos2D pos) {
	return static_cast<T>( std::sqrt((_x - pos._x) * (_x - pos._x) + (_y - pos._y) * (_y - pos._y)) );
}