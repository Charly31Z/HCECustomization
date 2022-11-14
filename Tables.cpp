#include "Tables.h"

Tables::Tables(BYTE* base) : base(base)
{
	players = new PlayerTable(base);
	objects = new ObjectTable(base);
}
