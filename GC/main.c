#include <stdlib.h>
#include "GC.h"
#include <stdio.h>

int main()
{
	VM l_vm = get_new_stack_vm();
	VM* vm = &l_vm;

	void** p = vm->gcalloc(vm, 32 * sizeof(int32_t));
	
	printf("%d\n", vm_get_num_objects(vm));

	for (int i = 0; i < 32 / 4; i++)
	{
		(*(int32_t**)p)[i] = 111;
	}

	vm->gc(vm);

	printf("%d\n", vm_get_num_objects(vm));

	vm_pop(vm);

	vm->gc(vm);

	printf("%d\n", vm_get_num_objects(vm));
	
	
	getchar();
	return 0;
}
