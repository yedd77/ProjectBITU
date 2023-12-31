#ifndef RESIDENTMGMNTMODULE_H
#define RESIDENTMGMNTMODULE_H

#include <string>

using namespace std;

class ResidentMgmntModule {

public:
	void residentMenu();
	void residentRegistration();
	void residentListView();
	void updateResident(string resID);
};
#endif // !RESIDENTMGMNTMODULE_H
