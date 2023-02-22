//////////////////////////////////////////////////////////////////////////////////////////////////////////
// File		: CanWork.h
// Version	: 1.0.1
// Date		: 2022.08.31
// Writer	: Kim, YeLin (MRAS)
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef _CANWORK_H__
#define _CANWORK_H__

#include "WorkBase.h"
#include "DataType.h"
#include "SystemMemory.h"

#include <Eigen/Dense>
#include <PCANBasic.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <iomanip>
#include <iostream>
#include <fstream>

#define BIRRATE				 PCAN_BAUD_1M
#define DF		                1./50.
#define DT		                1./2000.

class CCanWork :
    public CWorkBase
{
public:
	CCanWork(std::string name);
	virtual ~CCanWork();

	bool OpenPort();
	void ClosePort();

protected:
	virtual void _execute();

private:
	std::string _memname_data;
	std::string _memname_grav;

	CanData_t _can;
	CanData_t _can_filter;
	TPCANMsg _sendBuf;
	TPCANMsg _recvBuf[2];
	TPCANTimestamp _canTimeStamp;

	bool _isOpen;
	short raw[2][3];
	const TPCANHandle _canHandle = PCAN_USBBUS1;

	int udp_state;
	KuKaData_t kuka_data;
	Eigen::Vector3d fvec;

	SimpleMovingAverage<float, 2000> maf[3];
	int _cnt;

	std::ofstream fout;  
};






#endif