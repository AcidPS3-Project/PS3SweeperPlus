#include "save.h"
#include "importer.h"
#include "core.h"

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
}

void savestate_init()
{
	memset(&buffer, 0, sizeof(buffer));
	buffer.maxDirectories = 1;
	buffer.maxFiles = 2;
	buffer.bufferSize = 1024 * 1024;
	buffer.buffer = malloc(buffer.bufferSize);
	
	char debug1[256];
	sprintf(debug1,"buffer = %p",buffer.buffer);
    DebugLog(debug1);
}

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
                    if (in->fileList[i].fileSize > sizeof(gSave))
                    {
                        DebugLog("LOAD: SAVE.DAT has wrong size");
                        result->result = SYS_SAVE_CALLBACK_RESULT_CORRUPTED;
                        return;
                    }

                    DebugLog("LOAD: SAVE.DAT found");
                    break;
                }
            }

            if (!found)
            {
                DebugLog("LOAD: SAVE.DAT not found");
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
            out->buffer     = (void *)icon0_png_bin;
            out->bufferSize = icon0_png_bin_size;
            out->size       = icon0_png_bin_size;

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
	DebugLog("LOAD FILE CALLBACK ENTER");
    static char filename[] = "SAVE.DAT";

    memset(out, 0, sizeof(*out));
	DebugLog("LOAD CALLBACK: requesting SAVE.DAT");

	out->fileOperation = SYS_SAVE_FILE_OPERATION_READ;
	out->fileType = SYS_SAVE_FILETYPE_STANDARD_FILE;
	out->filename = filename;
	out->buffer = &gSave;
	out->bufferSize = sizeof(gSave);
	out->size = sizeof(gSave);

	result->incrementProgress = 100;
	result->result = SYS_SAVE_CALLBACK_RESULT_DONE;
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
	sprintf(debug, "SAVE: level=%d current=%d global=%d route=%d episode=%d", gSave.save_level,
		gSave.save_current_minecoins, gSave.save_global_minecoins, gSave.save_global_route, gSave.save_episode);
	DebugLog(debug);

	sprintf(debug,"Save finished: %d",ret);
    DebugLog(debug);

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
	
	char debug[256];

	sprintf(debug,"SAVING VALUES: level=%d current=%d global=%d route=%d", gSave.save_level,
		gSave.save_current_minecoins, gSave.save_global_minecoins, gSave.save_global_route);
	DebugLog(debug);
	
	saveMode = SAVE_ICON;
	operationMode = MODE_SAVE;
	
    save_running = 1;

    s32 ret = sysThreadCreate(&save_thread, SaveThread, NULL, 1500, 0x4000, 0, "SaveThread");

    if(ret != 0)
    {
		sprintf(debug, "SAVE: thread creation failed (%08X)" ,ret);
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
    DebugLog("LOAD: starting...");

    ret = sysSaveAutoLoad2(SYS_SAVE_CURRENT_VERSION, SAVE_DIR, SYS_SAVE_ERROR_DIALOG_SHOW,
		&buffer, status_cb, load_file_cb, 0, NULL);

    load_result = ret;
    load_running = 0;
    load_thread = 0;
	
	char debug1[256];
	sprintf(debug1,"LOAD: finished (%08X)",ret);
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
	
	DebugLog("loaded!");
	
	if(EPISODE<1) EPISODE=1;
	if(fp_width<1) fp_width=9;
	if(fp_height<1) fp_height=9;
	if(fp_mines<1) fp_mines=10;
	if(fp_music<1) fp_music=1;
	game_init = 1;
	
	char debug[256];
	sprintf(debug, "LOAD VALUES: level=%d current=%d global=%d route=%d episode=%d", gSave.save_level,
		gSave.save_current_minecoins, gSave.save_global_minecoins, gSave.save_global_route, gSave.save_episode);
	DebugLog(debug);

    sysThreadExit(0);
}

int StartLoad(void)
{
    if(load_running) return -1;
	
	operationMode = MODE_LOAD;
	
    load_running = 1;

    s32 ret = sysThreadCreate(&load_thread, LoadThread, NULL, 1500, 0x4000, 0, "LoadThread");

    if(ret != 0)
    {
		char debug1[256];
		sprintf(debug1,"LOAD: thread creation failed (%08X)",ret);
		DebugLog(debug1);

        load_running = 0;
        load_thread = 0;

        return ret;
    }

    return 0;
}