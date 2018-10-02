#ifndef POS_3D_H
#define POS_3D_H

namespace scl
{
	template<typename T>
	class Pos3D
	{
	public:
		Pos3D(T x, T y, T z);
		~Pos3D();

		T _x;
		T _y;
		T _z;
	};
}

#endif