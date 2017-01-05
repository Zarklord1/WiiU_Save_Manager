/****************************************************************************
 * Copyright (C) 2015 Dimok
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#include <coreinit/core.h>
#include <coreinit/foreground.h>
#include <proc_ui/procui.h>
#include <sysapp/launch.h>
#include "Application.h"
#include "common/types.h"
#include "logging/logger.h"
#include "test/Test.hpp"

Application *Application::applicationInstance = NULL;
bool Application::exitApplication = false;
bool Application::quitRequest = false;

Application::Application()
{
    ProcUIInit(OSSavesDone_ReadyToRelease);
}

Application::~Application()
{
    ProcUIShutdown();

    if(quitRequest)
    {
        SYSRelaunchTitle(0, 0);
    }
}

bool Application::procUI(void)
{
    bool executeProcess = false;

    switch(ProcUIProcessMessages(true))
    {
    case PROCUI_STATUS_EXITING:
    {
        log_printf("PROCUI_STATUS_EXITING\n");
        exitApplication = true;
        break;
    }
    case PROCUI_STATUS_RELEASE_FOREGROUND:
    {
        log_printf("PROCUI_STATUS_RELEASE_FOREGROUND\n");
		ProcUiHomeMenuUse();
		ProcUIDrawDoneRelease();
        break;
    }
    case PROCUI_STATUS_IN_FOREGROUND:
    {
        if(!quitRequest)
        {
            executeProcess = true;
			ProcUiHomeMenuReturn();
        }
        break;
    }
    case PROCUI_STATUS_IN_BACKGROUND:
    default:
        break;
    }

    return executeProcess;
}

int Application::exec()
{
	//write your code here
	ZipTests();
	ScreenTests();
	while(!exitApplication) {
		if(procUI() == false)
	        continue;
		//put looping code here
	}
	return exitCode;
}

void Application::quit(int code)
{
    exitCode = code;
    exitApplication = true;
    quitRequest = true;
}
