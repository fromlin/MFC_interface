//////////////////////////////////////////////////////////////////////////////////////////////////////////
// File		: CommWork.cpp
// Version	: 1.0.1
// Date		: 2022.08.31
// Writer	: Kim, YeLin (MRAS)
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CommWork.h"

CCommWork::CCommWork(std::string name)
    :CWorkBase(name)
{
    memset(&_omega, 0, sizeof(OmegaData_t));
    memset(&_stm, 0, sizeof(StmData_t));

    _memname_omega = name + "_Omega";
    _memname_stm = name + "_Stm";
    CREATE_SYSTEM_MEMORY(_memname_omega, OmegaData_t);
    CREATE_SYSTEM_MEMORY(_memname_stm, StmData_t);

    _sendPacket.data.header[0] = _sendPacket.data.header[1] = _sendPacket.data.header[2] = _sendPacket.data.header[3] = 0xFF;
    _sendPacket.data.id = 1;
    _sendPacket.data.mode = 2;
    _sendPacket.data.size = sizeof(SendPacket_t);
}


CCommWork::~CCommWork() {

    DELETE_SYSTEM_MEMORY(_memname_omega);
    DELETE_SYSTEM_MEMORY(_memname_stm);

    ClosePort();
}



bool CCommWork::OpenPort(std::string name, int baudRate) {

    return _comm.Open(name.c_str(), baudRate);
}



void CCommWork::ClosePort() {
    StmData_t _stm;
    _stm.fdb[0] = _stm.fdb[1] = _stm.fdb[2] = 0;
    SET_SYSTEM_MEMORY("CommWork_Stm", _stm);
    _comm.Close();
}



void CCommWork::_execute() {

    GET_SYSTEM_MEMORY("CommWork_Omega", _omega);

    static int mode, readSize = 0, checkSize;
    static unsigned char check;

    if (_comm.isOpen()) {
        _sendPacket.data.check = 0;
        _sendPacket.data.pos[0] = (int)_omega.pos[0];
		_sendPacket.data.pos[1] = (int)_omega.pos[1];
		_sendPacket.data.pos[2] = (int)_omega.pos[2];
		_sendPacket.data.ori[0] = (int)_omega.ori[0];
		_sendPacket.data.ori[1] = (int)_omega.ori[1];
		_sendPacket.data.ori[2] = (int)_omega.ori[2];
		_sendPacket.data.grp = (int)_omega.grp;
	

        for (int i = 8; i < sizeof(SendPacket_t); i++)
            _sendPacket.data.check += _sendPacket.buffer[i];
        _comm.Write((char*)_sendPacket.buffer, sizeof(SendPacket_t));


        readSize = _comm.Read((char*)_recvBuf, 4096);// sizeof(RecvPacket_t));
        for (int i = 0; i < readSize; i++) {
            switch (mode) {
            case 0:
                if (_recvBuf[i] == 0xFE) {
                    checkSize++;
                    if (checkSize == 4) {
                        mode = 1;
                    }
                }
                else {
                    checkSize = 0;
                }
                break;

            case 1:
                // size, id ,mode, check 받은거 buffer에 저장
                _recvPacket.buffer[checkSize++] = _recvBuf[i];
                if (checkSize == 8) {
                    mode = 2;
                }
                break;

            case 2:
                // fdb 값 받은거 저장
                _recvPacket.buffer[checkSize++] = _recvBuf[i];
                check += _recvBuf[i];

                if (checkSize == _recvPacket.data.size) {
                    if (check == _recvPacket.data.check) {
                        _stm.fdb[0] = _recvPacket.data.fdb[0] / 10.;
                        _stm.fdb[1] = _recvPacket.data.fdb[1] / 10.;
                        _stm.fdb[2] = _recvPacket.data.fdb[2] / 10.;
                    }
                    check = 0;
                    mode = 0;
                    checkSize = 0;
                }
            }
        }
        SET_SYSTEM_MEMORY(_memname_stm, _stm);
    }
}