#ifndef STAFFMNGMTMODULE_H
#define STAFFMNGMTMODULE_H

#include <string>

using namespace std;


class StaffMngmtModule{

public: 

	void staffMenu();
	void staffRegistration();
	void staffListView();
	string* staffSearch();	
	void staffUpdate(string* userData);
	void staffUpdateIC(string* userData);
	void staffUpdateName(string* userData);
	void staffUpdatePhone(string* userData);
	void staffRemove(string* userData);
	void staffNextAction(string* userData);
	void staffGeneralSearch();
};
#endif // !STAFFMNGMTMODULE_H
