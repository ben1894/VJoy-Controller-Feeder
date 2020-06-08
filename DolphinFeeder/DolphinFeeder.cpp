// DolphinFeeder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <Windows.h>
#include <iostream>
#include <vector>
#include "tchar.h"
#include "Serial.h"
#include "public.h"
#include "vjoyinterface.h"
const byte key = 255; 
const int chunkSize = 5;


int main()
{
	if (!vJoyEnabled())
	{
		_tprintf(_T("Failed Getting vJoy attributes.\n"));
		return -2;
	}	WORD VerDll, VerDrv;
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
	//if(end - pos > length of numbers - do the data thing)
	
	std::vector<byte> receivedData;
	
	while(1)
	{
		int size = test.ReadDataWaiting();
		if(size)
		{
			byte* lpBuffer = new byte[size];
			int nBytesRead = test.ReadData(lpBuffer, size);

			/*for(int i = 0; i < size; i++)
			{
				std::cout << lpBuffer[i];
			}*/
			
			receivedData.insert(receivedData.end(), &lpBuffer[0], &lpBuffer[size]);
			
			delete[]lpBuffer;
			//std::cout << "\n";
		}

		bool found = false;
		int position = 0;
		//std::cout << "1";
		for(unsigned int i = 0; i < receivedData.size(); i++)
		{
			if(receivedData[i] == key)
			{
				found = true;
				position = i;
				break;
			}
		}
		//std::cout << position;
		if(found)
		{
			JOYSTICK_POSITION_V2 iReport;
			iReport.bDevice = 1;
			/*
			EEBlue.write(x100);
			EEBlue.write(x10);
			//EEBlue.print(x1);

			EEBlue.write(y100);
			EEBlue.write(y10);
			//EEBlue.print(y1);
			*/
			/*for(unsigned int i = 0; i < receivedData.size(); i++)
			{
				std::cout << (int)receivedData[i] << ",";
			}*/
			//std::cout << "\n";
			//check if other all bytes have been received
			if((receivedData.size() - position) > chunkSize)
			{
				iReport.lButtons = 0; //pointer was reading old memory data. Have to manually clear it.
				//std::cout << (int)receivedData[position + 1] << "\n";
				iReport.lButtons |= (long)receivedData[position + 1];
				iReport.wAxisX = (receivedData[position + 2]*100) + receivedData[position + 3];
				iReport.wAxisY =  (receivedData[position + 4]*100) + receivedData[position + 5];

				//erase values that were just read
				receivedData.erase(receivedData.begin(), receivedData.begin() + position + chunkSize);
			}
			UpdateVJD(1, &iReport);
		}

		//Delay to not use all the CPU checking for values that aren't sent yet
		Sleep(10);
	}
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
