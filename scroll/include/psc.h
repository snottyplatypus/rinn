#ifndef PSC_H
#define PSC_H

#include <vector>
#include <utility>
#include <cmath>
#include <memory>
#include "pos_2d.h"

namespace scl {
	class PoissonDiskSampling {
	public:
		PoissonDiskSampling(int width, int height, double min_dist, int point_count);
		~PoissonDiskSampling(void);

		std::vector<std::pair<double, double> > Generate();

	private:
		
		std::vector<std::vector<scl::Pos2D_sptr>> _grid;
		std::vector<scl::Pos2D> _process;
		std::vector<std::pair<double, double>> _sample;

		int _width;
		int _height;
		double _min_dist;
		int _point_count;
		double _cell_size;
		int _grid_width;
		int _grid_height;

		scl::Pos2D generatePointAround(scl::Pos2D p_point);
		bool inRectangle(scl::Pos2D p_point);
		bool inNeighbourhood(scl::Pos2D p_point);
		std::vector<scl::Pos2D_sptr> getCellsAround(scl::Pos2D p_point);

		const double PI = 3.14159265358979323846;
	};
}

#endif