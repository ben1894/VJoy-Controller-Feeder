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

class Controller
{
	int iInterface = 1;
	VjdStat status = GetVJDStatus(iInterface);
	int nButtons = GetVJDButtonNumber(iInterface);
	CSerial test;
	std::vector<byte> receivedData;
	JOYSTICK_POSITION_V2 iReport;

	Controller()
	{
		iReport.bDevice = iInterface;
	}
};