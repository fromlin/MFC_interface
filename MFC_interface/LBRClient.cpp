//////////////////////////////////////////////////////////////////////////////////////////////////////////
// File		: LBRClient.cpp
// Version	: 1.0.1
// Date		: 2022.08.31
// Writer	: Kim, YeLin (MRAS)
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef  _M_IX86

#include "LBRClient.h"

using namespace KUKA::FRI;

MyLBRClient::MyLBRClient(double forceX, double forceY, double forceZ)
    : _forceX(forceX)
    , _forceY(forceY)
    , _forceZ(forceZ)
{
    memset(&_wrench, 0, sizeof(_wrench));
    memset(&_touch, 0, sizeof(TouchData_t));
    memset(&_kuka, 0, sizeof(KuKaData_t));

    printf("LBROverlayClient initialized:\n");
}

MyLBRClient::~MyLBRClient() {

}

void MyLBRClient::onStateChange(ESessionState oldState, ESessionState newState)
{
    LBRClient::onStateChange(oldState, newState);

    switch (newState) {
    case MONITORING_WAIT:
        break;

    case MONITORING_READY:
        memset(&_joint, 0, sizeof(_joint));
        memset(&_torques, 0, sizeof(_torques));
        memset(&_wrench, 0, sizeof(_wrench));
        break;

    case COMMANDING_WAIT:
        break;

    case COMMANDING_ACTIVE:
        break;

    default:
        break;
    }
}

void MyLBRClient::monitor() {

    LBRClient::monitor();

}

void MyLBRClient::waitForCommand() {

    LBRClient::waitForCommand();

    switch (robotState().getClientCommandMode()) {
    case JPOSITION:
        robotCommand().setJointPosition(_joint);
        break;

    case WRENCH:
        robotCommand().setWrench(_wrench);
        break;

    case TORQUE:
        robotCommand().setJointPosition(_joint);
        robotCommand().setTorque(_torques);
        break;

    default:
        break;
    }

}

void MyLBRClient::command() {

    LBRClient::command();

    //RowVector4d _g = { 0.0, 0.0, 9.80665 , 0.0 };
    //_receive = robotState().getMeasuredJointPosition();
    //memcpy(&_joint, _receive, sizeof(_joint));
    //
    //_T = transformMat(_joint[0], DH_D1, 0.0, DH_ALPHA) * transformMat(_joint[1], 0.0, 0.0, DH_ALPHA) * transformMat(_joint[2], DH_D3, 0.0, DH_ALPHA) * transformMat(_joint[3], 0.0, 0.0, DH_ALPHA) *
    //	transformMat(_joint[4], DH_D5, 0.0, DH_ALPHA) * transformMat(_joint[5], 0.0, 0.0, DH_ALPHA) * transformMat(_joint[6], DH_D7, 0.0, 0.0);
    //_g = _g * (_T.inverse());
    //std::cout << _g << std::endl;
    //for (int i = 0; i < 3; i++) {
    //    _wrench[i] = _g[i];
    //}

    //memset(&_etorques, 0, sizeof(_etorques));
    //memcpy(_etorques, robotState().getExternalTorque(), sizeof(_etorques));
    //printf("j[0]:%6.4lf\tj[1]:%6.4lf\tj[2]:%6.4lf\tj[3]:%6.4lf\tj[4]:%6.4lf\tj[5]:%6.4lf\tj[6]:%6.4lf\n",
    //    _etorques[0], _etorques[1], _etorques[2], _etorques[3], _etorques[4], _etorques[5], _etorques[6]);


    switch (robotState().getClientCommandMode()) {
    case JPOSITION:
        robotCommand().setJointPosition(_joint);
        break;

    case WRENCH:
        robotCommand().setWrench(_wrench);
        break;

    case TORQUE:
        //memcpy(_joint, robotState().getMeasuredJointPosition(), sizeof(_joint));

        //GET_SYSTEM_MEMORY("TouchWork_Data", _touch);
        //if (_touch.state == 0) {
            GET_SYSTEM_MEMORY("UdpClientWork_KuKa", _kuka);
            //GET_SYSTEM_MEMORY("CanWork_Grav", _fvec);
            //_dist = ((_fvec * 0.001) + (USER_DAMPING * _dist)) / ((USER_STIFFNESS * 0.001) + USER_DAMPING);
            //if (_dist.norm() > USER_DIST_LIMIT) {
            //    memset(&_torques, 0, sizeof(_torques));
            //}
            //else {
            for (int i = 0; i < KUKA::FRI::LBRState::NUMBER_OF_JOINTS; i++) {
                _joint[i] = _kuka.jpos[i];
                _torques[i] = -_kuka.jtorq[i];
            }
            _torques[0] = -0.05;
            //}
            robotCommand().setJointPosition(_joint);
            robotCommand().setTorque(_torques);
        //}
        break;

    default:
        break;
    }
}





TFproviClient::TFproviClient() {

    for (int row = 0; row < 3; row++) {
        for (int column = 0; column < 4; column++) {                // _tfMat(3x4) : 
            if (row == column)      _tf[row][column] = 1;           // { 1, 0, 0, 0 },
            else if (column == 3)   _tf[row][column] = 0;           // { 0, 1, 0, 0 },
            else                    _tf[row][column] = 0;           // { 0, 0, 1, 0 }                                     
        }
    }


    //fout.open("data/time/dist.csv");
    //if (!fout.is_open()) {
    //    std::cout << "Can't Open Files!!! \n" << std::endl;
    //    return;
    //}
    //fout.clear();
    //fout << "Dist_X,Dist_Y,Dist_Z,Kuka_X,Kuka_Y,Kuka_Z\n";


    //_pos[0] = _tf[0][3] = -665.0;
    //_pos[1] = _tf[1][3] =    0.0;
    //_pos[2] = _tf[2][3] =  295.0;
}

TFproviClient::~TFproviClient() {

    //fout.close();
}

void TFproviClient::provide() {

    GET_SYSTEM_MEMORY("CanWork_Grav", _fvec);
    GET_SYSTEM_MEMORY("UdpClientWork_KuKa", _kuka);
    GET_SYSTEM_MEMORY("TouchWork_Data", _touch);

	switch (_touch.state) {
    case 0:
        _dist = ((_fvec * 0.0005) + (USER_DAMPING * _dist)) / ((USER_STIFFNESS * 0.0005) + USER_DAMPING);
        if (_dist.norm() > USER_DIST_LIMIT) {
            for (int i = 0; i < 3; i++) {
                _tf[i][3] = _kuka.pos[i] + _dist[i];
            }
        }
        else {
            for (int i = 0; i < 3; i++) {
                _tf[i][3] = _kuka.pos[i];
            }
        }
        break;

    case 1:
        _tf[0][3] = _kuka.pos[0] + _touch.pos[2];
        _tf[1][3] = _kuka.pos[1] + _touch.pos[0];
        _tf[2][3] = _kuka.pos[2] + _touch.pos[1];
        break;
    }


    //fout << _dist[0] << "," << _dist[1] << "," << _dist[2] << _dist[0] << "," << _kuka.pos[0] << "," << _kuka.pos[1] << "," << _kuka.pos[2] << "\n";

    
	setTransformation("PBase", _tf, getTimestampSec(), getTimestampNanoSec());
    //printf(" X:%+4.2lf, Y:%+4.2lf, Z:%+4.2lf\n", _tf[0][3], _tf[1][3], _tf[2][3]);
}


//_erecv = robotState().getExternalTorque();            _mrecv = robotState().getMeasuredTorque();
//memcpy(&_etorques, _erecv, sizeof(_etorques));        memcpy(&_mtorques, _mrecv, sizeof(_mtorques));
//for (int i = 0; i < KUKA::FRI::LBRState::NUMBER_OF_JOINTS; i++) {
//    _torques[i] = 0.0;// _mtorques[i] - _etorques[i];
//}
//printf("%5.3lf,  %5.3lf,  %5.3lf,  %5.3lf,  %5.3lf,  %5.3lf,  %5.3lf\n\n",
//    _torques[0], _torques[1], _torques[2], _torques[3], _torques[4], _torques[5], _torques[6]);


Matrix4d MyLBRClient::transformMat(double theta, double d, double a, double alpha) {

    Matrix4d T;
    T << cos(theta), -sin(theta) * cos(alpha), sin(theta)* sin(alpha), a* cos(theta),
        sin(theta), cos(theta)* cos(alpha), -cos(theta) * sin(alpha), a* sin(theta),
        0, sin(alpha), cos(alpha), d,
        0, 0, 0, 1;

    return T;
}

//_T = forwardKinematics(_joint);
//Matrix4d MyLBRClient::forwardKinematics(double* j) {
//
//    Matrix4d T;
//    T << cos(j[6]) * (sin(j[5]) * (sin(j[3]) * (sin(j[0]) * sin(j[2]) + cos(j[0]) * cos(j[1]) * cos(j[2])) - cos(j[0]) * cos(j[3]) * sin(j[1])) + cos(j[5]) * (cos(j[4]) * (cos(j[3]) * (sin(j[0]) * sin(j[2]) + cos(j[0]) * cos(j[1]) * cos(j[2])) + cos(j[0]) * sin(j[1]) * sin(j[3])) - sin(j[4]) * (cos(j[2]) * sin(j[0]) - cos(j[0]) * cos(j[1]) * sin(j[2])))) + sin(j[6]) * (sin(j[4]) * (cos(j[3]) * (sin(j[0]) * sin(j[2]) + cos(j[0]) * cos(j[1]) * cos(j[2])) + cos(j[0]) * sin(j[1]) * sin(j[3])) + cos(j[4]) * (cos(j[2]) * sin(j[0]) - cos(j[0]) * cos(j[1]) * sin(j[2]))),
//        cos(j[6])* (sin(j[4]) * (cos(j[3]) * (sin(j[0]) * sin(j[2]) + cos(j[0]) * cos(j[1]) * cos(j[2])) + cos(j[0]) * sin(j[1]) * sin(j[3])) + cos(j[4]) * (cos(j[2]) * sin(j[0]) - cos(j[0]) * cos(j[1]) * sin(j[2]))) - sin(j[6]) * (sin(j[5]) * (sin(j[3]) * (sin(j[0]) * sin(j[2]) + cos(j[0]) * cos(j[1]) * cos(j[2])) - cos(j[0]) * cos(j[3]) * sin(j[1])) + cos(j[5]) * (cos(j[4]) * (cos(j[3]) * (sin(j[0]) * sin(j[2]) + cos(j[0]) * cos(j[1]) * cos(j[2])) + cos(j[0]) * sin(j[1]) * sin(j[3])) - sin(j[4]) * (cos(j[2]) * sin(j[0]) - cos(j[0]) * cos(j[1]) * sin(j[2])))),
//        cos(j[5])* (sin(j[3]) * (sin(j[0]) * sin(j[2]) + cos(j[0]) * cos(j[1]) * cos(j[2])) - cos(j[0]) * cos(j[3]) * sin(j[1])) - sin(j[5]) * (cos(j[4]) * (cos(j[3]) * (sin(j[0]) * sin(j[2]) + cos(j[0]) * cos(j[1]) * cos(j[2])) + cos(j[0]) * sin(j[1]) * sin(j[3])) - sin(j[4]) * (cos(j[2]) * sin(j[0]) - cos(j[0]) * cos(j[1]) * sin(j[2]))),
//        (63.0 * cos(j[5]) * (sin(j[3]) * (sin(j[0]) * sin(j[2]) + cos(j[0]) * cos(j[1]) * cos(j[2])) - cos(j[0]) * cos(j[3]) * sin(j[1]))) / 500 - (21 * cos(j[0]) * sin(j[1])) / 50 - (63 * sin(j[5]) * (cos(j[4]) * (cos(j[3]) * (sin(j[0]) * sin(j[2]) + cos(j[0]) * cos(j[1]) * cos(j[2])) + cos(j[0]) * sin(j[1]) * sin(j[3])) - sin(j[4]) * (cos(j[2]) * sin(j[0]) - cos(j[0]) * cos(j[1]) * sin(j[2])))) / 500.0 - (2.0 * sin(j[3]) * (sin(j[0]) * sin(j[2]) + cos(j[0]) * cos(j[1]) * cos(j[2]))) / 5.0 + (2.0 * cos(j[0]) * cos(j[3]) * sin(j[1])) / 5.0,
//        -cos(j[6]) * (sin(j[5]) * (sin(j[3]) * (cos(j[0]) * sin(j[2]) - cos(j[1]) * cos(j[2]) * sin(j[0])) + cos(j[3]) * sin(j[0]) * sin(j[1])) + cos(j[5]) * (cos(j[4]) * (cos(j[3]) * (cos(j[0]) * sin(j[2]) - cos(j[1]) * cos(j[2]) * sin(j[0])) - sin(j[0]) * sin(j[1]) * sin(j[3])) - sin(j[4]) * (cos(j[0]) * cos(j[2]) + cos(j[1]) * sin(j[0]) * sin(j[2])))) - sin(j[6]) * (sin(j[4]) * (cos(j[3]) * (cos(j[0]) * sin(j[2]) - cos(j[1]) * cos(j[2]) * sin(j[0])) - sin(j[0]) * sin(j[1]) * sin(j[3])) + cos(j[4]) * (cos(j[0]) * cos(j[2]) + cos(j[1]) * sin(j[0]) * sin(j[2]))),
//        sin(j[6])* (sin(j[5]) * (sin(j[3]) * (cos(j[0]) * sin(j[2]) - cos(j[1]) * cos(j[2]) * sin(j[0])) + cos(j[3]) * sin(j[0]) * sin(j[1])) + cos(j[5]) * (cos(j[4]) * (cos(j[3]) * (cos(j[0]) * sin(j[2]) - cos(j[1]) * cos(j[2]) * sin(j[0])) - sin(j[0]) * sin(j[1]) * sin(j[3])) - sin(j[4]) * (cos(j[0]) * cos(j[2]) + cos(j[1]) * sin(j[0]) * sin(j[2])))) - cos(j[6]) * (sin(j[4]) * (cos(j[3]) * (cos(j[0]) * sin(j[2]) - cos(j[1]) * cos(j[2]) * sin(j[0])) - sin(j[0]) * sin(j[1]) * sin(j[3])) + cos(j[4]) * (cos(j[0]) * cos(j[2]) + cos(j[1]) * sin(j[0]) * sin(j[2]))),
//        sin(j[5])* (cos(j[4]) * (cos(j[3]) * (cos(j[0]) * sin(j[2]) - cos(j[1]) * cos(j[2]) * sin(j[0])) - sin(j[0]) * sin(j[1]) * sin(j[3])) - sin(j[4]) * (cos(j[0]) * cos(j[2]) + cos(j[1]) * sin(j[0]) * sin(j[2]))) - cos(j[5]) * (sin(j[3]) * (cos(j[0]) * sin(j[2]) - cos(j[1]) * cos(j[2]) * sin(j[0])) + cos(j[3]) * sin(j[0]) * sin(j[1])),
//        (63.0 * sin(j[5]) * (cos(j[4]) * (cos(j[3]) * (cos(j[0]) * sin(j[2]) - cos(j[1]) * cos(j[2]) * sin(j[0])) - sin(j[0]) * sin(j[1]) * sin(j[3])) - sin(j[4]) * (cos(j[0]) * cos(j[2]) + cos(j[1]) * sin(j[0]) * sin(j[2])))) / 500.0 - (63.0 * cos(j[5]) * (sin(j[3]) * (cos(j[0]) * sin(j[2]) - cos(j[1]) * cos(j[2]) * sin(j[0])) + cos(j[3]) * sin(j[0]) * sin(j[1]))) / 500.0 - (21.0 * sin(j[0]) * sin(j[1])) / 50.0 + (2.0 * sin(j[3]) * (cos(j[0]) * sin(j[2]) - cos(j[1]) * cos(j[2]) * sin(j[0]))) / 5.0 + (2.0 * cos(j[3]) * sin(j[0]) * sin(j[1])) / 5.0,
//        sin(j[6])* (sin(j[4]) * (cos(j[1]) * sin(j[3]) - cos(j[2]) * cos(j[3]) * sin(j[1])) + cos(j[4]) * sin(j[1]) * sin(j[2])) + cos(j[6]) * (cos(j[5]) * (cos(j[4]) * (cos(j[1]) * sin(j[3]) - cos(j[2]) * cos(j[3]) * sin(j[1])) - sin(j[1]) * sin(j[2]) * sin(j[4])) - sin(j[5]) * (cos(j[1]) * cos(j[3]) + cos(j[2]) * sin(j[1]) * sin(j[3]))),
//        cos(j[6])* (sin(j[4]) * (cos(j[1]) * sin(j[3]) - cos(j[2]) * cos(j[3]) * sin(j[1])) + cos(j[4]) * sin(j[1]) * sin(j[2])) - sin(j[6]) * (cos(j[5]) * (cos(j[4]) * (cos(j[1]) * sin(j[3]) - cos(j[2]) * cos(j[3]) * sin(j[1])) - sin(j[1]) * sin(j[2]) * sin(j[4])) - sin(j[5]) * (cos(j[1]) * cos(j[3]) + cos(j[2]) * sin(j[1]) * sin(j[3]))),
//        -sin(j[5]) * (cos(j[4]) * (cos(j[1]) * sin(j[3]) - cos(j[2]) * cos(j[3]) * sin(j[1])) - sin(j[1]) * sin(j[2]) * sin(j[4])) - cos(j[5]) * (cos(j[1]) * cos(j[3]) + cos(j[2]) * sin(j[1]) * sin(j[3])),
//        (2.0 * cos(j[1]) * cos(j[3])) / 5.0 - (21.0 * cos(j[1])) / 50.0 - (63.0 * sin(j[5]) * (cos(j[4]) * (cos(j[1]) * sin(j[3]) - cos(j[2]) * cos(j[3]) * sin(j[1])) - sin(j[1]) * sin(j[2]) * sin(j[4]))) / 500.0 - (63.0 * cos(j[5]) * (cos(j[1]) * cos(j[3]) + cos(j[2]) * sin(j[1]) * sin(j[3]))) / 500.0 + (2.0 * cos(j[2]) * sin(j[1]) * sin(j[3])) / 5.0 + 9.0 / 25.0,
//        0.0, 0.0, 0.0, 1.0;
//
//    return T;
//}
//
//    printf("j[0]:%6.4lf\tj[1]:%6.4lf\tj[2]:%6.4lf\tj[3]:%6.4lf\tj[4]:%6.4lf\tj[5]:%6.4lf\tj[6]:%6.4lf\n\n",
//    _joint[0], _joint[1], _joint[2], _joint[3], _joint[4], _joint[5], _joint[6]);
//
//    eta = atan2(_T(2, 1) , _T(1, 1));
//    theta = atan2(-_T(3, 1), (sqrt(_T(3, 2) * _T(3, 2) + _T(3, 3) * _T(3, 3))));
//    phi = atan2(_T(3, 2), _T(3, 3));
//    printf("eta:%5.1lf\tphi:%5.1lf\ttheta:%5.1lf\n", RAD2DEG*eta, RAD2DEG*phi, RAD2DEG*theta);
//
//    _R << cos(theta) * cos(eta), sin(phi) * sin(theta) * cos(eta) - cos(phi) * sin(eta), cos(phi) * sin(theta) * cos(eta) + sin(phi) * sin(eta),
//        cos(theta) * sin(eta), sin(phi) * sin(theta) * sin(eta) + cos(phi) * cos(eta), cos(phi) * sin(theta) * sin(eta) - sin(phi) * sin(eta),
//        -sin(theta), sin(phi)* cos(theta), cos(phi)* cos(theta);
//    std::cout <<  _R << std::endl;
#endif

