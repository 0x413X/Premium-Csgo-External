#include "../Framework.h"

int Entity::GetTeam(const Memory& mem, DWORD entBase)
{
	return mem.Read<int>(entBase + m_iTeamNum);
}
