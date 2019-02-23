#include <stdlib.h>
#include "GC.h"
#include "VM.h"


ObjectHeader * objectalloc(size_t size)
{
	ObjectHeader* p = malloc(sizeof(ObjectHeader) + size);
	p->marked = 0;
	return p;
}

static void mark_object_header(void* header)
{
	((ObjectHeader*)((uint8_t*)header - sizeof(ObjectHeader)))->marked = 1;
}

static void mark_vm_stack(VM* p_vm)
{
	const int range = p_vm->index_nextfree;
	for (int i = 0; i < range; i++)
	{
		void* potential_gc_ref = p_vm->stack[i];
		if (potential_gc_ref != NULL)
			mark_object_header(potential_gc_ref);
	}
}

static void sweep_vm_stack(VM* p_vm)
{
	Object_ll * current_node = p_vm->alloc_head;

	while (current_node != NULL)
	{
		Object_ll* t_next_current;

		if (current_node->current->marked == 0)
		{
			Object_ll* t_prev = current_node->prev;
			t_next_current = current_node->next;

			free(current_node->current);
			free(current_node);

			if (t_prev == NULL)
			{
				p_vm->alloc_head = t_next_current;
			}
			else
			{
				// HS
				t_prev->next = t_next_current;
			}

			if (t_next_current != NULL)
			{
				t_next_current->prev = t_prev;
			}
		}
		else
		{
			current_node->current->marked = 0; 
			t_next_current = current_node->next;
		}
		current_node = t_next_current;
	}
}

void gc(VM* p_vm)
{
	mark_vm_stack(p_vm);
	sweep_vm_stack(p_vm);
}