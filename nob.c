#define NOB_IMPLEMENTATION
#define COMPILE_N_RUN
#define DEBUG
#define PREFIX "src/"

#include "nob.h"

void compiler(Nob_Cmd* cmd) {
    nob_cmd_append(cmd, "gcc");
}

void flag(Nob_Cmd* cmd) {
    nob_cmd_append(cmd, "-Wall", "-Wextra");
#ifndef DEBUG
    nob_cmd_append(cmd, "-O3", "-march=native", "-flto");
#else
    nob_cmd_append(cmd, "-ggdb");
#endif
}

void libdeps(Nob_Cmd* cmd) {
    nob_cmd_append(cmd, "-I./raylib-5.0_linux_amd64/include/");
    nob_cmd_append(cmd, "-L./raylib-5.0_linux_amd64/lib/");
    nob_cmd_append(cmd, "-l:libraylib.a", "-lm");
}

void inputf(Nob_Cmd* cmd) {
    nob_cmd_append(cmd, PREFIX"main.c");
}

void outputf(Nob_Cmd* cmd) {
    nob_cmd_append(cmd, "-o", "tictactoe");
}

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);
    Nob_Cmd cmd = {0};

    compiler(&cmd);
    inputf(&cmd);
    flag(&cmd);
    libdeps(&cmd);
    outputf(&cmd);
    
    if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;

#ifdef COMPILE_N_RUN
    cmd.count = 0;
    nob_cmd_append(&cmd, "./tictactoe");
    if (!nob_cmd_run_sync_and_reset(&cmd)) return 1;
#endif
    return 0;
}
