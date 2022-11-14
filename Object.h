#pragma once
#include <wtypes.h>
#include <cstdint>
#include <cmath>

struct Object
{
private:
	uint32_t* data;
public:
	
	bool* obj_is_on_ground;
	uint8_t* obj_region_permutation_index;

	Object(uint32_t* addrData);
};

struct ObjectTable
{
private:
	int object_header_pointer;
public:
	BYTE* base;

	ObjectTable(BYTE* base);

	Object get_object(uint32_t* id);
};