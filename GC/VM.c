#include <stdlib.h>
#include "GC.h"
#include <stdint.h>
#include "VM.h"
#include <assert.h>
#include <malloc.h>
#include <string.h>

#define d_assert(expr, str) (void)assert(expr, str)

int vm_get_num_objects(VM* this)
{
	if (this->alloc_head == NULL) return 0;
	if (this->alloc_head == this->alloc_tail) return 1;

	int counter = 1;
	Object_ll* current = this->alloc_head;
	for (; current->next != NULL; current = current->next, counter++) { }
	return counter;
}

uint32_t vm_push(VM* this, void* p)
{
	assert(this->index_nextfree < this->stack_size);
	this->stack[this->index_nextfree] = p;
	return this->index_nextfree++;
}

void* vm_pop(VM* this)
{
	assert(this->index_nextfree > 0);
	return this->stack[this->index_nextfree--];
}

static void vm_internal_push(VM* this, ObjectHeader* p)
{
	assert(this->index_nextfree < this->stack_size);
	this->stack[this->index_nextfree++] = (uint8_t*)p + sizeof(ObjectHeader);
}
 
static ObjectHeader* vm_internal_pop(VM* this)
{
	assert(this->index_nextfree > 0);
	return (ObjectHeader*)((uint8_t*)this->stack[this->index_nextfree--] - sizeof(ObjectHeader));
}

static void** vm_objectalloc(VM* this, size_t size)
{
	ObjectHeader* p = objectalloc(size);

	if (this->alloc_head == NULL)
	{
		d_assert(this->alloc_tail == NULL, "Mem corrupted");
		this->alloc_head = malloc(sizeof(Object_ll));
		this->alloc_head->current = p;
		this->alloc_head->next = NULL;
		this->alloc_head->prev = NULL;
		this->alloc_tail = this->alloc_head;
	}
	else
	{
		Object_ll* end = this->alloc_tail;
		Object_ll* next = malloc(sizeof(Object_ll));
		next->next = NULL;
		next->current = p;
		next->prev = end;
		end->next = next;		
		this->alloc_tail = next;
	}

	vm_internal_push(this, p);

	// peek top of VM stack (of pointers), get the val, and return a pointer to it
	return (void**)&(this->stack[this->index_nextfree - 1]);
}

#if defined(DEFINITE_STACK)
VM* get_new_heap_vm()
{
	const VM temp_vm =
	{
		.alloc_head = NULL,
		.alloc_tail = NULL,
		.stack_size = STACK_SIZE,
		.gcalloc = vm_objectalloc,
		.index_nextfree = 0,
		.gc = gc
	};

	VM* p_vm = malloc(sizeof(VM));
	memcpy(p_vm, &temp_vm, sizeof(VM));

	return p_vm;
}
#else
VM* get_new_heap_vm(size_t stack_size)
{
	const VM temp_vm =
	{
		.alloc_head = NULL,
		.stack_size = STACK_SIZE,
		.alloc_tail = NULL,
		.gcalloc = vm_objectalloc,
		.index_nextfree = 0,
		.gc = gc
	};

	VM* p_vm = malloc(sizeof(VM) + sizeof(ObjectHeader) * stack_size);
	memcpy(p_vm, &temp_vm, sizeof(VM));

	return p_vm;
}
#endif

#if defined(DEFINITE_STACK)
VM get_new_stack_vm()
{
	const VM p_vm =
	{
		.alloc_head = NULL,
		.alloc_tail = NULL,
		.stack_size = STACK_SIZE,
		.gcalloc = vm_objectalloc,
		.index_nextfree = 0,
		.gc = gc
	};

	return p_vm;
}
#else
#endif