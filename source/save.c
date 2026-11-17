#include <sysmodule/sysmodule.h>
#include <ppu-types.h>
#include <sys/process.h>
#include <sys/memory.h>
#include <sys/thread.h>

#include "libraries.h"
#include "save.h"
#include "importer.h"
#include "core.h"
#include "lang.h"

#include "icon0_bin.h"

SaveData gSave;
sysSaveBufferSettings buffer;

static int saveMode;
static int operationMode;

static sys_ppu_thread_t save_thread = 0;
volatile int save_running = 0;
volatile int save_result = 0;

static sys_ppu_thread_t load_thread = 0;
volatile int load_running = 0;
volatile int load_result = 0;

static struct XMBEvent {
    int drawing;
    int opened;
    int closed;
    int exit;
} xmb;

void SetSaveDefaults()
{
    memset(&gSave, 0, sizeof(gSave));

    gSave.save_level = 0;
    gSave.save_current_minecoins = 0;
    gSave.save_global_minecoins = 0;
    gSave.save_global_route = 0;
    gSave.save_episode = 1;
    gSave.save_difficulty = 1;

    gSave.save_fp_w = 9;
    gSave.save_fp_h = 9;
    gSave.save_fp_m = 10;
    gSave.save_fp_mus = 1;
    gSave.save_fp_bg = 0;

    gSave.save_lang = 0;
    gSave.save_setting_guarantee = 0;

    gSave.save_hat = 0;
	
    gSave.save_shop_unlocked = false;
    gSave.save_first_playing = true;
    gSave.save_lifes = 3;
	
	for(int _t = 0; _t < MAXLVLTEST; _t++)
	{
		gSave.save_level_time[_t] = 99999.9f;
	}
	
	gSave.save_mistakes = 0;
	
	gSave.save_shop_unlock_0 = false;
	gSave.save_shop_unlock_1 = false;
	gSave.save_shop_unlock_2 = false;
	gSave.save_shop_unlock_3 = false;
	gSave.save_shop_unlock_4 = false;
	gSave.save_shop_unlock_5 = false;
	
	for(int i = 0; i < 8; i++)
	{
		gSave.sranks_ep_1[i] = -1;
		gSave.sranks_ep_2[i] = -1;
		gSave.sranks_ep_3[i] = -1;
		gSave.sranks_ep_4[i] = -1;
	}
	
	gSave.route_touhou_unlocked = false;
	
	gSave.save_seen_nine = false;
	gSave.save_seen_fnine = false;
	gSave.save_seen_ten = false;
	gSave.save_seen_nten = false;
	gSave.save_seen_eleven = false;
	gSave.save_seen_meleven = false;
	gSave.save_seen_twelve = false;
	gSave.save_seen_nan = false;
	
	gSave.slegacy_9_time = false;

	gSave.ep2_unlock = false;
	gSave.ep3_unlock = false;
	gSave.ep4_unlock = false;
	gSave.ep5_unlock = false;
	gSave.ep6_unlock = false;
	gSave.ep7_unlock = false;
	
	gSave.save_seen_ktwelve = false;
	gSave.smaster_mode = false;
	gSave.SENSITIVITY = 4;
	gSave.saved_route = 0;
}

void savestate_init()
{
	memset(&buffer, 0, sizeof(buffer));
	buffer.maxDirectories = 1;
	buffer.maxFiles = 2;
	buffer.bufferSize = 1024 * 1024;
	buffer.buffer = malloc(buffer.bufferSize);
	
	char debug1[256];
	sprintf(debug1,"buffer = %p\n",buffer.buffer);
    DebugLog(debug1);
}

static u64 load_file_size;

void status_cb(sysSaveCallbackResult *result, sysSaveStatusIn *in, sysSaveStatusOut *out)
{
    static sysSaveSystemFileParam param;
	splash = splashes_random[rand() % SplashesCount()];
    memset(&param, 0, sizeof(param));

	snprintf(param.title, sizeof(param.title), "%s", global_name);
    snprintf(param.subtitle, sizeof(param.subtitle), "Game Save Data (%s)", global_version);
    snprintf(param.detail, sizeof(param.detail), "%s\nTotal Minecoins: %d", splash, global_minecoins);
	
    if(out)
	{
		out->setParam = &param;
		if (operationMode == MODE_SAVE) out->recreateMode = SYS_SAVE_RECREATE_MODE_DELETE;
		else if (operationMode == MODE_LOAD)
		{
            out->recreateMode = SYS_SAVE_RECREATE_MODE_OVERWRITE_NOT_CORRUPTED;

            int found = 0;
            for (int i = 0; i < in->numFiles; i++)
            {
                if (in->fileList[i].fileType == SYS_SAVE_FILETYPE_STANDARD_FILE)
                {
                    found = 1;
					load_file_size = in->fileList[i].fileSize;
                    if (load_file_size > sizeof(gSave))
                    {
                        DebugLog("LOAD: SAVE.DAT has wrong size\n");
                        result->result = SYS_SAVE_CALLBACK_RESULT_CORRUPTED;
                        return;
                    }

                    DebugLog("LOAD: SAVE.DAT found\n");
                    break;
                }
            }

            if (!found)
            {
                DebugLog("LOAD: SAVE.DAT not found\n");
                result->result = SYS_SAVE_CALLBACK_RESULT_NOT_FOUND;
                return;
            }
		}
	}

    result->result = SYS_SAVE_CALLBACK_RESULT_CONTINUE;
}

void save_file_cb(sysSaveCallbackResult *result, sysSaveFileIn *in, sysSaveFileOut *out)
{
	memset(out, 0, sizeof(*out));
	switch(saveMode)
    {
        case SAVE_ICON:
		{
            out->fileOperation = SYS_SAVE_FILE_OPERATION_WRITE;
            out->fileType      = SYS_SAVE_FILETYPE_CONTENT_ICON0;
            out->buffer     = (void *)icon0_bin;
            out->bufferSize = icon0_bin_size;
            out->size       = icon0_bin_size;

            result->result = SYS_SAVE_CALLBACK_RESULT_CONTINUE;
			result->incrementProgress = 10;
            saveMode = SAVE_DATA;
            break;
		}
        case SAVE_DATA:
		{
			static char filename[] = "SAVE.DAT";
			
            out->fileOperation = SYS_SAVE_FILE_OPERATION_WRITE;
            out->fileType      = SYS_SAVE_FILETYPE_STANDARD_FILE;
            out->filename      = filename;
            out->buffer     = &gSave;
            out->bufferSize = sizeof(gSave);
            out->size       = sizeof(gSave);

			result->incrementProgress = 90;
            result->result = SYS_SAVE_CALLBACK_RESULT_CONTINUE;
            saveMode = SAVE_DONE;
            break;
		}
        case SAVE_DONE:
        {
            result->result = SYS_SAVE_CALLBACK_RESULT_DONE;
            break;
        }
    }
}

void load_file_cb(sysSaveCallbackResult *result, sysSaveFileIn *in, sysSaveFileOut *out)
{
	DebugLog("LOAD FILE CALLBACK ENTER\n");
    static char filename[] = "SAVE.DAT";
	
	memset(out, 0, sizeof(*out));

    if(in->previousOperationResultSize == 0)
    {
        DebugLog("LOAD CALLBACK: requesting SAVE.DAT\n");

        out->fileOperation = SYS_SAVE_FILE_OPERATION_READ;
        out->fileType = SYS_SAVE_FILETYPE_STANDARD_FILE;
        out->filename = filename;

        out->buffer = &gSave;
        out->bufferSize = sizeof(gSave);
        out->size = load_file_size;

        result->incrementProgress = 100;
        result->result = SYS_SAVE_CALLBACK_RESULT_CONTINUE;
    }
    else
    {
        char debug[256];

        sprintf(debug,
            "LOAD CALLBACK: read=%d expected=%d\n",
            (int)in->previousOperationResultSize,
            (int)load_file_size);

        DebugLog(debug);

        if(in->previousOperationResultSize == load_file_size)
        {
            DebugLog("LOAD CALLBACK: SAVE.DAT read successfully\n");
            result->result = SYS_SAVE_CALLBACK_RESULT_DONE;
        }
        else
        {
            DebugLog("LOAD CALLBACK: SAVE.DAT read failed\n");
            result->result = SYS_SAVE_CALLBACK_RESULT_CORRUPTED;
        }
    }
}

static void SaveThread(void *arg)
{
	s32 ret;
	
	ret = sysSaveAutoSave2(SYS_SAVE_CURRENT_VERSION,SAVE_DIR,
		SYS_SAVE_ERROR_DIALOG_SHOW,&buffer,status_cb,save_file_cb,0,NULL);

    save_result = ret;
    save_running = 0;
    save_thread = 0;
	
	char debug[256];
	sprintf(debug, "SAVE: level=%d current=%d global=%d route=%d episode=%d\n", gSave.save_level,
		gSave.save_current_minecoins, gSave.save_global_minecoins, gSave.save_global_route, gSave.save_episode);
	DebugLog(debug);

	sprintf(debug,"Save finished: %d\n",ret);
    DebugLog(debug);
	xmb.opened = 0;
	xmb.closed = 1;
	xmb.drawing = 0;

    sysThreadExit(0);
}

int StartSave(void)
{
    if(save_running) return -1;

    gSave.save_level = LEVEL;
    gSave.save_current_minecoins = current_minecoins;
    gSave.save_global_minecoins = global_minecoins;
    gSave.save_global_route = global_route;
	gSave.save_episode = EPISODE;
	gSave.save_difficulty = DIFFICULTY;
	
	gSave.save_fp_w = fp_width;
	gSave.save_fp_h = fp_height;
	gSave.save_fp_m = fp_mines;
	gSave.save_fp_mus = fp_music;
	gSave.save_fp_bg = fp_back;
	
	gSave.save_lang = language;
	gSave.save_setting_guarantee = setting_guarantee;
	
	gSave.save_hat = the_hat;
	gSave.save_shop_unlocked = shop_unlocked;
	
	gSave.save_first_playing = first_playing;
	gSave.save_lifes = LIFES;
	
	for(int _t = 0; _t < MAXLVLTEST; _t++)
	{
		gSave.save_level_time[_t] = level_time[_t];
	}
	
	gSave.save_mistakes = mistakes;
	
	gSave.save_shop_unlock_0 = shop_unlock_0;
	gSave.save_shop_unlock_1 = shop_unlock_1;
	gSave.save_shop_unlock_2 = shop_unlock_2;
	gSave.save_shop_unlock_3 = shop_unlock_3;
	gSave.save_shop_unlock_4 = shop_unlock_4;
	gSave.save_shop_unlock_5 = shop_unlock_5;
	
	memcpy(gSave.sranks_ep_1, ranks_ep_1, sizeof(ranks_ep_1));
	memcpy(gSave.sranks_ep_2, ranks_ep_2, sizeof(ranks_ep_2));
	memcpy(gSave.sranks_ep_3, ranks_ep_3, sizeof(ranks_ep_3));
	memcpy(gSave.sranks_ep_4, ranks_ep_4, sizeof(ranks_ep_4));
	
	gSave.route_touhou_unlocked = route_touhou_unlocked;
	
	gSave.save_seen_nine = seen_nine;
	gSave.save_seen_fnine = seen_fnine;
	gSave.save_seen_ten = seen_ten;
	gSave.save_seen_nten = seen_nten;
	gSave.save_seen_eleven = seen_eleven;
	gSave.save_seen_meleven = seen_meleven;
	gSave.save_seen_twelve = seen_twelve;
	gSave.save_seen_nan = seen_nan;
	
	gSave.slegacy_9_time = legacy_9_time;

	gSave.ep2_unlock = ep2_unlock;
	gSave.ep3_unlock = ep3_unlock;
	gSave.ep4_unlock = ep4_unlock;
	gSave.ep5_unlock = ep5_unlock;
	gSave.ep6_unlock = ep6_unlock;
	gSave.ep7_unlock = ep7_unlock;
	
	gSave.save_seen_ktwelve = seen_ktwelve;
	gSave.smaster_mode = master_mode;
	gSave.SENSITIVITY = R_SENSITIVITY;
	gSave.saved_route = saved_route;

	char debug[256];

	sprintf(debug,"SAVING VALUES: level=%d current=%d global=%d route=%d\n", gSave.save_level,
		gSave.save_current_minecoins, gSave.save_global_minecoins, gSave.save_global_route);
	DebugLog(debug);
	
	saveMode = SAVE_ICON;
	operationMode = MODE_SAVE;
	
    save_running = 1;
	xmb.opened = 1;
	xmb.closed = 0;
	xmb.drawing = 1;

    s32 ret = sysThreadCreate(&save_thread, SaveThread, NULL, 1500, 0x4000, 0, "SaveThread");

    if(ret != 0)
    {
		sprintf(debug, "SAVE: thread creation failed (%08X)\n" ,ret);
		DebugLog(debug);
        save_running = 0;
        save_thread = 0;
        return ret;
    }

    return 0;
}

static void LoadThread(void *arg)
{
	SetSaveDefaults();
	
    s32 ret;
    DebugLog("LOAD: starting...\n");

    ret = sysSaveAutoLoad2(SYS_SAVE_CURRENT_VERSION, SAVE_DIR, SYS_SAVE_ERROR_DIALOG_SHOW,
		&buffer, status_cb, load_file_cb, 0, NULL);

    load_result = ret;
    load_running = 0;
    load_thread = 0;
	
	char debug1[256];
	sprintf(debug1,"LOAD: finished (%08X)\n",ret);
    DebugLog(debug1);
	
	LEVEL = gSave.save_level;
	current_minecoins = gSave.save_current_minecoins;
	global_minecoins = gSave.save_global_minecoins;
	global_route = gSave.save_global_route;
	EPISODE = gSave.save_episode;
	DIFFICULTY = gSave.save_difficulty;
	
	fp_width = gSave.save_fp_w;
	fp_height = gSave.save_fp_h;
	fp_mines = gSave.save_fp_m;
	fp_music = gSave.save_fp_mus;
	fp_back = gSave.save_fp_bg;
	
	language = gSave.save_lang;
	LangInit();
	
	setting_guarantee = gSave.save_setting_guarantee;
	
	the_hat = gSave.save_hat;
	shop_unlocked = gSave.save_shop_unlocked;
	
	first_playing = gSave.save_first_playing;
	LIFES = gSave.save_lifes;
	
	for(int _t = 0; _t < MAXLVLTEST; _t++)
	{
		level_time[_t] = gSave.save_level_time[_t];
	}
	
	mistakes = gSave.save_mistakes;
	
	shop_unlock_0 = gSave.save_shop_unlock_0;
	shop_unlock_1 = gSave.save_shop_unlock_1;
	shop_unlock_2 = gSave.save_shop_unlock_2;
	shop_unlock_3 = gSave.save_shop_unlock_3;
	shop_unlock_4 = gSave.save_shop_unlock_4;
	shop_unlock_5 = gSave.save_shop_unlock_5;
	
	memcpy(ranks_ep_1, gSave.sranks_ep_1, sizeof(ranks_ep_1));
	memcpy(ranks_ep_2, gSave.sranks_ep_2, sizeof(ranks_ep_2));
	memcpy(ranks_ep_3, gSave.sranks_ep_3, sizeof(ranks_ep_3));
	memcpy(ranks_ep_4, gSave.sranks_ep_4, sizeof(ranks_ep_4));
	
	route_touhou_unlocked = gSave.route_touhou_unlocked;
	
	seen_nine = gSave.save_seen_nine;
	seen_fnine = gSave.save_seen_fnine;
	seen_ten = gSave.save_seen_ten;
	seen_nten = gSave.save_seen_nten;
	seen_eleven = gSave.save_seen_eleven;
	seen_meleven = gSave.save_seen_meleven;
	seen_twelve = gSave.save_seen_twelve;
	seen_nan = gSave.save_seen_nan;
	
	legacy_9_time = gSave.slegacy_9_time;

	ep2_unlock = gSave.ep2_unlock;
	ep3_unlock = gSave.ep3_unlock;
	ep4_unlock = gSave.ep4_unlock;
	ep5_unlock = gSave.ep5_unlock;
	ep6_unlock = gSave.ep6_unlock;
	ep7_unlock = gSave.ep7_unlock;

	seen_ktwelve = gSave.save_seen_ktwelve;
	master_mode = gSave.smaster_mode;
	R_SENSITIVITY = gSave.SENSITIVITY;
	saved_route = gSave.saved_route;
	
	char debug[256];
	sprintf(debug, "LOAD VALUES: level=%d current=%d global=%d route=%d episode=%d\n", gSave.save_level,
		gSave.save_current_minecoins, gSave.save_global_minecoins, gSave.save_global_route, gSave.save_episode);
	DebugLog(debug);
	
	if(EPISODE<1) EPISODE=1;
	if(fp_width<1) fp_width=9;
	if(fp_height<1) fp_height=9;
	if(fp_mines<1) fp_mines=10;
	if(fp_music<1) fp_music=1;
	game_init = 1;
	
	xmb.opened = 0;
	xmb.closed = 1;
	xmb.drawing = 0;
	
    sysThreadExit(0);
}

int StartLoad(void)
{
    if(load_running) return -1;
	
	operationMode = MODE_LOAD;
	
    load_running = 1;
	xmb.opened = 1;
	xmb.closed = 0;
	xmb.drawing = 1;
    s32 ret = sysThreadCreate(&load_thread, LoadThread, NULL, 1500, 0x4000, 0, "LoadThread");

    if(ret != 0)
    {
		char debug1[256];
		sprintf(debug1,"LOAD: thread creation failed (%08X)\n",ret);
		DebugLog(debug1);

        load_running = 0;
        load_thread = 0;

        return ret;
    }

    return 0;
}