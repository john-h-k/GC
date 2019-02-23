#pragma once
#include <stdint.h>
#include "object.h"
#include "VM.h"

ObjectHeader * objectalloc(size_t size);

void gc(VM*);