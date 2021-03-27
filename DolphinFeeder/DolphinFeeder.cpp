// DolphinFeeder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <Windows.h>
#include <iostream>
#include <vector>
#include "tchar.h"
#include "Serial.h"
#include "public.h"
#include "vjoyinterface.h"
#include <stdio.h>

HANDLE hStdin;
DWORD fdwSaveOldMode;

const byte key = 255; 
const int chunkSize = 5;

void menu();

int main()
{
	HANDLE rhnd = GetStdHandle(STD_INPUT_HANDLE);  // handle to read console
	DWORD Events = 0;     // Event count
	DWORD EventsRead = 0; // Events read from console

	if (!vJoyEnabled())
	{
		_tprintf(_T("Failed Getting vJoy attributes.\n"));
		Sleep(1000);
		return -2;
	}

	WORD VerDll, VerDrv;
	if (!DriverMatch(&VerDll, &VerDrv))
		_tprintf(_T("Failed\r\nvJoy Driver (version %04x) does not match vJoyInterface DLL(version % 04x)\n"), VerDrv ,VerDll);
	else
			_tprintf(_T("OK - vJoy Driver and vJoyInterface DLL match vJoyInterface DLL (version % 04x)\n"), VerDrv);
	
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
					menu();
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
		Sleep(10);
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