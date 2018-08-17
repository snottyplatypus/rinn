#ifndef SYSTEM_H
#define SYSTEM_H

#include "renderer.h"
#include <memory>

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
}

#endif