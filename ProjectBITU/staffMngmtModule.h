#ifndef STAFFMNGMTMODULE_H
#define STAFFMNGMTMODULE_H

#include <string>

using namespace std;


class StaffMngmtModule{

public: 

	void staffMenu();
	void staffRegistration();
	void staffListView();
	void staffSearch();
	void staffUpdateorRemove(string* userData);
	void staffUpdate(string* userData);
	void staffUpdateIC(string* userData);
	void staffUpdateName(string* userData);
	void staffUpdatePhone(string* userData);
	void staffUpdateRole(string* userData);
	void staffRemove(string* userData);
};
#endif // !STAFFMNGMTMODULE_H
