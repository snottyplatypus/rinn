#ifndef SPACE_3D_H
#define SPACE_3D_H

#include <map>
#include <cereal/types/map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/access.hpp>

namespace scl
{
	template<class T>
	class Space3D
	{
	public:
		Space3D() : _width(0), _height(0), _depth(0) {}
		Space3D(double width, double height, double depth) : _width(width), _height(height), _depth(depth) {}
		~Space3D() {}

		void add(double x, double y, double z, T data)
		{
			_data[x + _width * (y + _depth * z)] = data;
		}

		T& operator()(double x, double y, double z)
		{
			return _data[x + _width * (y + _depth * z)];
		}

		template<class Archive>
		void save(Archive& ar) const
		{
			ar(_data, _width, _height, _depth);
		}

		friend class cereal::access;

	private:
		std::map<int, T> _data;
		double _width;
		double _height;
		double _depth;
	};
}

#endif