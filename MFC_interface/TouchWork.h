//////////////////////////////////////////////////////////////////////////////////////////////////////////
// File		: TouchWork.h
// Version	: 1.0.1
// Date		: 2022.08.31
// Writer	: Kim, YeLin (MRAS)
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#define _CRT_SECURE_NO_WARNINGS

#ifndef _TOUCHWORK_H__
#define _TOUCHWORK_H__

#include "WorkBase.h"
#include "DataType.h"
#include "SystemMemory.h"
#include <iostream>

#include "HD/hd.h"
#include "HDU/hduError.h"
#include "HDU/hduVector.h"

#define FORCE_GAIN			0.1
#define SPRING_K			0.1		

class CTouchWork : public CWorkBase {
	
public:
	CTouchWork(std::string name);
	virtual ~CTouchWork();

	bool OpenPort();
	void ClosePort();

protected:
	virtual void _execute();

private:
	std::string _memname_touch;

	HHD hHD;
	HDCallbackCode hPlaneCallback;
};

#endif