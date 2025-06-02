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
		std::shared_ptr<Renderer> renderer() const { return _renderer; }
	private:
		std::shared_ptr<Renderer> _renderer;
	};

	extern std::shared_ptr<Engine> system;

	namespace System 
	{
		void init();
	}

	extern std::unique_ptr<scl::DefaultPRNG> rand;
}

#endif