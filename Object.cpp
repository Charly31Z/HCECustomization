#include "Object.h"

ObjectTable::ObjectTable(BYTE* base) : base(base)
{
	object_header_pointer = 0x400506B4;
}

uint16_t lowerWord16(uint32_t* x)
{
    int highervals = *x / std::pow(2, 16);
    highervals = highervals * std::pow(2, 16);
    uint16_t lowervals = *x - highervals;
    return lowervals;
}

Object ObjectTable::get_object(uint32_t* id)
{
    uint32_t* obj_header = (uint32_t*)(object_header_pointer);
    int obj_header_size = 0x38;

    char obj_header_name[6];
    memcpy(obj_header_name, obj_header, sizeof(obj_header_name));

    //char* player_header_name = (char*)(player_header + 0x8);
    uint16_t* obj_header_maxobjs = (uint16_t*)(object_header_pointer + 0x20);
    uint16_t* obj_table_size = (uint16_t*)(object_header_pointer + 0x22);

    uint16_t obj_table_index = lowerWord16(id);
    uint32_t* obj_table_base = obj_header + obj_header_size;
    uint32_t* obj_table_address = obj_table_base + ((uint16_t)obj_table_index * (uint16_t)obj_table_size) + 0x8;

    uint32_t* obj_struct = (uint32_t*)(obj_table_address + 0x0);

	return Object(obj_struct);
}

Object::Object(uint32_t* addrData) : data(addrData)
{
    obj_is_on_ground = (bool*)(addrData + 0x10);

    obj_region_permutation_index = (uint8_t*)(addrData + 0x180);
}
