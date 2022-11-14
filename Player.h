#pragma once

#include <wtypes.h>
#include <cstdint>

struct Player
{
private:
	uint32_t* data;
public:
	uint16_t* player_id;
	char player_name[12];
	uint8_t* player_team;

	uint32_t* player_obj_id;

	Player(uint32_t* addrData);
};

struct PlayerTable
{
private:
	int player_header_pointer;
public:
	BYTE* base;

	PlayerTable(BYTE* base);

	Player get_player(int index);
};

/*struct Player
{
	static PlayerTable* playerTable;
};
*/
