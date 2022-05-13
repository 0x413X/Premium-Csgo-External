#include "../Framework.h"

void Esp::Run(const Memory& mem)
{
	DWORD localPlayer;
	DWORD glowManager;

	DWORD curEnt;
	DWORD curGlowIndex;

	while (Globals::bRunning)
	{
		if (Engine::IsInGame(mem))
		{
			localPlayer = LocalPlayer::GetAddress(mem);
			glowManager = mem.Read<DWORD>(Globals::clientModule + dwGlowObjectManager);

			for (int i = 0; i < 64; i++)
			{
				curEnt = EntityList::GetEntityBase(mem, i);
				curGlowIndex = mem.Read<int>(curEnt + m_iGlowIndex);

				int lifeState = mem.Read<int>(curEnt + m_lifeState);
				if (lifeState != 0)
					continue;

				if (Entity::GetTeam(mem, curEnt) == LocalPlayer::GetTeam(mem))
				{ //team
					if (Settings::esp.glowTeam)
					{
						mem.Write<float>(glowManager + (curGlowIndex * 0x38) + 0x8, Settings::esp.glowTeamColor[0]);
						mem.Write<float>(glowManager + (curGlowIndex * 0x38) + 0xC, Settings::esp.glowTeamColor[1]);
						mem.Write<float>(glowManager + (curGlowIndex * 0x38) + 0x10, Settings::esp.glowTeamColor[2]);
						mem.Write<float>(glowManager + (curGlowIndex * 0x38) + 0x14, Settings::esp.glowTeamColor[3]);

						mem.Write<bool>(glowManager + (curGlowIndex * 0x38) + 0x27, true);
						mem.Write<bool>(glowManager + (curGlowIndex * 0x38) + 0x28, true);
					}
					if (Settings::esp.chamsTeam)
					{
						
					}
				}
				else
				{ //enemy
					if (Settings::esp.glowEnemy)
					{
						mem.Write<float>(glowManager + (curGlowIndex * 0x38) + 0x8, Settings::esp.glowEnemyColor[0]);
						mem.Write<float>(glowManager + (curGlowIndex * 0x38) + 0xC, Settings::esp.glowEnemyColor[1]);
						mem.Write<float>(glowManager + (curGlowIndex * 0x38) + 0x10, Settings::esp.glowEnemyColor[2]);
						mem.Write<float>(glowManager + (curGlowIndex * 0x38) + 0x14, Settings::esp.glowEnemyColor[3]);

						mem.Write<bool>(glowManager + (curGlowIndex * 0x38) + 0x27, true);
						mem.Write<bool>(glowManager + (curGlowIndex * 0x38) + 0x28, true);
					}
					if (Settings::esp.chamsEnemy)
					{

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
