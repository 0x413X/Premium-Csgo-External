#include "../Framework.h"

void Misc::Run(const Memory& mem)
{
	DWORD fFlags;
	while (Globals::bRunning)
	{
		if (Engine::IsInGame(mem))
		{
			if (Settings::misc.radar)
			{
				for (int i = 0; i < 64; i++)
				{
					if(Entity::GetTeam(mem, EntityList::GetEntityBase(mem, i) != LocalPlayer::GetTeam(mem)))
						mem.Write<bool>(EntityList::GetEntityBase(mem, i) + m_bSpotted, true);
				}
			}

			if (Settings::misc.bhop)
			{
				if (GetAsyncKeyState(VK_SPACE))
				{
					fFlags = mem.Read<DWORD>(LocalPlayer::GetAddress(mem) + m_fFlags);
					if (fFlags == 256)
					{
						mem.Write<int>(Globals::clientModule + dwForceJump, 4);
					}
					else 
					{
						mem.Write<int>(Globals::clientModule + dwForceJump, 5);
					}
				}
			}
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
	}
}
