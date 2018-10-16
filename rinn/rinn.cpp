#include "rinn.h"
#include "scroll.h"
#include <iostream>
#include <vector>
#include <ctime>

int main()
{
	scl::System::init();
	scl::World world;
	scl::PoissonGenerator::DefaultPRNG PRNG;

	std::clock_t timer = clock();
	std::cout << "Generating points.. ";
	std::vector<Point_2> points = scl::PoissonGenerator::generatePoissonPoints(10000, PRNG, 10, true, 0.01f);
	std::cout << (clock() - timer) / (CLOCKS_PER_SEC / 1000) << "ms" << std::endl;
	std::cout << "Points generated : " << points.size() << std::endl;

	int ImageSize = 1024;
	scl::file::BMP_Image image(ImageSize, ImageSize);

	for (auto el : points)
	{
		int x = int(el.x() * ImageSize);
		int y = int(el.y() * ImageSize);
		image.put(x, y, 0xFFFFFF);
	}
	image.save("points.bmp");

	TCODConsole::root->flush();
	TCODConsole::root->waitForKeypress(true);

	return 0;
}