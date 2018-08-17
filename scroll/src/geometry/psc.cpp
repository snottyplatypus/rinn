#include "psc.h"
#include <algorithm>
#include <libtcod.hpp>
#include <system.h>

scl::PoissonDiskSampling::PoissonDiskSampling(int width, int height, double min_dist, int point_count) {
	_width			= width;
	_height			= height;
	_min_dist		= min_dist;
	_point_count	= point_count;
	_cell_size		= _min_dist / 1.414214;
	_grid_width		= static_cast<int>(std::ceil(_width / _cell_size));
	_grid_height	= static_cast<int>(std::ceil(_height / _cell_size));
	_grid			= std::vector<std::vector<scl::Pos2D_sptr>>(_grid_width, std::vector<scl::Pos2D_sptr>(_grid_height, NULL));
}

scl::PoissonDiskSampling::~PoissonDiskSampling(void)
{
}

std::vector<std::pair<double, double>> scl::PoissonDiskSampling::Generate() {

	scl::Pos2D first_point(scl::rand->getDouble(0, _width), scl::rand->getDouble(0, _height));

	_process.push_back(first_point);
	_sample.push_back(std::make_pair(first_point._x, first_point._y));
	int first_point_x = static_cast<int>(first_point._x / _cell_size);
	int first_point_y = static_cast<int>(first_point._y / _cell_size);
	_grid[first_point_x][first_point_y] = std::make_shared<scl::Pos2D>(first_point);

	while (!_process.empty()) {
		int new_point_index = scl::rand->getInt(0, static_cast<int>(_process.size()));
		scl::Pos2D new_point = _process[new_point_index];
		_process.erase(_process.begin() + new_point_index);

		for (int i = 0; i < _point_count; i++) {
			scl::Pos2D new_point_around = generatePointAround(new_point);

			if (inRectangle(new_point_around) && !inNeighbourhood(new_point_around)) {
				_process.push_back(new_point_around);
				_sample.push_back(std::make_pair(new_point_around._x, new_point_around._y));
				int new_point_x = static_cast<int>(new_point_around._x / _cell_size);
				int new_point_y = static_cast<int>(new_point_around._y / _cell_size);
				_grid[new_point_x][new_point_y] = std::make_shared<scl::Pos2D>(new_point_around);
			}
		}
	}

	return _sample;
}

scl::Pos2D scl::PoissonDiskSampling::generatePointAround(scl::Pos2D p_point) {
	double r1 = scl::rand->getDouble(0, RAND_MAX);
	double r2 = scl::rand->getDouble(0, RAND_MAX);

	double radius = _min_dist * (r1 + 1);

	double angle = 2 * PI * r2;

	double new_x = p_point._x + radius * cos(angle);
	double new_y = p_point._y + radius * sin(angle);

	return scl::Pos2D(new_x, new_y);
}

bool scl::PoissonDiskSampling::inRectangle(scl::Pos2D p_point) {
	return (p_point._x >= 0 && p_point._y >= 0 && p_point._x < _width && p_point._y < _height);
}

bool scl::PoissonDiskSampling::inNeighbourhood(scl::Pos2D p_point) {
	std::vector<scl::Pos2D_sptr> cells = getCellsAround(p_point);
	int size = static_cast<int>(cells.size());
	for (int i = 0; i < size; i++) {
		if (cells[i]->distance(p_point) < _min_dist) {
			return true;
		}
	}
	return false;
}

std::vector<scl::Pos2D_sptr> scl::PoissonDiskSampling::getCellsAround(scl::Pos2D p_point) {
	std::vector<scl::Pos2D_sptr> cells;
	int x_index = static_cast<int>(p_point._x / _cell_size);
	int y_index = static_cast<int>(p_point._y / _cell_size);

	int min_x = std::max(0, x_index - 1);
	int max_x = std::min(_grid_width - 1, x_index + 1);

	int min_y = std::max(0, y_index - 1);
	int max_y = std::min(_grid_height - 1, y_index + 1);

	for (int i = min_x; i <= max_x; i++) {
		for (int j = min_y; j <= max_y; j++) {
			if (_grid[i][j] != NULL) {
				cells.push_back(_grid[i][j]);
			}
		}
	}
	return cells;
}