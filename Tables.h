#pragma once

#include "Player.h"
#include "Object.h"

struct Tables
{
private:
	BYTE* base;

public:
	PlayerTable* players;
	ObjectTable* objects;

	Tables(BYTE* base);
};

