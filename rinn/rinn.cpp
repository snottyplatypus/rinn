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
		scl::system->renderer()->drawFont("Rinn", scl::system->renderer()->_data_window._width / 2 - 32, scl::system->renderer()->_data_window._height / 2, WHITE);

		EndDrawing();
	}

	return 0;
}