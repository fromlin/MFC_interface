#include "VegaWork.h"

CVegaWork::CVegaWork(std::string name)
	:CWorkBase(name)
{
	capi = CombinedApi();
	apiSupportsBX2 = false;
	//memset(&_pad, 0, sizeof(PArtecData_t));
	_memname_data = name + "_Data";

	//CREATE_SYSTEM_MEMORY(_memname_data, ArtecData_t);
}

CVegaWork::~CVegaWork() {

	//DELETE_SYSTEM_MEMORY(_memname_data);

	ClosePort();
}

bool CVegaWork::OpenPort() {

	if (__argc < 2 || __argc > 3) {
		std::cout << "CAPIsample Ver " << capi.getVersion() << std::endl;
		return false;
	}

	std::string hostname = std::string(__argv[1]);
	std::string scu_hostname = (__argc == 3) ? std::string(__argv[2]) : "";

	if (capi.connect(hostname) != 0) {
		std::cout << "Connection Failed!" << std::endl;
		std::cout << "Press Enter to continue...";
		std::cin.ignore();
		return false;
	}
	std::cout << "Connected!" << std::endl;
	// Determine if the connected device supports the BX2 command
	determineApiSupportForBX2();

	// Initialize the system. This clears all previously loaded tools, unsaved settings etc...
	onErrorPrintDebugMessage("capi.initialize()", capi.initialize());
	onErrorPrintDebugMessage("capi.startTracking()", capi.startTracking());
	std::cout << capi.getTrackingDataTX(TrackingReplyOption::ToolAndMarkerData) << std::endl;
	// -> TX 0002 -> 2+20 byte reply
	onErrorPrintDebugMessage("capi.stopTracking()", capi.stopTracking());
	configureUserParameters();

	// Various tool types are configured in slightly different ways
	configurePassiveTools();

	// Once loaded or detected, tools are initialized and enabled the same way
	initializeAndEnableTools();

	return true;
}

void CVegaWork::ClosePort() {

	// Give the user a chance to view the output in the terminal before exiting
	std::cout << "CAPI demonstration complete." << std::endl;
	capi.~CombinedApi();
}

void CVegaWork::_execute() {

	// Write a CSV file
	trackingPassiveTools();
}


void CVegaWork::captureOneFrame() {

}





void CVegaWork::sleepSeconds(unsigned numSeconds) {
	Sleep((DWORD)1000 * numSeconds); // Sleep(ms)
}

void CVegaWork::onErrorPrintDebugMessage(std::string methodName, int errorCode) {
	if (errorCode < 0) {
		std::cout << methodName << " failed: " << capi.errorToString(errorCode) << std::endl;
	}
}

std::string CVegaWork::getToolInfo(std::string toolHandle) {
	// Get the port handle info from PHINF
	PortHandleInfo info = capi.portHandleInfo(toolHandle);

	// Return the ID and SerialNumber the desired string format
	std::string outputString = info.getToolId();
	outputString.append(" s/n:").append(info.getSerialNumber());
	return outputString;
}

void CVegaWork::singularitycheck(int group, double theta) {
	if (group == 1 && (M_PI - theta < M_PI / 180. || theta < M_PI / 180.)) {
		printf("singularity check failed: %f || %f < %f rad\n", M_PI - theta, theta,
			M_PI / 180.);
	}
	else if (group == 2 && fabs(theta - M_PI / 2) < M_PI / 180) {
		printf("singularity check failed: %f < %f rad\n", fabs(theta - M_PI / 2),
			M_PI / 180.);
	}
}

void CVegaWork::quaternionToEuler(std::stringstream& stream, const ToolData& toolData) {
	double psi, theta, phi;

	// XYZ
	psi = atan2(2. * (toolData.transform.qx * toolData.transform.q0 - toolData.transform.qy * toolData.transform.qz), (toolData.transform.q0 * toolData.transform.q0 - toolData.transform.qx * toolData.transform.qx - toolData.transform.qy * toolData.transform.qy + toolData.transform.qz * toolData.transform.qz));
	theta = asin(2. * (toolData.transform.qx * toolData.transform.qz + toolData.transform.qy * toolData.transform.q0));
	phi = atan2(2. * (toolData.transform.qz * toolData.transform.q0 - toolData.transform.qx * toolData.transform.qy), (toolData.transform.q0 * toolData.transform.q0 + toolData.transform.qx * toolData.transform.qx - toolData.transform.qy * toolData.transform.qy - toolData.transform.qz * toolData.transform.qz));
	singularitycheck(2, theta);

	//// ZYX
	//psi =  atan2(2. * (toolData.transform.qx * toolData.transform.qy + toolData.transform.qz * toolData.transform.q0),    (toolData.transform.q0 * toolData.transform.q0 + toolData.transform.qx * toolData.transform.qx - toolData.transform.qy * toolData.transform.qy - toolData.transform.qz * toolData.transform.qz));
	//theta = asin(2. * (toolData.transform.qy * toolData.transform.q0 - toolData.transform.qx * toolData.transform.qz));
	//phi =  atan2(2. * (toolData.transform.qx * toolData.transform.q0 + toolData.transform.qz * toolData.transform.qy),    (toolData.transform.q0 * toolData.transform.q0 - toolData.transform.qx * toolData.transform.qx - toolData.transform.qy * toolData.transform.qy + toolData.transform.qz * toolData.transform.qz));
	//singularitycheck(2, theta);

	stream << "Euler:" << std::endl;
	stream << RAD_DEGREE(psi) << ", " << RAD_DEGREE(theta) << ", " << RAD_DEGREE(phi) << std::endl;
}

std::string CVegaWork::toolTrackingData(const ToolData& toolData) {
	std::stringstream stream;
	//stream << std::setprecision(toolData.PRECISION) << std::setfill('0');
	//stream << "FrameNumber" << static_cast<unsigned>(toolData.frameNumber) << std::endl;
	stream << "Port:" << static_cast<unsigned>(toolData.transform.toolHandle) << std::endl;
	//stream << "FaceNumber:" << static_cast<unsigned>(toolData.transform.getFaceNumber()) << std::endl;
	stream << "Marker:" << toolData.markers.size() << "   Status:";

	if (toolData.transform.isMissing())	{
		stream << "Missing..." << std::endl;
	}
	else {
		stream << TransformStatus::toString(toolData.transform.getErrorCode()) << std::endl;
		// quaternion -> euler
		stream << toolData.transform.q0 << ", " << toolData.transform.qx << ", " << toolData.transform.qy << ", " << toolData.transform.qz << std::endl;
		quaternionToEuler(stream, toolData);

		// global coordinate (x,y,z)
		stream << toolData.transform.tx << ", " << toolData.transform.ty << ", " << toolData.transform.tz << ", " << toolData.transform.error << std::endl;
	}

	//for (int i = 0; i < toolData.markers.size(); i++)
	//{
	//	stream << MarkerStatus::toString(toolData.markers[i].status);
	//	if (toolData.markers[i].status == MarkerStatus::Missing)
	//	{
	//		stream << "...   ";
	//	}
	//	else
	//	{
	//		stream << toolData.markers[i].x << ", " << toolData.markers[i].y << ", " << toolData.markers[i].z;
	//	}
	//	stream << "   ";
	//}
	return stream.str();
}

void CVegaWork::printToolData(const ToolData& toolData) {
	if (toolData.systemAlerts.size() > 0) {
		std::cout << "[" << toolData.systemAlerts.size() << " alerts] ";
		for (int a = 0; a < toolData.systemAlerts.size(); a++) {
			std::cout << toolData.systemAlerts[a].toString() << std::endl;
		}
	}

	if (toolData.buttons.size() > 0) {
		std::cout << "[buttons: ";
		for (int b = 0; b < toolData.buttons.size(); b++) {
			std::cout << ButtonState::toString(toolData.buttons[b]) << " ";
		}
		std::cout << "] ";
	}
	std::cout << toolTrackingData(toolData) << std::endl;
}

void CVegaWork::printTrackingData() {
	// Start tracking, output a few frames of data, and stop tracking
	std::cout << std::endl << "Entering tracking mode and collecting data..." << std::endl;
	onErrorPrintDebugMessage("capi.startTracking()", capi.startTracking());

	for (int i = 0; i < 3; i++) {
		// Demonstrate TX command: ASCII command sent, ASCII reply received
		std::cout << capi.getTrackingDataTX() << std::endl;

		// Demonstrate BX or BX2 command
		std::vector<ToolData> toolData = apiSupportsBX2 ? capi.getTrackingDataBX2("--6d=tools --3d=tools --sensor=none --1d=buttons") :
			capi.getTrackingDataBX(TrackingReplyOption::TransformData | TrackingReplyOption::AllTransforms);

		// Print to stdout in similar format to CSV
		std::cout << "[alerts] [buttons] Frame#,ToolHandle,Face#,TransformStatus,Q0,Qx,Qy,Qz,Tx,Ty,Tz,Error,#Markers,State,Tx,Ty,Tz" << std::endl;
		for (int i = 0; i < toolData.size(); i++) {
			printToolData(toolData[i]);
		}
	}

	// Stop tracking (back to configuration mode)
	std::cout << std::endl << "Leaving tracking mode and returning to configuration mode..." << std::endl;
	onErrorPrintDebugMessage("capi.stopTracking()", capi.stopTracking());
}

void CVegaWork::initializeAndEnableTools() {
	std::cout << std::endl << "Initializing and enabling tools..." << std::endl;

	// Initialize and enable tools
	std::vector<PortHandleInfo> portHandles = capi.portHandleSearchRequest(PortHandleSearchRequestOption::NotInit);
	for (int i = 0; i < portHandles.size(); i++) {
		onErrorPrintDebugMessage("capi.portHandleInitialize()", capi.portHandleInitialize(portHandles[i].getPortHandle()));
		onErrorPrintDebugMessage("capi.portHandleEnable()", capi.portHandleEnable(portHandles[i].getPortHandle()));
	}

	// Print all enabled tools
	portHandles = capi.portHandleSearchRequest(PortHandleSearchRequestOption::Enabled);
	for (int i = 0; i < portHandles.size(); i++) {
		std::cout << portHandles[i].toString() << std::endl;
	}
}

void CVegaWork::loadTool(const char* toolDefinitionFilePath) {
	// Request a port handle to load a passive tool into
	int portHandle = capi.portHandleRequest();
	onErrorPrintDebugMessage("capi.portHandleRequest()", portHandle);

	// Load the .rom file using the previously obtained port handle
	capi.loadSromToPort(toolDefinitionFilePath, portHandle);
}

void CVegaWork::configurePassiveTools() {
	// Load a few passive tool definitions from a .rom files
	std::cout << std::endl << "Configuring Passive Tools - Loading .rom Files..." << std::endl;
	loadTool("C:/Users/YeLin/OneDrive - SNU/Desktop/vega_vt/visual_studio/library_vega/vega/sroms/8700340.rom");		// pointer
	loadTool("C:/Users/YeLin/OneDrive - SNU/Desktop/vega_vt/visual_studio/library_vega/vega/sroms/8700449.rom");		// camera
}

void CVegaWork::configureActiveTools(std::string scuHostname) {
	// Setup the SCU connection for demonstrating active tools
	std::cout << std::endl << "Configuring Active Tools - Setup SCU Connection" << std::endl;
	onErrorPrintDebugMessage("capi.setUserParameter()", capi.setUserParameter("Param.Connect.SCU Hostname", scuHostname));
	std::cout << capi.getUserParameter("Param.Connect.SCU Hostname") << std::endl;

	// Wait a few seconds for the SCU to detect any wired tools plugged in
	std::cout << std::endl << "Demo Active Tools - Detecting Tools..." << std::endl;
	sleepSeconds(2);

	// Print all port handles
	std::vector<PortHandleInfo> portHandles = capi.portHandleSearchRequest(PortHandleSearchRequestOption::NotInit);
	for (int i = 0; i < portHandles.size(); i++) {
		std::cout << portHandles[i].toString() << std::endl;
	}
}

void CVegaWork::configureActiveWirelessTools() {
	// Load an active wireless tool definitions from a .rom files
	std::cout << std::endl << "Configuring an Active Wireless Tool - Loading .rom File..." << std::endl;
	loadTool("sroms/active-wireless.rom");
}

void CVegaWork::configureDummyTools() {
	std::cout << std::endl << "Loading passive, active-wireless, and active dummy tools..." << std::endl;
	onErrorPrintDebugMessage("capi.loadPassiveDummyTool()", capi.loadPassiveDummyTool());
	onErrorPrintDebugMessage("capi.loadActiveWirelessDummyTool()", capi.loadActiveWirelessDummyTool());
	onErrorPrintDebugMessage("capi.loadActiveDummyTool()", capi.loadActiveDummyTool());
}

void CVegaWork::configureUserParameters() {
	//std::cout << capi.getUserParameter("Param.User.String0") << std::endl;
	//onErrorPrintDebugMessage("capi.setUserParameter(Param.User.String0, customString)", capi.setUserParameter("Param.User.String0", "customString"));
}

void CVegaWork::simulateAlerts(uint32_t simulatedAlerts) {
	// Simulate alerts if any were requested
	if (simulatedAlerts > 0x0000) {
		std::cout << std::endl << "Simulating system alerts..." << std::endl;
		std::stringstream stream;
		stream << simulatedAlerts;
		onErrorPrintDebugMessage("capi.setUserParameter(Param.Simulated Alerts, alerts)", capi.setUserParameter("Param.Simulated Alerts", stream.str()));
		std::cout << capi.getUserParameter("Param.Simulated Alerts") << std::endl;
	}
}

void CVegaWork::determineApiSupportForBX2() {
	// Lookup the API revision
	std::string response = capi.getApiRevision();

	// Refer to the API guide for how to interpret the APIREV response
	char deviceFamily = response[0];
	int majorVersion = capi.stringToInt(response.substr(2, 3));

	// As of early 2017, the only NDI device supporting BX2 is the Vega
	// Vega is a Polaris device with API major version 003
	if (deviceFamily == 'G' && majorVersion >= 3) {
		apiSupportsBX2 = true;
	}
}

void CVegaWork::trackingPassiveTools() {
	// Assumption: tools are not enabled/disabled during CSV output
	std::vector<PortHandleInfo> portHandles = capi.portHandleSearchRequest(PortHandleSearchRequestOption::Enabled);
	if (portHandles.size() < 1) {
		std::cout << "No tools are enabled!" << std::endl;
		return;
	}

	// Lookup and store the serial number for each enabled tool
	std::vector<ToolData> enabledTools;
	for (int i = 0; i < portHandles.size(); i++) {
		enabledTools.push_back(ToolData());
		enabledTools.back().transform.toolHandle = (uint16_t)capi.stringToInt(portHandles[i].getPortHandle());
		enabledTools.back().toolInfo = getToolInfo(portHandles[i].getPortHandle());
	}

	// Start tracking
	std::cout << std::endl << "Entering tracking mode..." << std::endl;
	onErrorPrintDebugMessage("capi.startTracking()", capi.startTracking());

	// 

	// Loop to gather tracking data and write to the file
	int trackingCount = 0;
	int previousFrameNumber = 0; // use this variable to avoid printing duplicate data with BX
	while (1) {
		// Get new tool data using BX2
		std::vector<ToolData> newToolData = apiSupportsBX2 ? capi.getTrackingDataBX2("--6d=tools --3d=tools --sensor=none --1d=buttons") :
			capi.getTrackingDataBX(TrackingReplyOption::TransformData | TrackingReplyOption::AllTransforms);

		// Update enabledTools array with new data
		for (int t = 0; t < enabledTools.size(); t++) {
			for (int j = 0; j < newToolData.size(); j++) {
				if (enabledTools[t].transform.toolHandle == newToolData[j].transform.toolHandle) {
					// Copy the new tool data
					newToolData[j].toolInfo = enabledTools[t].toolInfo; // keep the serial number
					enabledTools[t] = newToolData[j]; // use the new data
				}
			}
		}

		if (apiSupportsBX2) {
			// Count the number of tools that have new data
			int newDataCount = 0;
			for (int t = 0; t < enabledTools.size(); t++) {
				if (enabledTools[t].dataIsNew) {
					newDataCount++;
				}
			}

			// Send another BX2 if some tools still have old data
			if (newDataCount < enabledTools.size()) {
				continue;
			}
		}
		else {
			if (previousFrameNumber == enabledTools[0].frameNumber)	{
				// If the frame number didn't change, don't print duplicate data to the CSV, send another BX
				continue;
			}
			else {
				// This frame number is different, so we'll print a line to the CSV, but remember it for next time
				previousFrameNumber = enabledTools[0].frameNumber;
			}
		}

		std::cout << std::endl;
		for (int t = 0; t < enabledTools.size(); t++) {
			// "#Tools,ToolInfo,Frame#,PortHandle,Face#,TransformStatus,Q0,Qx,Qy,Qz,Tx,Ty,Tz,Error,Markers,State,Tx,Ty,Tz" << std::endl;
			(t == 0) ? std::cout << ">> camera" << std::endl : std::cout << ">> pointer" << std::endl;
			std::cout << "Quaternion : " << enabledTools[t].transform.q0 << "\t" << enabledTools[t].transform.qx << "\t" << enabledTools[t].transform.qy << "\t" << enabledTools[t].transform.qz << std::endl;
			std::cout << "Transform  : " << enabledTools[t].transform.tx << "\t" << enabledTools[t].transform.ty << "\t" << enabledTools[t].transform.tz << std::endl;
			//std::cout << toolTrackingData(enabledTools[t]) << std::endl;
			enabledTools[t].dataIsNew = false; // once printed, the data becomes "old"
		}
		//std::cout << std::endl;
		trackingCount++;
	}

	// Stop tracking and return to configuration mode
	onErrorPrintDebugMessage("capi.stopTracking()", capi.stopTracking());
}