#ifndef SAVE_H
#define SAVE_H

#include "libraries.h"

#define SAVE_DIR "PS3SWEEPERPLUS"
#define MAXLVLTEST 20
float level_time[MAXLVLTEST];

typedef struct
{
	// my idiotic mistake i did earlier and such.
	// whatever you do, please dont change the order!
	// adding to it is fine as long as the order doesnt change :)
	int save_level;
	int save_global_minecoins;
	int save_current_minecoins;
	int save_global_route;
	int save_episode;
	int save_difficulty;
	
	int save_fp_w;
	int save_fp_h;
	int save_fp_m;
	int save_fp_mus;
	int save_fp_bg;
	
	int save_lang;
	int save_setting_guarantee;
	
	int save_hat;
	
	bool save_shop_unlocked;
	bool save_first_playing;
	
	int save_lifes;
	float save_level_time[MAXLVLTEST];
} SaveData;

extern SaveData gSave;
extern sysSaveBufferSettings buffer;

void SetSaveDefaults();

extern volatile int save_running;

enum
{
    SAVE_ICON,
    SAVE_DATA,
    SAVE_DONE
};

enum
{
    MODE_SAVE,
    MODE_LOAD
};

void savestate_init();
void status_cb(sysSaveCallbackResult *result, sysSaveStatusIn *in, sysSaveStatusOut *out);
void save_file_cb(sysSaveCallbackResult *result, sysSaveFileIn *in, sysSaveFileOut *out);
void load_file_cb(sysSaveCallbackResult *result, sysSaveFileIn *in, sysSaveFileOut *out);

int StartSave(void);
int StartLoad(void);

#endif