#ifndef SYSTEM_H
#define SYSTEM_H

#include "renderer.h"
#include "random.h"
#include <memory>

namespace scl
{
	class Engine
	{
	public:
		Engine();
		~Engine();

		void init();
	};

	namespace System 
	{
		void init();
	}

	extern std::shared_ptr<Engine> system;
	extern std::unique_ptr<scl::DefaultPRNG> rand;
	extern std::shared_ptr<Renderer> renderer;

}

#endif