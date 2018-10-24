#include "geometry/space_3d.h"

template<class T>
scl::Space3D<T>::Space3D() : _width(0), _height(0), _depth(0)
{
}

template<class T>
scl::Space3D<T>::Space3D(double width, double height, double depth): _width(width), _height(height), _depth(depth)
{
}

template<class T>
scl::Space3D<T>::~Space3D()
{
}

template<class T>
void scl::Space3D<T>::add(double x, double y, double z, T data)
{
	_data[x + _width * (y + _depth * z)] = data;
}

template<class T>
T& scl::Space3D<T>::operator()(double x, double y, double z)
{
	return _data[x + _width * (y + _depth * z)]
}
