#pragma once

#include <list>
#include "process.h"

class ProcessGroup
{
public:
	ProcessGroup();
	Process& appendProcess();
	int execAndWait();
private:
	list<Process> mProcessList;
};

