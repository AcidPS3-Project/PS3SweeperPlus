#include "save.h"
#include "importer.h"

static int saveMode;
volatile int save_running = 0;
sys_ppu_thread_t save_thread;

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