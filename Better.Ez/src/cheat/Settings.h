#pragma once

namespace Settings {
	inline struct Aim {
		bool aimbotEnabled = false;
		float aimbotFov = 0.f;
		float aimbotSmooth = 0.f;
		bool aimbotTeammates = false;

		bool rcsEnabled = false;
		int rcsHorizontal = 0;
		int rcsVertical = 0;
	}aim;
	inline struct Esp {
		bool glowEnemy = false;
		float glowEnemyColor[4] = {1.f, 0.f, 0.f, 1.f};
		bool glowTeam = false;
		float glowTeamColor[4] = { 0.f, 1.f, 0.f, 1.f };

		bool chamsEnemy = false;
		float chamsEnemyColor[4] = { 1.f, 0.f, 0.f, 1.f };
		bool chamsTeam = false;
		float chamsTeamColor[4] = { 0.f, 1.f, 0.f, 1.f };
	}esp;
	inline struct Visuals {
		bool noHands = false;
	}vis;
	inline struct Misc {
		bool radar = false;

		bool bhop = false;
		int bhopPercentage = 0;
	}misc;
}