#ifndef POS_2D_H
#define POS_2D_H

#include <memory>

namespace scl
{
	class Pos2D
	{
	public:
		Pos2D(long double x, long double y);
		Pos2D(const Pos2D &pos);
		~Pos2D();

		double distance(Pos2D pos);

		long double _x;
		long double _y;
	};

	typedef std::shared_ptr<scl::Pos2D> Pos2D_uptr;
	typedef std::shared_ptr<scl::Pos2D> Pos2D_sptr;
}

#endif