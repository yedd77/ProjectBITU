#ifndef PREMGMNTDUBMODULE_H
#define PREMGMNTDUBMODULE_H
#include <string>
#include <iostream>
#include <utility>

using namespace std;

class PreMgmntSubModule {

public:
	void preMgmntMenu();
	void addPrescription();
	void removePrescription();
	void removePrescription(string resID, string preID);
	string getResidentID(string resID);
	string getPrescriptionID(string action, string resID);
	void viewPrescription();
};
#endif // !PREMGMNTDUBMODULE_H
