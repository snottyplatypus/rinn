#include "geometry/space_2d.h"

template<class T>
scl::Space2D<T>::Space2D() : _width(0), _height(0)
{
}

template<class T>
scl::Space2D<T>::Space2D(double width, double height) : _width(width), _height(height)
{
}

template<class T>
scl::Space2D<T>::~Space2D()
{
}

template<class T>
void scl::Space2D<T>::add(double x, double y, T data)
{
	_data[x + _width * y] = data;
}

template<class T>
T& scl::Space2D<T>::operator()(double x, double y)
{
	return _data[x + _width * y]
}
