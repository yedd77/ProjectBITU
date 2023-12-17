#ifndef MEDMGMNTSUB_H
#define MEDMGMNTSUB_H

#include <string>

using namespace std;

class MedMgmntSub {

public :

	void MedsMgmntMenu();
	void MedsMgmntAdd();
	void medsMgmntView();
	void medsSearch();
	void medsUpdate(string* medsData);
	void medsUpdatePrice(string* medsData);
	void medsUpdateDesc(string* medsData);
	void medsRemove(string* medsData);

	void batchMgmntAdd(string* medsData);
};
#endif // !MEDMGMNTSUB_H
