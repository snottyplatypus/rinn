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
	scl::World world = gen.generate();

	std::cout << "Saving world.. ";
	std::string path = "../data/world";
	std::string sub_name = "world";
	int n_world = 1;
	for (auto & p : std::filesystem::directory_iterator(path)) {
		std::string f_name = std::filesystem::path(p).filename().string();
		std::size_t found = f_name.find(sub_name);
		if (found != std::string::npos)
			n_world++;
	}
	sub_name += std::to_string(n_world);
	path += "/" + sub_name;
	std::filesystem::create_directory(path);

	std::ofstream file(path + "/gen.dat", std::ios::binary | std::ios::out);
	cereal::BinaryOutputArchive archive(file);
	archive(gen);
	file.close();
	std::cout << "done" << std::endl;

	TCODConsole::root->flush();
	TCODConsole::root->waitForKeypress(true);

	return 0;
}