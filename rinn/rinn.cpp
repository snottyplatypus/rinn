#include "rinn.h"
#include "scroll.h"

int main()
{
	scl::System::init();

	scl::World world;
	scl::PoissonGenerator::DefaultPRNG PRNG;
	std::vector<Point_2> vec = scl::PoissonGenerator::generatePoissonPoints(10, PRNG, 1000, false, 0.001f);
	std::cout << "vec size : " << vec.size() << std::endl;
	for each (auto el in vec)
	{
		std::cout << el.x() << " " << el.y() << std::endl;
	}
	TCODConsole::root->flush();
	TCODConsole::root->waitForKeypress(true);
	return 0;
}