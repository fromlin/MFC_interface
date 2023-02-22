//////////////////////////////////////////////////////////////////////////////////////////////////////////
// File		: ArtecWork.h
// Version	: 1.0.1
// Date		: 2022.11.04
// Writer	: Kim, YeLin (MRAS)
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef _ARTECWORK_H__
#define _ARTECWORK_H__

#include "WorkBase.h"
#include "DataType.h"
#include "SystemMemory.h"

#include <Eigen/Dense>
#include <string.h>
#include <iostream>
#include <fstream>

#ifndef  _M_IX86

#include "IScanner.h"
#include "IArrayScannerId.h"
#include "IFrameProcessor.h"
#include "IFrame.h"
#include "BaseSdkDefines.h"
#include "Log.h"
#include "ObjIO.h"
#include "PlyIO.h"
#include "ImageIO.h"
#include "IFrameMesh.h"
#include "TArrayRef.h"
#include "IScanner.h"

namespace asdk {
	using namespace artec::sdk::base;
	using namespace artec::sdk::capturing;
};
using asdk::TRef;
using asdk::TArrayRef;

class CArtecWork : public CWorkBase {

public:
	CArtecWork(std::string name);
	virtual ~CArtecWork();

	bool OpenPort();
	void ClosePort();

protected:
	virtual void _execute();

private:
	std::string _memname_data;
	//Eigen::Vector3d fvec;
	//std::ofstream fout;

public:
	TRef<asdk::IScanner> scanner;
	asdk::ErrorCode ec;

	PArtecData_t _pad;

	uint16_t num;

	void captureOneFrame();
};
#endif

#endif