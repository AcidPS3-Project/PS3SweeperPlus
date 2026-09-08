#ifndef SAVE_H
#define SAVE_H

#include <sysutil/save.h>

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
	int save_mistakes;
	
	bool save_shop_unlock_0; // navy's hat
	bool save_shop_unlock_1; // military hat
	bool save_shop_unlock_2; // golden hat
	bool save_shop_unlock_3; // bald
	bool save_shop_unlock_4; // koishi's hat
	bool save_shop_unlock_5; // alien captain's hat
	
    int sranks_ep_1[8];
    int sranks_ep_2[8];
    int sranks_ep_3[8];
    int sranks_ep_4[8];
	
	bool route_touhou_unlocked;
	
	bool save_seen_nine;
	bool save_seen_fnine;
	bool save_seen_ten;
	bool save_seen_nten;
	bool save_seen_eleven;
	bool save_seen_meleven;
	bool save_seen_twelve;
	bool save_seen_nan;
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