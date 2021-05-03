// DolphinFeeder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#define NOMINMAX
#include <Windows.h>
#include <iostream>
#include <vector>
#include "tchar.h"
#include "Serial.h"
#include "public.h"
#include "vjoyinterface.h"
#include "easyinput.h"
#include "Controller.h"
#include <stdio.h>

#include <algorithm>
#include <fstream>
#include <string>

//class Controller;

enum STATES
{
	RUNNING,
	MENU,
	EXIT
};

HANDLE hStdin;
DWORD fdwSaveOldMode;

STATES currentState = MENU;
std::vector<Controller> controllers;

int main();

bool initialVerification();
void clearScreen();
int editController();
int addController();
int run();
int removeController();
int changeComPort();


int main()
{
	//If initial verification does not go well, exit
	if (!initialVerification())
	{
		return -1;
	}


	while (1)
	{
		std::cout << "Select Operation to Preform:\n";
		std::cout <<
			"[1] = Add Controller\n"
			"[2] = Edit Controller\n"
			"[3] = Remove Controller\n"
			"[4] = Run\n"
			"[5] = Exit\n\n"
			"Selection: ";

		int option = 0;
		if (!cinNumber(option, 5))
		{
			option = -1;
		}

		clearScreen();
		switch (option)
		{
		case 1:
			addController();
			break;
		case 2:
			editController();
			break;
		case 3:
			removeController();
			break;
		case 4:
			run();
			break;
		case 5:
			//Exit from the program
			return 1;
			break;
		default:
			break;
		}
		system("CLS");
	}
}

bool initialVerification()
{
	//verify info
	//state user input to retry or exit
	std::cout << "Checking if VJoy is running and driver is correct...\n\n";
	bool VJoyEnabled = false;
	bool driverVerified = false;

	while (!VJoyEnabled && !driverVerified)
	{
		if (!vJoyEnabled())
		{
			std::cout << "Error - Please make sure VJoy is running.\n";
		}
		else
		{
			std::cout << "OK - VJoy is running.\n";
			VJoyEnabled = true;
		}

		WORD VerDll, VerDrv;
		if (!DriverMatch(&VerDll, &VerDrv))
			printf("Error - vJoy Driver (version %04x) does not match vJoyInterface DLL(version % 04x)\n\n", VerDrv, VerDll);
		else
		{
			printf("OK - vJoy Driver and vJoyInterface DLL match vJoyInterface DLL (version % 04x)\n\n", VerDrv);
			driverVerified = true;
		}

		if (!VJoyEnabled || !driverVerified)
		{
			if (retry() == 0)
			{
				return 0;
			}
		}
		std::cout << "\n";
	}
	return true;
}

int editController()
{
	int option = 0;
	std::cout << "Select Operation to Preform:\n";
	std::cout <<
		"[1] = Change COM Port\n"
		"[2] = Edit Name\n\n"
		"Selection:";
	if (!cinNumber(option, 2))
	{
		option = -1;
	}
	clearScreen();
	switch (option)
	{
	case 1:
		changeComPort();
		break;
	case 2:
		break;
	default:
		break;
	}

	return -1;
}

int addController()
{	
	Controller *temp = new Controller();
	int configureState = 0;
	bool breakInterfaceConfig = false;

	while(!breakInterfaceConfig)
	{
		std::cout << "Please enter an interface number (1-16): ";
		int interfaceEntered;
		if(!cinNumber(interfaceEntered, 16))
		{
			configureState = 0;
		}
		else
		{
			configureState = temp->configureInterface(interfaceEntered);
		}

		switch (configureState)
		{
			case 1:
				breakInterfaceConfig = true;
				break;
			default:
				if (retry() == 0)
				{
					return -1;
				}
		}
	}

	bool breakISerialConfig = false;
	while (!breakISerialConfig)
	{
		std::cout << "\nPlease enter bluetooth COM port: ";
		int serialEntered;
		if(!cinNumber(serialEntered, 256))
		{
			if (retry() == 0)
			{
				return -1;
			}
		}
		else
		{
			if (temp->configureSerialPort(serialEntered) == -1)
			{
				printf("Unable to connect to COM port %d\n", serialEntered);
				if (retry() == 0)
				{
					return -1;
				}
			}
			else
			{
				breakISerialConfig = true;
			}
		}
	}

	std::cout << "\nEnter a name for this controller: ";
	cinString(temp->name, false);

	controllers.push_back(*temp);

	return 1;
	
	//-1 try again with different iinterface
	//-2 delete controller and return

}

int run()
{
	HANDLE rhnd = GetStdHandle(STD_INPUT_HANDLE);  // handle to read console
	DWORD Events = 0;     // Event count
	DWORD EventsRead = 0; // Events read from console

	if (controllers.empty())
	{
		clearScreen();
		std::cout << "Please add a controller first. \n";
		system("pause");
		return -1;
	}

	std::cout << "Feeder is running\n";
	std::cout << "Press space to pause and return to menu\n\n";

	while (1)
	{
		GetNumberOfConsoleInputEvents(rhnd, &Events);
		if (Events != 0) { // if something happened we will handle the events we want

			// create event buffer the size of how many Events
			INPUT_RECORD* eventBuffer = new INPUT_RECORD[Events];

			// fills the event buffer with the events and saves count in EventsRead
			ReadConsoleInput(rhnd, eventBuffer, Events, &EventsRead);

			// loop through the event buffer using the saved count
			for (DWORD i = 0; i < EventsRead; ++i)
			{
				// check if event[i] is a key event && if so is a press not a release && a space was pressed
				if (eventBuffer[i].EventType == KEY_EVENT &&
					eventBuffer[i].Event.KeyEvent.bKeyDown &&
					eventBuffer[i].Event.KeyEvent.wVirtualKeyCode == VK_SPACE)
				{
					return 0;
				}

			} // end EventsRead loop
			delete[] eventBuffer;
		}

		for (int i = 0; i < controllers.size(); i++)
		{
			controllers[i].updateController();
		}

		Sleep(5);
	}
}

int removeController()
{
	if (controllers.empty())
	{
		clearScreen();
		std::cout << "Please add a controller to remove first. \n";
		system("pause");
		return -1;
	}

	for (int i = 0; i < controllers.size(); i++)
	{
		std::cout << i + 1 << ". " << controllers[i].name << "\n";
	}

	int controllerToRemoveIndex;
	if (!cinNumber(controllerToRemoveIndex, controllers.size()))
	{
		return -1;
	}

	controllers.erase(controllers.begin() + (controllerToRemoveIndex - 1));

	return 1;
}

int setComPort(Controller *controller)
{

}

int changeComPort()
{
	if (controllers.empty())
	{
		clearScreen();
		std::cout << "Please add a controller to remove first. \n";
		system("pause");
		return -1;
	}

	for (int i = 0; i < controllers.size(); i++)
	{
		std::cout << i + 1 << ". " << controllers[i].name << "\n";
	}

	int controllerToEdit;
	if (!cinNumber(controllerToEdit, controllers.size()))
	{
		return -1;
	}

	controllers[controllerToEdit - 1].configureSerialPort();

	return 1;
}

void clearScreen()
{
	std::cout << "\033[2J\033[1;1H";
}