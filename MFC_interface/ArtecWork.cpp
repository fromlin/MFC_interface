//////////////////////////////////////////////////////////////////////////////////////////////////////////
// File		: ArtecWork.h
// Version	: 1.0.1
// Date		: 2022.11.04
// Writer	: Kim, YeLin (MRAS)
//////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ArtecWork.h"

#ifndef  _M_IX86

CArtecWork::CArtecWork(std::string name)
	:CWorkBase(name)
{
	num = 0;
	memset(&_pad, 0, sizeof(PArtecData_t));
	_memname_data = name + "_Data";

	CREATE_SYSTEM_MEMORY(_memname_data, ArtecData_t);
}

CArtecWork::~CArtecWork() {

	DELETE_SYSTEM_MEMORY(_memname_data);

	ClosePort();
}

bool CArtecWork::OpenPort() {

    asdk::setOutputLevel(asdk::VerboseLevel_Warning);
    TRef<asdk::IArrayScannerId> scannersList;
    std::wcout << L"Enumerating scanners... ";
    ec = asdk::enumerateScanners(&scannersList);
    if (ec != asdk::ErrorCode_OK) {
        std::wcout << L"failed" << std::endl;
        return false;
    }
    std::wcout << L"done" << std::endl;
    int scanner_count = scannersList->getSize();
    if (scanner_count == 0) {
        std::wcout << L"No scanners found" << std::endl;
        return false;
    }

    const asdk::ScannerId* idArray = scannersList->getPointer();
    const asdk::ScannerId& defaultScanner = idArray[0];                         // just take the first available scanner
    std::wcout
        << L"Connecting to " << asdk::getScannerTypeName(defaultScanner.type)
        << L" scanner " << defaultScanner.serial << L"... "
        ;
    ec = asdk::createScanner(&scanner, &defaultScanner);
    if (ec != asdk::ErrorCode_OK) {
        std::wcout << L"failed" << std::endl;
        return false;
    }
    std::wcout << L"done" << std::endl;

    return true;
}

void CArtecWork::ClosePort() {

    scanner = NULL;
    std::wcout << L"Scanner released" << std::endl;
}

void CArtecWork::_execute() {

}

void CArtecWork::captureOneFrame() {
	std::wcout << L"Capturing frame... ";
	TRef<asdk::IFrame> frame;
	TRef<asdk::IFrameMesh> mesh;
	TRef<asdk::IFrameProcessor> processor;

	ec = scanner->createFrameProcessor(&processor);
	/*
	processor->setROI(asdk::RectF);*/
	processor->setScanningRange(170.0, 256.0);		// 170.0 ~ 300.0
	processor->setSensitivity(0.75);					//	 0.0 ~   1.0

	if (ec == asdk::ErrorCode_OK) {
		frame = NULL;
		ec = scanner->capture(&frame, true);

		num++;
		TRef<asdk::IImage> img;
		scanner->convertTextureFull(&img, frame->getTexture());
		wchar_t wcsbuf_img[50];
		swprintf(wcsbuf_img, 50, L"%s[%d].png", L"data/img/texture", num);
		ec = asdk::io::saveImageToFile(wcsbuf_img, img);

		if (ec == asdk::ErrorCode_OK) {
			mesh = NULL;
			ec = processor->reconstructAndTexturizeMesh(&mesh, frame);
			if (ec == asdk::ErrorCode_OK) {
				wchar_t wcsbuf_ply[50];
				swprintf(wcsbuf_ply, 50, L"%s[%d].ply", L"data/ply/pcframe", num);
				ec = asdk::io::Ply::save(wcsbuf_ply, mesh);

				int row = mesh->getPoints()->getSize();
				int col = 3;
				_pad = (PArtecData_t)malloc(sizeof(ArtecData_t) * row);
				memcpy(_pad, mesh->getPoints()->getPointer(), sizeof(float) * row * col);

				/*
				DELETE_SYSTEM_MEMORY(_memname_data);
				CREATE_SYSTEM_MEMORY(_memname_data, _pad);	// "Artec_Data"
				SET_SYSTEM_MEMORY("ArtecWork_Data", _pad);
				*/

				//for (int r = 0; r < row; r++) {
				//	for (int c = 0; c < col; c++) {
				//		printf("%.4f, ", _pad[r].data[c]);
				//	}
				//	printf("\n");
				//}
				//printf("\n");
				free(_pad);

				/*
				 working with normals
				 1. generate normals
				 2. get normals array using helper class
				 3. get number of normals
				 4. use normal
				*/
				mesh->calculate(asdk::CM_Normals);
				asdk::TArrayPoint3F pointsNormals = mesh->getPointsNormals();
				int normalCount = pointsNormals.size();
				ASDK_UNUSED(normalCount);
				asdk::Point3F point = pointsNormals[0];
				ASDK_UNUSED(point);
				std::wcout << L"Captured mesh saved to disk" << std::endl;
			}
			else {
				std::wcout << L"failed" << std::endl;
			}
		}
	}
}
#endif