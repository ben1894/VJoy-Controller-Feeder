// DolphinFeeder.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <Windows.h>
#include <iostream>
#include <vector>
#include "tchar.h"
#include "Serial.h"
#include "public.h"
#include "vjoyinterface.h"

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

	int nButtons = GetVJDButtonNumber(iInterface);

	CSerial test;
	while (test.Open(3, 9600) == 0)
	{
		Sleep(500);
		std::cout << "oof";
	}

	std::vector<unsigned char> full;
	while (1)
	{
		int size = test.ReadDataWaiting();
		if (size)
		{
			char* lpBuffer = new char[size];
			int nBytesRead = test.ReadData(lpBuffer, size);
			
			for (int i = 0; i < size; i++)
			{
				std::cout << lpBuffer[i];
			}
			dataVec.insert(dataVec.end(), &dataArray[0], &dataArray[dataArraySize]);
			delete[]lpBuffer;
		}
		Sleep(1000);
	}
    std::cout << "Hello World!\n";
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
