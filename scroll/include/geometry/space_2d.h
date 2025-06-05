#ifndef SPACE_2D_H
#define SPACE_2D_H

#include <map>
#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/access.hpp>

namespace scl
{
	template<class T>
	class Space2D
	{
	public:
        Space2D() : _width(0.0), _height(0.0) {}
        Space2D(double width, double height) : _width(width), _height(height) {}
		~Space2D() {}

        void add(double x, double y, T data)
        {
            _data[x + _width * y] = data;
        }

        T& operator()(double x, double y)
        {
            return _data[x + _width * y];
        }

        template<class Archive>
        void save(Archive& ar) const
        {
            ar(_data, _width, _height);
        }

        friend class cereal::access;

	private:
		std::map<double, T> _data;
		double _width;
		double _height;
	};
}

#endif