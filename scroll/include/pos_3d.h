#ifndef POS_3D_H
#define POS_3D_H

namespace scl
{
	class Pos3D
	{
	public:
		Pos3D(int x, int y, int z);
		~Pos3D();

		long int _x;
		long int _y;
		long int _z;
	};
}

#endif