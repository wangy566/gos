#include <asm/type.h>
#include <print.h>
#include <device.h>
#include <string.h>
#include <dmac.h>
#include <timer.h>
#include "../command.h"

static void Usage()
{
	print("dma_test [dst_addr] [src_addr] [size]\n");
}

static int cmd_dma_test_handler(int argc, char *argv[], void *priv)
{
	int ret = 0;
	void *dst, *src;
	unsigned int size;
	unsigned int diff = 0, start_time = 0;

	if (argc != 3) {
		Usage();
		return -1;
	}

	if (!is_digit((char *)argv[0]) || !is_digit((char *)argv[1])
	    || !is_digit((char *)argv[2])) {
		print("invalid input params.\n");
		return -1;
	}

	dst = (void *)atoi(argv[0]);
	src = (void *)atoi(argv[1]);
	size = atoi(argv[2]);

	start_time = get_system_time_ms();
	ret = memcpy_hw(dst, src, size);
	if (ret == -1) {
		print("memcpy_hw failed, timeout...\n");
		return -1;
	}

	diff = get_system_time_ms() - start_time;

	print("dma_test success! cost: %dms\n", diff);

	return 0;
}

static const struct command cmd_dma_test = {
	.cmd = "dma_test",
	.handler = cmd_dma_test_handler,
	.priv = NULL,
};

int dma_test_init()
{
	register_command(&cmd_dma_test);

	return 0;
}

APP_COMMAND_REGISTER(dma_test, dma_test_init);
