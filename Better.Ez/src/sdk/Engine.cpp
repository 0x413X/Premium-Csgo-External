#include "../Framework.h"

bool Engine::IsInGame(const Memory& mem)
{
	return true;
	//if (mem.Read<int>(Globals::engineModule + dwClientState + dwClientState_State) == 6)
		//return true;
	//return false;
}
