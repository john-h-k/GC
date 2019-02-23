#pragma once
#include <stdint.h>

typedef struct _ObjectHeader
{
	uint8_t marked;
	uint8_t data[];
} ObjectHeader;
