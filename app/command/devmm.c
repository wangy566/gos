#include <asm/type.h>
#include <print.h>
#include <device.h>
#include <string.h>
#include <asm/mmio.h>
#include "../command.h"

#define READ_MEM 0
#define WRITE_MEM 1

static void usage()
{
	print("Usage: devmem ADDRESS [WIDTH [VALUE]]\n");
	print("\n");
	print("Read/write from physical address\n");
	print("\n");
	print("ADDRESS Address to act upon\n");
	print("WIDTH Width (8/32/64)\n");
	print("VALUE Data to be written\n");
}

static void wr_address(unsigned long address, int width, int wr,
		       unsigned long value)
{
	unsigned long val;

	if (wr == READ_MEM) {
		if (width == 8)
			val = readb(address);
		else if (width == 32)
			val = readl(address);
		else if (width == 64)
			val = readq(address);

		print("0x%x\n", val);
	} else if (wr == WRITE_MEM) {
		if (width == 8)
			writeb(address, value);
		else if (width == 32)
			writel(address, value);
		else if (width == 64)
			writeq(address, value);
	}
}

static int cmd_devmm_handler(int argc, char *argv[], void *priv)
{
	unsigned long where = 0;
	int wr, width = 32;
	unsigned long value;

	if (argc == 0 || !strncmp(argv[0], "help", sizeof("help"))) {
		usage();
		return 0;
	}

	if (!is_digit(argv[0]))
		goto fail;

	if (argc == 1) {
		where = atoi(argv[0]);
		wr = READ_MEM;
	} else if (argc == 2) {
		where = atoi(argv[0]);
		width = atoi(argv[1]);
		if (width != 8 && width != 64 && width != 32)
			goto fail;
		wr = READ_MEM;
	} else if (argc == 3) {
		where = atoi(argv[0]);
		width = atoi(argv[1]);
		if (width != 8 && width != 64 && width != 32)
			goto fail;
		wr = WRITE_MEM;
		if (!is_digit(argv[2]))
			goto fail;
		value = atoi(argv[2]);
	} else
		goto fail;

	wr_address(where, width, wr, value);

	return 0;

fail:
	print("unsupported params...\n");
	return -1;
}

static const struct command cmd_devmm = {
	.cmd = "devmem",
	.handler = cmd_devmm_handler,
	.priv = NULL,
};

int cmd_devmm_init()
{
	register_command(&cmd_devmm);

	return 0;
}

APP_COMMAND_REGISTER(devmm, cmd_devmm_init);
