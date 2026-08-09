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
	int save_episode;
	int save_fp_w;
	int save_fp_h;
	int save_fp_m;
	int save_fp_mus;
	int save_fp_bg;
} SaveData;

SaveData gSave;
sysSaveBufferSettings buffer;

extern volatile int save_running;

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

int StartSave(void);
int StartLoad(void);

#endif