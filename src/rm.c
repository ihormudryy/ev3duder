#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ev3_io.h"

#include "defs.h"
#include "systemcmd.h"
#include "error.h"
#include "funcs.h"

int rm(const char *path)
{
    int res;
    size_t path_sz = strlen(path) + 1;
    DELETE_FILE *rm_cmd = packet_alloc(DELETE_FILE, path_sz);
    memcpy(rm_cmd->path, path, path_sz);

    res = ev3_write(handle, (u8 *)rm_cmd, rm_cmd->packetLen + PREFIX_SIZE);
    if (res < 0)
    {
        errmsg = "Unable to write DELETE_FILE.";
        hiderr = ev3_error(handle);
        return ERR_HID;
    }
    fputs("Checking reply: \n", stderr);
    CREATE_DIR_REPLY rmrep;

    res = ev3_read_timeout(handle, (u8 *)&rmrep, sizeof rmrep, TIMEOUT);
    if (res <= 0)
    {
        errmsg = "Unable to read DELETE_FILEhiderr";
        hiderr = ev3_error(handle);
        return ERR_HID;
    }

    if (rmrep.type == VM_ERROR)
    {
        if (rmrep.ret < ARRAY_SIZE(ev3_error_msgs))
            hiderr = ev3_error_msgs[rmrep.ret];
        else
            hiderr = L"ERROR_OUT_OF_BOUNDS";
        fputs("Operation failed.\nlast_reply=", stderr);
        print_bytes(&rmrep, rmrep.packetLen);


        errmsg = "`DELETE_FILE` was denied.";
        return ERR_VM;
    }
    errmsg = "`DELETE_FILE` was successful.";
    return ERR_UNK;

}

