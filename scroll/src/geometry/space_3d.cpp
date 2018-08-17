#include "space_3d.h"


template<class T>
scl::Space3D<T>::Space3D(int width, int height, int depth): _width(width), _height(height), _depth(depth)
{
}

template<class T>
scl::Space3D<T>::~Space3D()
{
}

template<class T>
T & scl::Space3D<T>::operator()(int x, int y, int z)
{
	return _data[x + _width * (y + _depth * z)]
}
