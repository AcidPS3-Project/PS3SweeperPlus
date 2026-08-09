#include "save.h"
#include "importer.h"
#include "core.h"

static int saveMode;

static sys_ppu_thread_t save_thread = 0;
volatile int save_running = 0;
volatile int save_result = 0;

static sys_ppu_thread_t load_thread = 0;
volatile int load_running = 0;
volatile int load_result = 0;

void savestate_init()
{
	memset(&buffer, 0, sizeof(buffer));
	buffer.maxDirectories = 1;
	buffer.maxFiles = 1;
	buffer.bufferSize = 1024 * 1024;
	buffer.buffer = malloc(buffer.bufferSize);
	printf("buffer = %p\n", buffer.buffer);
}

void status_cb(sysSaveCallbackResult *result, sysSaveStatusIn *in, sysSaveStatusOut *out)
{
    static sysSaveSystemFileParam param;
	splash = splashes_random[rand() % SplashesCount()];
    memset(&param, 0, sizeof(param));

	snprintf(param.title, sizeof(param.title), "%s", global_name);
    snprintf(param.subtitle, sizeof(param.subtitle), "Game Save Data (%s)", global_version);
    snprintf(param.detail, sizeof(param.detail), "%s\nTotal Minecoins: %d", splash, global_minecoins);
    if(out) out->setParam = &param;

    result->result = SYS_SAVE_CALLBACK_RESULT_CONTINUE;
}

void save_file_cb(sysSaveCallbackResult *result, sysSaveFileIn *in, sysSaveFileOut *out)
{
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

			result->incrementProgress = 100;
            saveMode = SAVE_ICON;
            result->result = SYS_SAVE_CALLBACK_RESULT_DONE;
            break;
		}
    }
}

void load_file_cb(sysSaveCallbackResult *result, sysSaveFileIn *in, sysSaveFileOut *out)
{
	static char filename[] = "SAVE.DAT";
	
	out->fileOperation = SYS_SAVE_FILE_OPERATION_READ;
	out->fileType      = SYS_SAVE_FILETYPE_STANDARD_FILE;
	out->filename      = filename;
	out->buffer     = &gSave;
	out->bufferSize = sizeof(gSave);
	out->size       = sizeof(gSave);

	result->incrementProgress = 100;
	result->result = SYS_SAVE_CALLBACK_RESULT_DONE;
}

static void SaveThread(void *arg)
{
	s32 ret;
	
	ret = sysSaveAutoSave2(SYS_SAVE_CURRENT_VERSION,SAVE_DIR,
		SYS_SAVE_ERROR_DIALOG_SHOW,&buffer,status_cb,save_file_cb,
		0,NULL);

    save_result = ret;
    save_running = 0;
    save_thread = 0;

    printf("Save finished: %d\n", ret);

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
	gSave.save_fp_w = fp_width;
	gSave.save_fp_h = fp_height;
	gSave.save_fp_m = fp_mines;
	gSave.save_fp_mus = fp_music;
	gSave.save_fp_bg = fp_back;

    save_running = 1;

    s32 ret = sysThreadCreate(&save_thread, SaveThread, NULL, 1500, 0x4000, 0, "SaveThread");

    if(ret != 0)
    {
        printf("SAVE: thread creation failed (%08X)\n", ret);
        save_running = 0;
        save_thread = 0;
        return ret;
    }

    return 0;
}

static void LoadThread(void *arg)
{
    s32 ret;

    printf("LOAD: starting...\n");

    ret = sysSaveAutoLoad2(SYS_SAVE_CURRENT_VERSION,SAVE_DIR, SYS_SAVE_ERROR_DIALOG_SHOW,
		&buffer,status_cb, load_file_cb,0,NULL);

    load_result = ret;
    load_running = 0;
    load_thread = 0;
	
	if(ret>=0) game_init = 1;

    printf("LOAD: finished (%08X)\n", ret);

    sysThreadExit(0);
}

int StartLoad(void)
{
    if(load_running) return -1;

    load_running = 1;

    s32 ret = sysThreadCreate(&load_thread, LoadThread, NULL, 1500, 0x4000, 0, "LoadThread");

    if(ret != 0)
    {
        printf("LOAD: thread creation failed (%08X)\n", ret);

        load_running = 0;
        load_thread = 0;

        return ret;
    }

    return 0;
}