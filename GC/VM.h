#pragma once

#define DEFINITE_STACK

#if defined(DEFINITE_STACK)
#define STACK_SIZE 512
#endif

typedef struct _Object_ll
{
	ObjectHeader* current;
	struct _Object_ll* next;
	struct _Object_ll* prev;
} Object_ll;

typedef struct _VM VM;

typedef void** (*allocator)(VM* vm, size_t size);
typedef void   (*collector)(VM* vm);

typedef struct _VM
{
	const collector gc;
	const allocator gcalloc;
	size_t stack_size;
	size_t index_nextfree;
	Object_ll* alloc_head;
	Object_ll* alloc_tail;
#if defined(DEFINITE_STACK)
	void* stack[STACK_SIZE];
#else
	ObjectHeader* stack[];
#endif

} VM;

#if defined(DEFINITE_STACK)
VM* get_new_heap_vm();
#else
VM* get_new_heap_vm(size_t stack_size);
#endif

#if defined(DEFINITE_STACK)
VM get_new_stack_vm();
#else
#endif

int vm_get_num_objects(VM* this);
uint32_t vm_push(VM* this, void* p);
void* vm_pop(VM* this);