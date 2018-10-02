#ifndef SPACE_2D_H
#define SPACE_2D_H

#include <map>

namespace scl
{
	template<class T>
	class Space2D
	{
	public:
		Space2D(double width, double height);
		~Space2D();

		T& operator()(int x, int y);

	private:
		std::map<double, T> _data;
		double _width;
		double _height;
	};
}

#endif