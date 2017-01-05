#include <iosuhax.h>
#include <iosuhax_devoptab.h>
#include "Application.h"
#include "logging/logger.h"
#include "utils/utils.h"
#include "test/Test.hpp"

/* Entry point */
extern "C" int Menu_Main(void)
{
    //!*******************************************************************
    //!                   Initialize function pointers                   *
    //!*******************************************************************
    //! do OS (for acquire) and sockets first so we got logging
    log_init("192.168.1.32");

    log_printf("Welcome to the File Utils Test\n");

    int res = IOSUHAX_Open(NULL);
    if(res < 0)
    {
		//coldboot hax support
		//sres = IOSUHAX_Open("/dev/mcp");
		if(res < 0)
		{
			log_printf("IOSUHAX_open failed\n");
		}
    }
	log_printf("It Got Here!\n");
    int fsaFd = IOSUHAX_FSA_Open();
    if(fsaFd < 0)
    {
        log_printf("IOSUHAX_FSA_Open failed\n");
    }
	
	log_printf("Mount SD partition\n");
    mount_fs("sd", fsaFd, "/dev/sdcard01", "/vol/storage_sdcard");
	log_printf("Mount MLC partition\n");
    mount_fs("storage_mlc", fsaFd, NULL, "/vol/storage_mlc01");
	log_printf("Mount USB partition\n");
    mount_fs("storage_usb", fsaFd, NULL, "/vol/storage_usb01");
    //!*******************************************************************
    //!                    Enter main application                        *
    //!*******************************************************************
    log_printf("Start main application\n");
    int returnCode = Application::instance()->exec();
    log_printf("Main application stopped\n");
	
    unmount_fs("sd");
    unmount_fs("storage_mlc");
    unmount_fs("storage_usb");
    IOSUHAX_FSA_Close(fsaFd);
    IOSUHAX_Close();
	
    Application::destroyInstance();

    log_printf("File Utils Test exit\n");
    log_deinit();

    return returnCode;
}

