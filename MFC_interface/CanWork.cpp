//////////////////////////////////////////////////////////////////////////////////////////////////////////
// File		: CanWork.cpp
// Version	: 1.0.1
// Date		: 2022.08.31
// Writer	: Kim, YeLin (MRAS)
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CanWork.h"

CCanWork::CCanWork(std::string name)
	:CWorkBase(name)
{
	memset(&_can, 0, sizeof(CanData_t));
	memset(&raw, 0, sizeof(raw));

	_memname_data = name + "_Data";
	_memname_grav = name + "_Grav";
	CREATE_SYSTEM_MEMORY(_memname_data, CanData_t);
	CREATE_SYSTEM_MEMORY(_memname_grav, Eigen::Vector3d);
}

CCanWork::~CCanWork() {

	ClosePort();
}

bool CCanWork::OpenPort() {

	TPCANStatus res;
	res = CAN_Initialize(_canHandle, BIRRATE);
	if (res != PCAN_ERROR_OK) {
		std::cout << "Can not initialize. Please check the defines in the code \n";
		return 0;
	}
	else {
		_sendBuf.ID = 0x065;
		_sendBuf.LEN = 8;
		_sendBuf.DATA[0] = (BYTE)8;
		_sendBuf.DATA[1] = (BYTE)1;
		_sendBuf.DATA[2] = (BYTE)14;		// Cutoff 5Hz : 11
		res = CAN_Write(_canHandle, &_sendBuf);
		if (res != PCAN_ERROR_OK) {
			std::cout << "Filter can't setting \n";
			return 0;
		}
		else {
			_sendBuf.DATA[0] = (BYTE)17;
			_sendBuf.DATA[1] = (BYTE)1;
			res = CAN_Write(_canHandle, &_sendBuf);
			if (res != PCAN_ERROR_OK) {
				std::cout << "Bias can't setting \n";
				return 0;
			}
			else {
				_sendBuf.DATA[0] = (BYTE)15;
				_sendBuf.DATA[1] = (BYTE)8;	// Output 1000 Hz
				res = CAN_Write(_canHandle, &_sendBuf);
				if (res != PCAN_ERROR_OK) {
					std::cout << "Output rate can't setting \n";
					return 0;
				}
				else {
					std::cout << "Successfully initialized \n";
					_sendBuf.DATA[0] = (BYTE)11;
					res = CAN_Write(_canHandle, &_sendBuf);
					if (res != PCAN_ERROR_OK) {
						std::cout << "Data Request Fail \n";
						return 0;
					}
					std::cout << "Data Request Success \n";
					_recvBuf->LEN = 16;


					//fout.open("data/time/force.csv");
					//if (!fout.is_open()) {
					//	std::cout << "Can't Open Files!!! \n" << std::endl;
					//	return 0;
					//}
					//fout.clear();
					//fout << "Force_X,Force_Y,Force_Z\n";


					_isOpen = true;
					return 1;
				}
			}
		}
	}
}

void CCanWork::ClosePort() {

	_isOpen = false;


	//fout.close();
	

	memset(&_sendBuf, 0, sizeof(_sendBuf));
	_sendBuf.DATA[0] = (BYTE)12;
	CAN_Write(_canHandle, &_sendBuf);
	CAN_Uninitialize(_canHandle);
}

void CCanWork::_execute() {

	_sendBuf.DATA[0] = (BYTE)11;
	CAN_Write(_canHandle, &_sendBuf);

	GET_SYSTEM_MEMORY("UdpClientWork_KuKa", kuka_data);
	GET_SYSTEM_MEMORY("UdpClientWork_State", udp_state);

	unsigned short raw_force[3] = { 0,0,0 };
	unsigned short raw_torq[3] = { 0,0,0 };

	if (_isOpen) {
		for (int i = 0; i < 2; i++) {
			TPCANStatus stsResult = CAN_Read(_canHandle, &_recvBuf[i], &_canTimeStamp);
			if (stsResult != PCAN_ERROR_QRCVEMPTY) {
				switch (_recvBuf[i].ID) {
				case 3:
					if (_recvBuf[i].DATA[0] == 0x0B) {
						raw_force[0] += _recvBuf[i].DATA[1] * 256;		raw_force[0] += _recvBuf[i].DATA[2];
						raw_force[1] += _recvBuf[i].DATA[3] * 256;		raw_force[1] += _recvBuf[i].DATA[4];
						raw_force[2] += _recvBuf[i].DATA[5] * 256;		raw_force[2] += _recvBuf[i].DATA[6];
						raw_torq[0] += _recvBuf[i].DATA[7] * 256;
						for (int j = 0; j < 3; j++) {
							raw[0][j] = (short)raw_force[j];
							_can.force[j] = (double)raw[0][j] * DF;
						}
					}
					break;

				case 4:
					raw_torq[0] += _recvBuf[i].DATA[0];
					raw_torq[1] += _recvBuf[i].DATA[1] * 256;		raw_torq[1] += _recvBuf[i].DATA[2];
					raw_torq[2] += _recvBuf[i].DATA[3] * 256;		raw_torq[2] += _recvBuf[i].DATA[4];
					for (int j = 0; j < 3; j++) {
						raw[1][j] = (short)raw_torq[j];
						_can.torq[j] = (float)raw[1][j] * DT;
					}
					break;
				}
			}
		}

		/* Moving Average Filter */
		_cnt++;
		for (int j = 0; j < 3; j++) {
			const auto input = _can.force[j];
			if (_cnt < maf[j].count()) {
				maf[j].calculate(input);
			}
			else {
				fvec[j] = maf[j].calculate(input).present_value();
				_cnt = 0;
			}
		}
		

		if (udp_state && (_cnt == 0)) {	// ){//
			Eigen::Matrix3d rotR;
			Eigen::Vector3d _fvec;

			rotR << cos(kuka_data.ori[1]) * cos(kuka_data.ori[2]), sin(kuka_data.ori[0])* sin(kuka_data.ori[1])* cos(kuka_data.ori[2]) - cos(kuka_data.ori[0]) * sin(kuka_data.ori[2]), cos(kuka_data.ori[0])* sin(kuka_data.ori[1])* cos(kuka_data.ori[2]) + sin(kuka_data.ori[0]) * sin(kuka_data.ori[2]),
				cos(kuka_data.ori[1])* sin(kuka_data.ori[2]), sin(kuka_data.ori[0])* sin(kuka_data.ori[1])* sin(kuka_data.ori[2]) + cos(kuka_data.ori[0]) * cos(kuka_data.ori[2]), cos(kuka_data.ori[0])* sin(kuka_data.ori[1])* sin(kuka_data.ori[2]) - sin(kuka_data.ori[0]) * cos(kuka_data.ori[2]),
				-sin(kuka_data.ori[1]), sin(kuka_data.ori[0])* cos(kuka_data.ori[1]), cos(kuka_data.ori[0])* cos(kuka_data.ori[1]);
			_fvec = rotR * fvec;
			//printf("Ori : %+4.4lf, %+4.4lf, %+4.4lf\t\t", _can.force[0], _can.force[1], _can.force[2]);
			//printf("Rot : %+4.4lf, %+4.4lf, %+4.4lf\n", _fvec[0], _fvec[1], _fvec[2]);


			//fout << _fvec[0] << "," << _fvec[1] << "," << _fvec[2] << "\n";

			udp_state = false;
			SET_SYSTEM_MEMORY("CanWork_Grav", _fvec);
		}

		//for (int i = 0; i < 3; i++) {
		//	_can.force[i] = (fvec[i] * 0.001) / (0.2 * 0.001);
		//}
		SET_SYSTEM_MEMORY("CanWork_Data", _can);
	}
}