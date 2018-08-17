#ifndef POS_2D_H
#define POS_2D_H

namespace scl
{
	class Pos2D
	{
	public:
		Pos2D(int x, int y);
		~Pos2D();

		long int _x;
		long int _y;
	};
}

#endif