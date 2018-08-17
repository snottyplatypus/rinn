#include "rinn.h"
#include "scroll.h"

int main()
{
	scl::System::init();

	scl::World world;

	TCODConsole::root->flush();
	TCODConsole::root->waitForKeypress(true);
	return 0;
}
