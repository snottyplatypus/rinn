#include "geometry/space_2d.h"

template<class T>
scl::Space2D<T>::Space2D(double width, double height) : _width(width), _height(height)
{
}

template<class T>
scl::Space2D<T>::~Space2D()
{
}

template<class T>
T & scl::Space2D<T>::operator()(double x, double y)
{
	return _data[x + _width * y]
}
