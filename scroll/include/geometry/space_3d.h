#ifndef SPACE_3D_H
#define SPACE_3D_H

#include <map>

namespace scl
{
	template<class T>
	class Space3D
	{
	public:
		Space3D();
		Space3D(double width, double height, double depth);
		~Space3D();

		void add(T data, double x, double y, double z);

		T& operator()(double x, double y, double z);

	private:
		std::map<int, T> _data;
		double _width;
		double _height;
		double _depth;
	};
}

#endif