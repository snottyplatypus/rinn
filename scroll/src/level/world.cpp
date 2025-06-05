#include "world.h"
#include <cereal/archives/binary.hpp>
#include <fstream>
#include <filesystem>

scl::World::World()
{
	_chunks = std::make_shared<Space2D<int>>();
}

scl::World::~World()
{
}

void scl::World::saveTo(const std::string& path) const
{
	std::cout << "Saving world.. ";
	std::ofstream file(path + "/world.dat", std::ios::binary | std::ios::out);
	cereal::BinaryOutputArchive archive(file);
	archive(*this);
	file.close();
	std::cout << "done at " << path << std::endl;
}