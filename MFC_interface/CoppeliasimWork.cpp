#include "CoppeliasimWork.h"

CCoppeliasimWork::CCoppeliasimWork(std::string name)
	:CWorkBase(name)
{
	//memset(&_data, 0, sizeof(Data_t));
	_memname_data = name + "_Data";

	CREATE_SYSTEM_MEMORY(_memname_data, ArtecData_t);
}

CCoppeliasimWork::~CCoppeliasimWork() {

	DELETE_SYSTEM_MEMORY(_memname_data);

	ClosePort();
}

bool CCoppeliasimWork::OpenPort() {

	client.setStepping(true);
	client.call("sim.startSimulation", nullptr);



	return true;
}
 
void CCoppeliasimWork::ClosePort() {

	client.call("sim.stopSimulation", nullptr);
}

void CCoppeliasimWork::_execute() {

	t = client.call("sim.getSimulationTime")[0].as<float>();
	printf("Simulation time: %.2f [s]\n", t);
	client.step();
}