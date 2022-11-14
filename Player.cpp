#include "Player.h"

PlayerTable::PlayerTable(BYTE* base) : base(base)
{

    player_header_pointer = 0x402AAF8C;
}

Player PlayerTable::get_player(int index)
{
    uint32_t* player_header = (uint32_t*)(player_header_pointer) - 0x8;
    int player_header_size = 0x40;

    char player_header_name[14];
    memcpy(player_header_name, player_header + 0x8, sizeof(player_header_name));

    //char* player_header_name = (char*)(player_header + 0x8);
    uint16_t* player_header_maxplayers = (uint16_t*)(player_header_pointer + 0x28);
    uint16_t* player_struct_size = (uint16_t*)(player_header_pointer + 0x2A);

    //char* player_header_data = (char*)(player_header + 0x30);
    uint16_t* player_header_ingame = (uint16_t*)(player_header_pointer + 0x34);
    uint16_t* player_header_current_players = (uint16_t*)(player_header_pointer + 0x36);

    uint32_t* player_base = player_header + player_header_size;
    uint32_t* player_struct = player_base + (index * (uint16_t)player_struct_size);

    return Player(player_struct);
}

Player::Player(uint32_t* addrData)
{
    this->data = addrData;

    player_id = (uint16_t*)(addrData);

    player_team = (uint8_t*)(addrData + 0x20);

    player_obj_id = (uint32_t*)(addrData + 0x34);
}
