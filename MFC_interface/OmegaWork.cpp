//////////////////////////////////////////////////////////////////////////////////////////////////////////
// File		: OmegaWork.cpp
// Version	: 1.0.1
// Date		: 2022.08.31
// Writer	: Kim, YeLin (MRAS)
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OmegaWork.h"

COmegaWork::COmegaWork(std::string name)
	:CWorkBase(name)
{
	memset(&_data, 0, sizeof(OmegaData_t));
	memset(&_temp, 0, sizeof(OmegaData_t));
	memset(&_omega_comm, 0, sizeof(OmegaData_t));
	memset(&_omega_udp, 0, sizeof(OmegaData_t));
	memset(&_stm, 0, sizeof(StmData_t));
	memset(&_kuka, 0, sizeof(KuKaData_t));
	memset(&_state, 0, sizeof(int));

	_memname_data = name + "_Data";
	_memname_state = name + "_State";
	CREATE_SYSTEM_MEMORY(_memname_data, OmegaData_t);
	CREATE_SYSTEM_MEMORY(_memname_state, int);
}

COmegaWork::~COmegaWork() {

	DELETE_SYSTEM_MEMORY(_memname_data);
	DELETE_SYSTEM_MEMORY(_memname_state);

	ClosePort();
}

bool COmegaWork::OpenPort() {

	return drdOpen()<0?false:true;
}

void COmegaWork::ClosePort() {

	_state = 0;
	SET_SYSTEM_MEMORY("OmegaWork_State", _state);
	drdMoveToPos(-0.04, 0.0, 0.04);
	drdClose();
}

void COmegaWork::_execute() {

	GET_SYSTEM_MEMORY("OmegaWork_State", _state);
	GET_SYSTEM_MEMORY("CommWork_Stm", _stm);
	GET_SYSTEM_MEMORY("UdpClientWork_KuKa", _kuka);

	dhdGetStatus(_status);
	switch (_state) {
	case 0:
		// before init
		break;

	case 1:
		if (_status[DHD_STATUS_BRAKE]) {
			dhdEnableForce(DHD_ON);
		}

		// get position, orientation and gripper
		dhdGetPositionAndOrientationDeg(&_data.pos[0], &_data.pos[1], &_data.pos[2], &_data.ori[0], &_data.ori[1], &_data.ori[2]);
		dhdGetGripperAngleDeg(&_data.grp);

		// apply force
		if ((_data.grp > 13) && _grap) {
			_grap = false;
		}
		else if ((_data.grp < 3) && !_grap) {
			_grap = true;
			memcpy(&_temp, &_data, sizeof(OmegaData_t));
		}
		_grp = (30 - _data.grp) * GRIPPER_FORCE;
		//dhdSetForceAndGripperForce(_stm.fdb[0], _stm.fdb[1], _stm.fdb[2], _grp);
		//dhdSetForceAndTorqueAndGripperForce(_stm.fdb[0], _stm.fdb[1], _stm.fdb[2], 0.0, 0.0, 0.0, _grp);
		dhdSetForceAndTorqueAndGripperForce(_kuka.force[0], _kuka.force[1], _kuka.force[2], _kuka.torq[0], _kuka.torq[1], _kuka.torq[2], _grp);

		/* COL project */
		_omega_comm.pos[0] = _data.pos[0] * INV_LINEAR_RESOLUTION;		_omega_comm.pos[1] = _data.pos[1] * INV_LINEAR_RESOLUTION;		_omega_comm.pos[2] = _data.pos[2] * INV_LINEAR_RESOLUTION;
		_omega_comm.ori[0] = _data.ori[0] * INV_ANGULAR_RESOLUTION;		_omega_comm.ori[1] = _data.ori[1] * INV_ANGULAR_RESOLUTION;		_omega_comm.ori[2] = _data.ori[2] * INV_ANGULAR_RESOLUTION;
		_omega_comm.grp = _grap;

		/* KuKa project */
		_omega_udp.pos[0] = (_data.pos[0] - _temp.pos[0]) * 1000.;
		_omega_udp.pos[1] = (_data.pos[1] - _temp.pos[1]) * 1000.;
		_omega_udp.pos[2] = (_data.pos[2] - _temp.pos[2]) * 1000.;
		_omega_udp.ori[0] = -DEG2RAD*((_data.ori[0] - _temp.ori[0]) / 5.);
		_omega_udp.ori[1] = -DEG2RAD*((_data.ori[1] - _temp.ori[1]) / 5.);
		_omega_udp.ori[2] =  DEG2RAD*((_data.ori[2] - _temp.ori[2]) / 5.);
		_omega_udp.grp = _grap;


		SET_SYSTEM_MEMORY("OmegaWork_Data", _data);
		SET_SYSTEM_MEMORY("CommWork_Omega", _omega_comm);
		SET_SYSTEM_MEMORY("UdpClientWork_Omega", _omega_udp);

		break;
	}
}
