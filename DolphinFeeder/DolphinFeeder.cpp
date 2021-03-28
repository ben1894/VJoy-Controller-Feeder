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
			"[5] = Exit\n"
			"Selection:";
		int option = 0;
		int settingsOption = 0;
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

			break;
		case 4:
			break;
		case 5:
			break;
		case 6:
			std::cout << "Select Operation to Preform:\n";
			std::cout <<
				"[1] = Rename Class\n"
				"[2] = Switch Class Order\n"
				"[3] = Reset Class Homework\n"
				"[4] = Reset All Homework\n"
				"[5] = Reset All Data\n\n"
				"Selection:";
			if (!cinNumber(settingsOption, 5))
			{
				settingsOption = -1;
			}
			system("CLS");
			switch (settingsOption)
			{
			case 1:
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				break;
			case 5:
				break;
			default:
				break;
			}

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
		}

		WORD VerDll, VerDrv;
		if (!DriverMatch(&VerDll, &VerDrv))
			printf("Error - vJoy Driver (version %04x) does not match vJoyInterface DLL(version % 04x)\n\n", VerDrv, VerDll);
		else
			printf("OK - vJoy Driver and vJoyInterface DLL match vJoyInterface DLL (version % 04x)\n\n", VerDrv);

		char answer = 'y';
		if (!VJoyEnabled || !driverVerified)
		{
			std::cout << "Retry? [y/n]: ";
			std::cin >> answer;
		}
		if (answer == 'n')
		{
			return 0;
		}
		std::cout << "\n";
	}
	return true;
}

int editController()
{
	return -1;
}

int addController()
{	
	Controller temp;
	int configureState = 0;
	bool breakInterfaceConfig = false;
	while(!breakInterfaceConfig)
	{
		int interfaceEntered;
		while(!cinNumber(interfaceEntered, 11))
		{
			std::cout << "Please enter an interface 1-12";
		}
		configureState = temp.configureInterface(interfaceEntered);

		switch (configureState)
		{
			case 1:
				breakInterfaceConfig = true;
				break;
			default:
				std::string option;
				std::cout << "Retry? [y/n]: ";
				while (!cinString(option, false))
				{}
				if (option == "n" || option == "no")
				{
					return -1;
				}
		}
	}

	bool breakISerialConfig = false;
	while (!breakISerialConfig)
	{
		int serialEntered;
		while (!cinNumber(serialEntered, 256))
		{
			std::cout << "Please enter an COM port 0-256";
		}

		if (temp.configureInterface(serialEntered) == 0)
		{
			std::string option;
			std::cout << "Retry? [y/n]: ";
			while (!cinString(option, false))
			{}
			if (option == "n" || option == "no")
			{
				return -2;
			}
		}
		else
		{
			breakISerialConfig = true;
		}
	}

	controllers.push_back(temp);

	return 1;
	
	//-1 try again with different iinterface
	//-2 delete controller and return

}

int continueRunning()
{
	HANDLE rhnd = GetStdHandle(STD_INPUT_HANDLE);  // handle to read console
	DWORD Events = 0;     // Event count
	DWORD EventsRead = 0; // Events read from console

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

void removeController(int number)
{
	controllers.erase(controllers.begin() + number);
}

void exit()
{

}

void clearScreen()
{
	std::cout << "\033[2J\033[1;1H";
}