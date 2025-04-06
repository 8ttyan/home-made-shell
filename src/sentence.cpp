
#include <sys/wait.h>
#include "sentence.h"

Sentence::Sentence()
: mWaitType(WaitType::ForeGround)
{
}
ProcessGroup& Sentence::appendProcessGroup()
{
	if ( mPGList.size()==mCondList.size()+1 ) {
		mCondList.push_back( Condition::OR );
	}
	mPGList.push_back( ProcessGroup() );
	return mPGList.back();
}
void Sentence::appendCondition(Sentence::Condition pCondition)
{
	if ( mPGList.size()-1==mCondList.size() ) {
		mCondList.push_back( pCondition );
	}
}
void Sentence::setWaitType(WaitType pWaitType)
{
	mWaitType = pWaitType;
}
int Sentence::exec()
{
	int status=0;
	auto condItr = mCondList.begin();
	for (auto& procGrp : mPGList) {
		status = procGrp.execAndWait();
		//printf("status:%d\n",status);
		//printf("WEXITSTATUS:%d\n",WEXITSTATUS(status));
		int exitcode = WEXITSTATUS(status);
		if ( condItr==mCondList.end() ) break;
		Condition cond = *(condItr++);
		if ( cond==Condition::AND ) {
			if ( exitcode==0 ) {
				continue;
			} else {
				break;
			}
		} else {
			if ( exitcode==0 ) {
				break;
			} else {
				continue;
			}
		}
	}
	return status;
}

