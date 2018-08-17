#ifndef SPACE_3D_H
#define SPACE_3D_H

#include <map>

namespace scl
{
	template<class T>
	class Space3D
	{
	public:
		Space3D(int width, int height, int depth);
		~Space3D();

		T& operator()(int x, int y, int z);

	private:
		std::map<int, T> _data;
		int _width;
		int _height;
		int _depth;
	};
}

#endif