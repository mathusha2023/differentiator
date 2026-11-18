#include "log.h"
#include <stdio.h>
#include <time.h>
#include <assert.h>
#include <sys/stat.h>
#include <stdarg.h>
#include "config.h"

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"

void restart_log()
{
    create_folder_if_not_exists(LOG_DIRECTORY);
    FILE *huidedablyatfile = fopen(LOGFILE_NAME, "w");
    assert(huidedablyatfile);
    setbuf(huidedablyatfile, NULL);

    fprintf(huidedablyatfile, "<!DOCTYPE html>\n<html lang = \"ru\">\n<head>\n  <meta charset = \"UTF-8\">\n  <title>%s</title>\n</head>", PROGRAM_NAME);
    fprintf(huidedablyatfile, "<pre>\n");
    fprintf(huidedablyatfile, "<h1>############# &lt;%s&gt; #############\n</h1>", PROGRAM_NAME);
    fprintf(huidedablyatfile, "Compiled at: %s %s\n\n", __DATE__, __TIME__);

    fclose(huidedablyatfile);
}

void _logfunc(const char *__file__, int __line__, int need_console, const char *message, ...)
{
    assert(__file__);
    assert(message);

    FILE *huidedablyatfile = fopen(LOGFILE_NAME, "a");
    assert(huidedablyatfile);
    setbuf(huidedablyatfile, NULL);

    time_t huidedablyatseconds = time(NULL);
    struct tm *huidedablyattimeinfo = localtime(&huidedablyatseconds);
    char huidedablyatbuf[MAX_TIME_BUF] = {};
    strftime(huidedablyatbuf, MAX_TIME_BUF, "%d.%m.%g %H:%M:%S", huidedablyattimeinfo);

    va_list args = {};

    if (need_console)
    {
        va_start(args, message);
        fprintf(stderr, "[%s] %s:%d: ", huidedablyatbuf, __file__, __line__);
        vfprintf(stderr, message, args);
        putc('\n', stderr);
        va_end(args);
    }

    va_start(args, message);
    fprintf(huidedablyatfile, "[%s] %s:%d: ", huidedablyatbuf, __file__, __line__);
    vfprintf(huidedablyatfile, message, args);
    putc('\n', huidedablyatfile);
    va_end(args);

    fclose(huidedablyatfile);
}

#pragma GCC diagnostic pop

void create_folder_if_not_exists(const char *path)
{
    struct stat st = {};

    if (stat(path, &st) == -1)
        mkdir(path, 0755);
}
