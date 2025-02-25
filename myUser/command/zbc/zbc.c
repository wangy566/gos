#include "command.h"
#include <print.h>
#include "type.h"

int clmul_test(void);
int clmulh_test(void);
int clmulr_test(void);

rtests tests_zbc[] = {
	{"clmul test",clmul_test},
	{"clmulh test",clmulh_test},
	{"clmulr test",clmulr_test},
};
static int test_zbc(void)
{
	for(int i=0;; i++){
		if(!tests_zbc[i].name)
			break;
		if(!tests_zbc[i].fp()){
			printf("%s pass \n", tests_zbc[i].name);
		}else{
			printf("ERROR: %s fail \n", tests_zbc[i].name);
		}
	}
	return 0;
}
static int cmd_zbc_handler(int argc, char *argv[], void *priv)
{
	printf("zbc testing ......\n");
	test_zbc();
	printf("zbc test...... end\n");
	return 0;
}

static const struct command cmd_zbc_test = {
	.cmd = "zbc_test",
	.handler = cmd_zbc_handler,
	.priv = NULL,
};

int user_cmd_zbc_test_init()
{
	register_command(&cmd_zbc_test);

	return 0;
}

APP_COMMAND_REGISTER(zbc_test, user_cmd_zbc_test_init);
