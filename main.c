#include "mainloop.h"
#include "config.h"
#include "log.h"

int main(void)
{
    restart_log();

    int status_code = mainloop(DATA_FILE_NAME, TEX_FILE_NAME);

    close_log();

    return status_code;
}
