#include "Test.hpp"
#include "common/types.h"
#include "file_utils/Zip.hpp"
#include "logging/logger.h"
#include <string.h>
#include <coreinit/screen.h>
#include <malloc.h>
#include <coreinit/time.h>

static bool returnScreen = false;

void ZipTests() {
	UnZip unzipFile = UnZip("sd:/test/test.zip");
	unzipFile.ExtractDir("unique","sd:/test/extracted/80000002");
	unzipFile.ExtractDir("common","sd:/test/extracted/common");
}

void ScreenTests() {
	//init screens
	OSScreenInit();
	//get buffers
	u32 TVBufferSize = OSScreenGetBufferSizeEx(SCREEN_TV);
	//set buffer loc in mem
	//tv
	OSScreenSetBufferEx(SCREEN_TV, (void*)0xF4000000);
	//drc
	OSScreenSetBufferEx(SCREEN_DRC, (void*)(0xF4000000 + TVBufferSize));
	//enable screens
	OSScreenEnableEx(SCREEN_TV, 1);
	OSScreenEnableEx(SCREEN_DRC, 1);
	//Clear each buffer (RGBA colour)
	OSScreenClearBufferEx(SCREEN_TV, 0x00000000);
	OSScreenClearBufferEx(SCREEN_DRC, 0x00000000);
	
	//Print text to TV (buffer 0)
	//Remember: PutFontEx is (bufferNum, x, y, text).
	//X is in characters (monospace font)
	//Y is in lines (small margin between characters)
	//Nintendo just had to be confusing, but this system is actually REALLY helpful once you're used to it.
	OSScreenPutFontEx(SCREEN_TV, 0, 0, "Hello World! This is the TV.");
	OSScreenPutFontEx(SCREEN_TV, 0, 1, "Press HOME on the DRC to quit back to HBL.");

	//Print text to DRC (buffer 1)
	OSScreenPutFontEx(SCREEN_DRC, 0, 0, "Hello World! This is the DRC.");
	OSScreenPutFontEx(SCREEN_DRC, 0, 1, "Press HOME on the DRC to quit back to HBL.");
	
	OSScreenFlipBuffersEx(SCREEN_TV);
	OSScreenFlipBuffersEx(SCREEN_DRC);
}
void ProcUiHomeMenuUse() {
	OSScreenFlipBuffersEx(SCREEN_TV);
	OSScreenFlipBuffersEx(SCREEN_DRC);
	returnScreen = true;
}
void ProcUiHomeMenuReturn() {
	if(returnScreen) {
		OSScreenFlipBuffersEx(SCREEN_TV);
		OSScreenFlipBuffersEx(SCREEN_DRC);
		returnScreen = false;
	}
}