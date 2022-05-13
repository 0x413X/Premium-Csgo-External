#include "../Framework.h"

void Visuals::Run(const Memory& mem)
{

	while (Globals::bRunning)
	{
		if (Engine::IsInGame(mem))
		{
			if (Settings::vis.noHands)
			{
				mem.Write<int>(LocalPlayer::GetAddress(mem) + m_nViewModelIndex, 0);
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		}
		
	}

}
