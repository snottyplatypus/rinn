#ifndef SPACE_2D_H
#define SPACE_2D_H

#include <map>

namespace scl
{
	template<class T>
	class Space2D
	{
	public:
		Space2D(int width, int height);
		~Space2D();

		T& operator()(int x, int y);

	private:
		std::map<int, T> _data;
		int _width;
		int _height;
	};
}

#endif