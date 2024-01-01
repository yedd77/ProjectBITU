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
	void updateResidentName(string resID);
	void updateResidentRoomNum(string resID);
	void updateGuardianName(string resID);
	void updateGuardianContact(string resID);
	void removeResident(string resID);
};
#endif // !RESIDENTMGMNTMODULE_H
