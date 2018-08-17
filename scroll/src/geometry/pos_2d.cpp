#include "pos_2d.h"
#include <cmath>

scl::Pos2D::Pos2D(long double x, long double y) : _x(x), _y(y)
{
}

scl::Pos2D::Pos2D(const Pos2D &pos) : _x(pos._x), _y(pos._y)
{
}

scl::Pos2D::~Pos2D()
{
}

double scl::Pos2D::distance(Pos2D pos) {
	return std::sqrt((_x - pos._x) * (_x - pos._x) + (_y - pos._y) * (_y - pos._y));
}