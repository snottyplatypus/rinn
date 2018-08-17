#include "space_2d.h"

template<class T>
scl::Space2D<T>::Space2D(int width, int height) : _width(width), _height(height)
{
}

template<class T>
scl::Space2D<T>::~Space2D()
{
}

template<class T>
T & scl::Space2D<T>::operator()(int x, int y)
{
	return _data[x + _width * y]
}
