#include "../Framework.h"

DWORD EntityList::GetEntityBase(const Memory& mem, int index)
{
	return (Globals::clientModule + dwEntityList + index * 0x10);
}