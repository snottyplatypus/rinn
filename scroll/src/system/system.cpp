#include "system.h"

std::shared_ptr<scl::Engine> scl::system;
std::unique_ptr<scl::DefaultPRNG> scl::rand;

scl::Engine::Engine()
{
	_renderer = nullptr;
}

scl::Engine::~Engine()
{
}

void scl::Engine::init()
{
	_renderer = std::make_shared<Renderer>();
	_renderer->init();

	scl::rand = std::make_unique<scl::DefaultPRNG>();
}

void scl::System::init()
{
	scl::system = std::make_shared<scl::Engine>();
	scl::system->init();
}