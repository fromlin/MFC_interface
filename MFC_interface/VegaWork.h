//////////////////////////////////////////////////////////////////////////////////////////////////////////
// File		: VegaWork.h
// Version	: 1.0.1
// Date		: 2022.11.18
// Writer	: Kim, YeLin (MRAS)
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef _VEGAWORK_H__
#define _VEGAWORK_H__

#include "WorkBase.h"
#include "DataType.h"
#include "SystemMemory.h"

#include <windows.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <math.h>

#include "CombinedApi.h"
#include "PortHandleInfo.h"
#include "ToolData.h"

#define M_PI			3.14159265358979323846
#define RAD_DEGREE(x)	(x/M_PI)*180.
#define DEGREE_RAD(y)	(y/180.)*M_PI

class CVegaWork : public CWorkBase
{
public:
	CVegaWork(std::string name);
	virtual ~CVegaWork();

	bool OpenPort();
	void ClosePort();

protected:
	virtual void _execute();

private:
	std::string _memname_data;

public:
	CombinedApi capi;
	bool apiSupportsBX2;

	void captureOneFrame();

	void sleepSeconds(unsigned numSeconds);
	void onErrorPrintDebugMessage(std::string methodName, int errorCode);
	std::string getToolInfo(std::string toolHandle);
	void singularitycheck(int group, double theta);
	void quaternionToEuler(std::stringstream& stream, const ToolData& toolData);
	std::string toolTrackingData(const ToolData& toolData);
	void printToolData(const ToolData& toolData);
	void printTrackingData();
	void initializeAndEnableTools();
	void loadTool(const char* toolDefinitionFilePath);
	void configurePassiveTools();
	void configureActiveTools(std::string scuHostname);
	void configureActiveWirelessTools();
	void configureDummyTools();
	void configureUserParameters();
	void simulateAlerts(uint32_t simulatedAlerts = 0x00000000);
	void determineApiSupportForBX2();
	void trackingPassiveTools();
};

#endif