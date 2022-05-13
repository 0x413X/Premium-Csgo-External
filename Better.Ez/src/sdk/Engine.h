#pragma once

namespace Engine {
	DWORD GetEnginePointer(const Memory& mem);
	bool IsInGame(const Memory& mem);
	void ForceUpdate(const Memory& mem);
}
