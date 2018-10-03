#ifndef SPACE_2D_H
#define SPACE_2D_H

#include <map>

namespace scl
{
	template<class T>
	class Space2D
	{
	public:
		Space2D();
		Space2D(double width, double height);
		~Space2D();

		void add(T data, double x, double y);

		T& operator()(double x, double y);

	private:
		std::map<double, T> _data;
		double _width;
		double _height;
	};
}

#endif