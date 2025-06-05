#include "system.h"

std::shared_ptr<scl::Engine> scl::system;
std::unique_ptr<scl::DefaultPRNG> scl::rand;
std::shared_ptr<scl::Renderer> scl::renderer;

scl::Engine::Engine()
{
}

scl::Engine::~Engine()
{
}

void scl::Engine::init()
{
	scl::rand = std::make_unique<scl::DefaultPRNG>();
}

void scl::System::init()
{
	scl::system = std::make_shared<scl::Engine>();
	scl::system->init();
	renderer = std::make_shared<Renderer>();
	renderer->init();
}