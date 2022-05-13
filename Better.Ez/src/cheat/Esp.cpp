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
						//Model Color
						mem.Write<BYTE>(curEnt + 0x70, BYTE((Settings::esp.chamsTeamColor[0] * 255))); //r
						mem.Write<BYTE>(curEnt + 0x71, BYTE((Settings::esp.chamsTeamColor[1] * 255))); //g
						mem.Write<BYTE>(curEnt + 0x72, BYTE((Settings::esp.chamsTeamColor[2] * 255))); //b

						//Set model brightness
						DWORD thisPtr = (int)(Globals::engineModule + model_ambient_min - 0x2c);
						DWORD xored = *(DWORD*)&Settings::esp.chamsTeamBright ^ thisPtr;
						mem.Write<int>(Globals::engineModule + model_ambient_min, xored);
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
						//Model Color
						mem.Write<BYTE>(curEnt + 0x70, BYTE((Settings::esp.chamsEnemyColor[0] * 255))); //r
						mem.Write<BYTE>(curEnt + 0x71, BYTE((Settings::esp.chamsEnemyColor[1] * 255))); //g
						mem.Write<BYTE>(curEnt + 0x72, BYTE((Settings::esp.chamsEnemyColor[2] * 255))); //b

						//Set model brightness
						DWORD thisPtr = (int)(Globals::engineModule + model_ambient_min - 0x2c);
						DWORD xored = *(DWORD*)&Settings::esp.chamsEnemyBright ^ thisPtr;
						mem.Write<int>(Globals::engineModule + model_ambient_min, xored);
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
