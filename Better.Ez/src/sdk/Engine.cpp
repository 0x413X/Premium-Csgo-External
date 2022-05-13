#include "../Framework.h"

DWORD Engine::GetEnginePointer(const Memory& mem)
{
	return mem.Read<int>(Globals::engineModule + dwClientState);
}

bool Engine::IsInGame(const Memory& mem)
{
	return true;
	//if (mem.Read<int>(Globals::engineModule + dwClientState + dwClientState_State) == 6)
		//return true;
	//return false;
}

void Engine::ForceUpdate(const Memory& mem)
{
	mem.Write<int>(Engine::GetEnginePointer(mem) + 0x16C, -1);
}
