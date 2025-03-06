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
	//Init system and generate a world
	scl::System::init();
	rnn::WorldGen gen;
	scl::World world = gen.generate();
	
	std::cout << "Saving world.. ";
	std::string path = "../data/world";
	std::string sub_name = "world";
	int n_world = 1;
	//Check the number of world already saved and add 1 to this number
	for (auto & p : std::filesystem::directory_iterator(path)) {
		std::string f_name = std::filesystem::path(p).filename().string();
		std::size_t found = f_name.find(sub_name);
		if (found != std::string::npos)
			n_world++;
	}
	//Number of the new world saved
	sub_name += std::to_string(n_world);
	//Create the directory
	path += "/" + sub_name;
	std::filesystem::create_directory(path);

	//Save the world in a binary file with cereal
	std::ofstream file(path + "/gen.dat", std::ios::binary | std::ios::out);
	cereal::BinaryOutputArchive archive(file);
	archive(gen);
	file.close();

	std::cout << "done" << std::endl;

	TCODConsole::root->flush();
	TCODConsole::root->waitForKeypress(true);

	return 0;
}