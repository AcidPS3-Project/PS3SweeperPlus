#ifndef SAVE_H
#define SAVE_H

#include "libraries.h"

#define SAVE_DIR "PS3SWEEPERPLUS"
typedef struct
{
	int save_level;
	int save_global_minecoins;
	int save_current_minecoins;
	int save_global_route;
} SaveData;

SaveData gSave;
sysSaveBufferSettings buffer;

enum
{
    SAVE_ICON,
    SAVE_DATA,
    SAVE_DONE
};

void savestate_init();
void status_cb(sysSaveCallbackResult *result, sysSaveStatusIn *in, sysSaveStatusOut *out);
void save_file_cb(sysSaveCallbackResult *result, sysSaveFileIn *in, sysSaveFileOut *out);
void load_file_cb(sysSaveCallbackResult *result, sysSaveFileIn *in, sysSaveFileOut *out);

#endif