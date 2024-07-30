#include "command.h"
#include "print.h"
#include "asm/type.h"
#include "string.h"
#include "../drivers/page_tlb_test.h"

static int cmd_memory_test_handler(int argc, char *argv[], void *priv)
{
	unsigned long addr = get_memory_test_addr(FIRST_MAPPING_ADDR);
	unsigned long addr1 = get_memory_test_addr(SECOND_MAPPING_ADDR);

	print("addr:0x%lx -- %s, 0x%lx\n", addr, addr, &addr);
	print("addr1:0x%lx -- %s, 0x%lx\n", addr1, addr1, &addr1);

	if (argv[0] != NULL)
		if (atoi(argv[0]) == 1) {
			if(!strncmp((char *)addr, "The host memory pa1!",strlen("The host memory pa1!")) &&
				!strncmp((char *)addr1, "The host memory pa2!",strlen("The host memory pa2!")))
					print(" hfence_gvma.all is success! \n");
			else {
				if (atoi(argv[1]) == 1)
					print(" hfence_gvma.all is failed! \n");
			}
		}

	return 0;
}

static const struct command cmd_memory_test = {
	.cmd = "memory_test",
	.handler = cmd_memory_test_handler,
	.priv = NULL,
};

int cmd_memory_test_init(void)
{
	register_command(&cmd_memory_test);

	return 0;
}

APP_COMMAND_REGISTER(memory_test, cmd_memory_test_init);
