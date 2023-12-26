#ifndef STORAGEMGMNTSUB_H
#define STORAGEMGMNTSUB_H

#include <string>

using namespace std;

class StorageMgmntSub {

public:

	void StorageMgmntMenu();
	string getMedicineBatchesID();
	void StorageMgmntUpdateBatches(string ID);
	void StorageMgmntUpdateBatchesQuantity(string ID, string oldQty);
	void StorageMgmntUpdateBatchesExpiry(string ID, string oldQty);
	void StorageMgmntDeleteBatches(string ID);
	/*void StorageMgmntView();
	void StorageSearch();
	void StorageUpdate(string* StorageData);
	void StorageUpdatePrice(string* StorageData);
	void StorageUpdateDesc(string* StorageData);
	void StorageRemove(string* StorageData);*/

};
#endif // !STORAGEMGMNTSUB_H
