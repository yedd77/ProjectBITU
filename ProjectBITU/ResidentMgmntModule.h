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
	void residentIncompleteSchedule();
	void residentCompleteSchedule();
	void updateSchedule(string sceheduleID); //background process
	void updateScheduleRollback(string sceheduleID); //background process
	void updateMedsStock(string scheduleID); //background process
};
#endif // !RESIDENTMGMNTMODULE_H
