#ifndef POS_3D_H
#define POS_3D_H

namespace scl
{
	template<typename T>
	class Pos3D
	{
	public:
		Pos3D(T x, T y, T z) : _x(x), _y(y), _z(z) {}
		~Pos3D() {}

		T _x;
		T _y;
		T _z;

		template<class Archive>
		void serialize(Archive& ar) const
		{
			ar(_x, _y, _z);
		}
	};
}

#endif