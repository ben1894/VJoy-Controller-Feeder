#include "Controller.h"
Controller::Controller()
{
	int iInterface = 1;
	VjdStat status = GetVJDStatus(iInterface);
	int nButtons = GetVJDButtonNumber(iInterface);
	CSerial test;
	std::vector<byte> receivedData;
	JOYSTICK_POSITION_V2 iReport;
	iReport.bDevice = iInterface;
}
