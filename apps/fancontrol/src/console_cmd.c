
#include "app.h"

TRACE_TAG(console_cmd);

static void console_cmd_usage(console_t *con, const char *name, int argc, char *argv[])
{
   console_printf(con, "Console commands:\r\n");
   console_printf(con, "-----------------\r\n");
   console_printf(con, "   help\r\n");
   console_printf(con, "   uptime\r\n");
   console_printf(con, "   pwm [value]\r\n");
}

static void console_cmd_uptime(console_t *con, const char *name, int argc, char *argv[])
{
   console_printf(con, "%d\r\n", hal_time_ms());
}

static void console_cmd_fanpwm(console_t *con, const char *name, int argc, char *argv[])
{
    int pwm;

    if (argc != 2) {
        console_error(con, "Bad number of args. You have to enter pwm_pct value as param");
        return;
    }

    pwm = atoi(argv[1]);

    if (fanpwm_set(pwm) == 0) {
        console_printf(con, "OK fanpwm %d\r\n", pwm);
    } else {
        console_error(con, "Set pwm failed");
    }
}

/** Console commands definition */
const console_cmd_t console_cmds[] =
{
    {"", console_cmd_usage},
    {"?", console_cmd_usage},
    {"help", console_cmd_usage},
    {"uptime", console_cmd_uptime},
    {"pwm", console_cmd_fanpwm},
    {NULL, NULL}
};
