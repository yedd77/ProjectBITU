#ifndef MEDMGMNTSUB_H
#define MEDMGMNTSUB_H

#include <string>

using namespace std;

class MedMgmntSub {

public :

	void MedsMgmntMenu();
	void MedsMgmntAdd();
	void medsMgmntView();
	string* medsSearch();
	void medsUpdate(string* medsData);
	void medsUpdatePrice(string* medsData);
	void medsUpdateDesc(string* medsData);
	void medsRemove(string* medsData);
	void medsNextAction(string* medsData);
	void medsGeneralSearch();
};
#endif // !MEDMGMNTSUB_H
