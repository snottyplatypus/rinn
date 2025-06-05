#include "rinn.h"
#include "worldgen/worldgen.h"
#include <scroll.h>
#include <iostream>
#include <ctime>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
#include <fstream>
#include <filesystem>
#include <string>

int main()
{
	scl::System::init();

	rnn::WorldGen gen;
	scl::World world;
	gen.generate(world);

	while (!WindowShouldClose())
	{
		BeginDrawing();
		ClearBackground(BLACK);
		
		scl::renderer->drawFont("Rinn", scl::renderer->_window._width / 2, scl::renderer->_window._height / 2 - 128, 5);
		scl::renderer->drawSprite("apache", scl::renderer->_window._width / 2, scl::renderer->_window._height / 2 + 128, 5);
		EndDrawing();
	}

	return 0;
}