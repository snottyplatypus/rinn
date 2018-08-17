#ifndef SYSTEM_H
#define SYSTEM_H

#include "renderer.h"
#include <memory>
#include <libtcod.hpp>

namespace scl
{
	class Engine
	{
	public:
		Engine();
		~Engine();

		void init();
	private:
		std::shared_ptr<Renderer> _renderer;
	};

	extern std::shared_ptr<Engine> system;

	namespace System 
	{
		void init();
	}

	extern std::unique_ptr<TCODRandom> rand;
}

#endif