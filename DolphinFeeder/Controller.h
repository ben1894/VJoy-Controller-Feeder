#pragma once
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

const byte key = 255;
const int chunkSize = 5;

class Controller
{
	std::vector<byte> receivedData;
	int iInterface;
	int nButtons;
	CSerial serialPort;
	JOYSTICK_POSITION_V2 iReport;

public:
	Controller()
	{
	}

	~Controller()
	{
		RelinquishVJD(iInterface);
	}

	int configureInterface(int newInterface)
	{
		int iInterface = newInterface;
		VjdStat status = GetVJDStatus(iInterface);

		switch (status)
		{
		case VJD_STAT_OWN:
			printf("vJoy Device %d is already owned by this feeder\n", iInterface);
			break;
		case VJD_STAT_FREE:
			printf("vJoy Device %d is free\n", iInterface);
			break;
		case VJD_STAT_BUSY:
			printf("vJoy Device %d is already owned by another feeder\nCannot continue\n", iInterface);
			return -3;
		case VJD_STAT_MISS:
			printf("vJoy Device %d is not installed or disabled\nCannot continue\n", iInterface);
			return -4;
		default:
			printf("vJoy Device %d general error\nCannot continue\n", iInterface);
		}

		// Acquire the target
		if ((status == VJD_STAT_OWN) || ((status == VJD_STAT_FREE) && (!
			AcquireVJD(iInterface))))
		{
			printf("Failed to acquire vJoy device number %d.\n", iInterface);
			return -1;
		}
		else
		{
			printf("Acquired: vJoy device number %d.\n", iInterface);
		}

		nButtons = GetVJDButtonNumber(iInterface);
		iReport.bDevice = iInterface;

		return 1;
	}

	int configureSerialPort(int comPort)
	{
		return serialPort.Open(4, 9600);
	}

	void updateController()
	{
		int size = serialPort.ReadDataWaiting();
		if (size)
		{
			//store data to array
			byte* lpBuffer = new byte[size];
			int nBytesRead = serialPort.ReadData(lpBuffer, size);

			//add data to full vector
			receivedData.insert(receivedData.end(), &lpBuffer[0], &lpBuffer[size]);

			delete[]lpBuffer;
		}

		//search vector for first instance of key
		bool found = false;
		int position = 0;
		for (unsigned int i = 0; i < receivedData.size(); i++)
		{
			if (receivedData[i] == key)
			{
				found = true;
				position = i;
				break;
			}
		}

		//if the 255 exist in the vector
		//and all the bytes have been recieved (accounts for bytes already in buffer)
		if (found && ((receivedData.size() - position) > chunkSize))
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
			iReport.wAxisX = (receivedData[position + 2] * 100) + receivedData[position + 3];
			iReport.wAxisY = (receivedData[position + 4] * 100) + receivedData[position + 5];

			//erase values that were just read
			receivedData.erase(receivedData.begin(), receivedData.begin() + position + chunkSize);

			//update the dll
			UpdateVJD(iInterface, &iReport);
		}
	}
};
