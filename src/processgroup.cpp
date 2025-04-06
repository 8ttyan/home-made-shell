
#include "processgroup.h"

ProcessGroup::ProcessGroup()
{
}
Process& ProcessGroup::appendProcess()
{
	Process* prev = NULL;
	if ( mProcessList.size()>0 ) prev=&mProcessList.back();
	mProcessList.push_back( Process() );
	if (prev) prev->connectByPipe( mProcessList.back() );
	return mProcessList.back();
}
int ProcessGroup::execAndWait()
{
	int pgid=0;
	for (auto& proc : mProcessList) {
		int pid = proc.forkExec(pgid);
		if ( pgid==0 ) pgid=pid;
	}
	int status=0;
	for (auto& proc : mProcessList) {
		status = proc.wait();
	}
	return status;
}

