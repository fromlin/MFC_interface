//////////////////////////////////////////////////////////////////////////////////////////////////////////
// File		: TouchWork.cpp
// Version	: 1.0.1
// Date		: 2022.08.31
// Writer	: Kim, YeLin (MRAS)
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TouchWork.h"

std::string _memname_temp;
TouchData_t _touch;
TouchData_t _touch_temp;
TouchData_t _touch_temp2;
HDCallbackCode HDCALLBACK FrictionlessPlaneCallback(void* data);

CTouchWork::CTouchWork(std::string name)
	:CWorkBase(name)
{
    _memname_touch = name + "_Data";
    CREATE_SYSTEM_MEMORY(_memname_touch, TouchData_t);

    _memname_temp = _memname_touch;
}

CTouchWork::~CTouchWork() {

	DELETE_SYSTEM_MEMORY(_memname_touch);

	ClosePort();
}

bool CTouchWork::OpenPort() {

    HDErrorInfo error;
    HDSchedulerHandle hGravityWell;

    hHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        return false;
    }
    else {
        // Start the servo scheduler and enable forces.
        hdEnable(HD_FORCE_OUTPUT);
        hdStartScheduler();
        if (HD_DEVICE_ERROR(error = hdGetError())) {
            hduPrintError(stderr, &error, "Failed to start the scheduler");
            return false;
        }

        hPlaneCallback = hdScheduleAsynchronous(FrictionlessPlaneCallback, 0, HD_DEFAULT_SCHEDULER_PRIORITY);
        return true;
    }
}

void CTouchWork::ClosePort() {

    hdStopScheduler();
    hdUnschedule(hPlaneCallback);
    hdDisableDevice(hHD);
}

void CTouchWork::_execute() {

    if (!hdWaitForCompletion(hPlaneCallback, HD_WAIT_CHECK_STATUS)) {
        fprintf(stderr, "\nThe main scheduler callback has exited\n");
        return;
    }
}

HDCallbackCode HDCALLBACK FrictionlessPlaneCallback(void* data) {

    HDint buttons;
    hduVector3Dd position, orientation;
    hduVector3Dd force, center;

    /**
      * X = Right(+), Left(-)
      * Y = Up(+), Down(-)
      * Z = To User(+), Away From User(-)
      */

    /* TODO: between beginFrame and endFrame */
    hdBeginFrame(hdGetCurrentDevice());
    hdGetDoublev(HD_CURRENT_POSITION, position);
    hdGetIntegerv(HD_CURRENT_BUTTONS, &buttons);
    hdGetDoublev(HD_CURRENT_GIMBAL_ANGLES, orientation);


    _touch.state = buttons;
    switch (buttons) {
    case 0:
        for (int i = 0; i < 3; i++) {
            _touch_temp.pos[i] = position[i];
            //_touch_temp.ori[i] = (orientation[i] * RAD2DEG);
        }
        //if (center[1] > position[1]) {
        //    force[1] = (center[1] - position[1]) * SPRING_K;
        //}
        break;

    case 1:
        hduVecSet(force, 0., 0., 0.);
        for (int i = 0; i < 3; i++) {
            _touch.pos[i] = position[i] - _touch_temp.pos[i];
            //_touch.ori[i] = (orientation[i] * RAD2DEG) - _touch_temp.ori[i];
        }
        center = position;
        break;

    case 2:
        break;
    }

    hdSetDoublev(HD_CURRENT_FORCE, force);
    hdEndFrame(hdGetCurrentDevice());


    // In case of error, terminate the callback.
    HDErrorInfo error;
    if (HD_DEVICE_ERROR(error = hdGetError())) {
        hduPrintError(stderr, &error, "Error detected during main scheduler callback\n");

        if (hduIsSchedulerError(&error)) {
            return HD_CALLBACK_DONE;
        }
    }

    SET_SYSTEM_MEMORY(_memname_temp, _touch);

    return HD_CALLBACK_CONTINUE;
}