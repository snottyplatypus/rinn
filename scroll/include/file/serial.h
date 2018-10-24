#ifndef SERIAL_H
#define SERIAL_H

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel					K;
typedef K::Point_2															Point_2;

/*
template<class Archive>
inline void serialize(Archive & ar, Point_2 & p)
{
	ar(static_cast<double>(p.x()), static_cast<double>(p.y()));
}
*/
#endif