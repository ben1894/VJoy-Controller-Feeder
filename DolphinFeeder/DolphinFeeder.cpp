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
#include <stdio.h>

#include <algorithm>
#include <fstream>
#include <string>

enum STATES
{
	RUNNING,
	MENU,
	EXIT
};

HANDLE hStdin;
DWORD fdwSaveOldMode;

const byte key = 255; 
const int chunkSize = 5;

STATES currentState = MENU;

bool initialVerification();
void menu();
void clearScreen();

int main()
{
	//If initial verification does not go well, exit
	if (!initialVerification())
	{
		return -1;
	}

	std::vector<byte> receivedData;

	if (currentState == MENU)
	{
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
		//Print Menu
		//Wait for user input
		//Bring Up Menu
			//Add Controller
			//Start Running / Continue Running
			//Continue Running
			//Remove Controller
			//Edit Controller
				//Change COM Port
				//Change Name
	}
	else if (currentState == RUNNING)
	{
		//Clear screen
		//Say press space to pause and bring up menu
	}
	else if (currentState == EXIT)
	{
		return 1;
	}



	HANDLE rhnd = GetStdHandle(STD_INPUT_HANDLE);  // handle to read console
	DWORD Events = 0;     // Event count
	DWORD EventsRead = 0; // Events read from console

	menu();

	int iInterface = 1;
	VjdStat status = GetVJDStatus(iInterface);

	switch (status)
	{
	case VJD_STAT_OWN:
		_tprintf(_T("vJoy Device %d is already owned by this feeder\n"), iInterface);
		break;
	case VJD_STAT_FREE:
		_tprintf(_T("vJoy Device %d is free\n"), iInterface);
		break;
	case VJD_STAT_BUSY:
		_tprintf(_T("vJoy Device %d is already owned by another feeder\nCannot continue\n"), iInterface);
		return -3;
	case VJD_STAT_MISS:
		_tprintf(_T("vJoy Device %d is not installed or disabled\nCannot continue\n"), iInterface);
		return -4;
	default:
		_tprintf(_T("vJoy Device %d general error\nCannot continue\n"), iInterface);
	}

	// Acquire the target
	if((status == VJD_STAT_OWN) || ((status == VJD_STAT_FREE) && (!
		AcquireVJD(iInterface))))
	{
		_tprintf(_T("Failed to acquire vJoy device number %d.\n"), iInterface);
		return -1;
	}
	else
	{
		_tprintf(_T("Acquired: vJoy device number %d.\n"), iInterface);
	}




	int nButtons = GetVJDButtonNumber(iInterface);
	CSerial test;
	while (test.Open(4, 9600) == 0)
	{
		Sleep(500);
		std::cout << "oof";
	}
	
	std::vector<byte> receivedData;

	//Declare here to prevent constant initialization
	JOYSTICK_POSITION_V2 iReport;
	iReport.bDevice = iInterface;

	while(1)
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
					return;
				}

			} // end EventsRead loop
			delete[] eventBuffer;
		}

		//check if data is waiting to be read
		int size = test.ReadDataWaiting();
		if(size)
		{
			//store data to array
			byte* lpBuffer = new byte[size];
			int nBytesRead = test.ReadData(lpBuffer, size);

			//add data to full vector
			receivedData.insert(receivedData.end(), &lpBuffer[0], &lpBuffer[size]);
			
			delete[]lpBuffer;
		}

		//search vector for first instance of key
		bool found = false;
		int position = 0;
		for(unsigned int i = 0; i < receivedData.size(); i++)
		{
			if(receivedData[i] == key)
			{
				found = true;
				position = i;
				break;
			}
		}

		//if the 255 exist in the vector
		//and all the bytes have been recieved (accounts for bytes already in buffer)
		if(found && ((receivedData.size() - position) > chunkSize))
		{			
			/*for(unsigned int i = 0; i < receivedData.size(); i++)
			{
				std::cout << (int)receivedData[i] << ",";
			}
			//std::cout << "\n";
			//check if other all bytes have been received*/

			//255, buttons, x100, x10, y100, y10
			//iReport.lButtons |= (long)receivedData[position + 1]; //bit operations will be needed for more than 8 buttons
			iReport.lButtons = receivedData[position + 1]; 
			iReport.wAxisX = (receivedData[position + 2]*100) + receivedData[position + 3];
			iReport.wAxisY =  (receivedData[position + 4]*100) + receivedData[position + 5];

			//erase values that were just read
			receivedData.erase(receivedData.begin(), receivedData.begin() + position + chunkSize);
			
			//update the dll
			UpdateVJD(iInterface, &iReport);
		}

		//Delay to not use all the CPU checking for values that aren't sent yet
		Sleep(5);
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
}

void menu()
{

}

void setUpController()
{

}

void continueRunning()
{
	
}

void exit()
{

}

void clearScreen()
{
	std::cout << "\033[2J\033[1;1H";
}