//////////////////////////////////////////////////////////////////////////////////////////////////////////
// File		: OmegaWork.h
// Version	: 1.0.1
// Date		: 2022.08.31
// Writer	: Kim, YeLin (MRAS)
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#ifndef _OMEGAWORK_H__
#define _OMEGAWORK_H__

#include "WorkBase.h"
#include "DataType.h"
#include "SystemMemory.h"
#include <iostream>

#include "dhdc.h"
#include "drdc.h"

#define GRIPPER_FORCE		0.07			// x 30 = max 2.1N(8N)

class COmegaWork : public CWorkBase {

public:
	COmegaWork(std::string name);
	virtual ~COmegaWork();

	bool OpenPort();
	void ClosePort();

protected:
	virtual void _execute();

private:
	std::string _memname_data;
	std::string _memname_state;

	OmegaData_t _data;
	OmegaData_t _temp;
	OmegaData_t _omega_comm;
	OmegaData_t _omega_udp;
	StmData_t _stm;
	KuKaData_t _kuka;

	int  _state;
	bool  _grap;
	double _grp;


	int _status[DHD_MAX_STATUS];
};

#endif