#include "../Framework.h"

DWORD LocalPlayer::GetAddress(const Memory& mem)
{
	return (Globals::clientModule + dwLocalPlayer);
}

int LocalPlayer::GetTeam(const Memory& mem)
{
	return mem.Read<int>(LocalPlayer::GetAddress(mem) + m_iTeamNum);
}
