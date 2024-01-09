#ifndef STORAGEMGMNTSUB_H
#define STORAGEMGMNTSUB_H

#include <string>

using namespace std;

class StorageMgmntSub {

public:

	void StorageMgmntMenu();
	string getMedicineBatchesID();
	string getMedicineID();
	void batchMgmntAdd(string medsID);
	void viewMedicineBatches();
	void StorageMgmntUpdateBatches(string ID);
	void StorageMgmntUpdateBatchesQuantity(string ID, string oldQty);
	void StorageMgmntUpdateBatchesExpiry(string ID, string oldQty);
	void StorageMgmntDeleteBatches(string ID);
	void splitMedicineBatches();

};
#endif // !STORAGEMGMNTSUB_H
