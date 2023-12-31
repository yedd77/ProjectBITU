#include "StorageMgmntSub.h"
#include "Artwork.h"
#include "MedStrgModule.h"
#include "Auth.h"
#include <iostream>
#include "table.h"
#include "Misc.h"

using namespace std;

extern Artwork art;
extern Auth auth;
extern Misc misc;
extern MedStrgModule medStrgModule;

//function to display the storage management sub-module menu
void StorageMgmntSub::StorageMgmntMenu() {

	system("cls");
	art.logoArt();
	art.directoryArt("MSMM/Storage Management sub-module");

	cout << "\x1B[94mPlease select your next action\033[0m\n\n";

	cout << "1 - Update medicine's batches\n";
	cout << "2 - Delete medicine's batches\n";
	cout << "3 - Split medicine's dosage\n";
	cout << "4 = Go back to previous menu\n";
	cout << "5 - Log out\n";

	cout << "\nPlease enter your choice : ";

	int option;
	cin >> option;

	if (cin.fail()) {
		cin.clear();
		cin.ignore();
		cout << "\n\x1B[31mInvalid input, please try again\033[0m\n";
		system("pause");
		StorageMgmntMenu();
	}
	else {
		switch (option) {
		case 1:
			StorageMgmntUpdateBatches(getMedicineBatchesID());
			break;
		case 2:
			StorageMgmntDeleteBatches(getMedicineBatchesID());
			break;
		case 3:
			splitMedicineBatches();
			break;
		case 4:
			cout << "\nRedirecting you back to previous menu\n";
			system("pause");
			medStrgModule.nurseModuleMenu();
			break;
		case 5:
			system("pause");
			if (!auth.logout()) {
				StorageMgmntMenu();
			}
			break;
		}
	}
}

//This function going to list of the medicines that are available in the system
//and ask the user to choose which medicine that they want to use
//then the system will display the medicine's batches and ask the user 
//to choose which batch that they want to use
string StorageMgmntSub::getMedicineBatchesID() {

	system("cls");
	art.logoArt();
	art.directoryArt("MSMM/Storage Management sub-module/Get Medicines ID");

	//initialize query and table
	string selectMedQuery = "SELECT * FROM medicines";
	const char* q = selectMedQuery.c_str();
	conState = mysql_query(connection, q);
	clitable::Table table;

	if (!conState) {
		res = mysql_store_result(connection);
		int num_fields = mysql_num_fields(res);

		//set table design and header
		clitable::Column c[7] = {
			clitable::Column("Meds ID",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
			clitable::Column("Meds Name",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 26, clitable::Column::NON_RESIZABLE),
			clitable::Column("Meds  Dosage",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 6, clitable::Column::NON_RESIZABLE),
			clitable::Column("Meds Type",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 11, clitable::Column::NON_RESIZABLE),
			clitable::Column("Meds Price",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 11, clitable::Column::NON_RESIZABLE),
			clitable::Column("Meds Brand",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 25, clitable::Column::NON_RESIZABLE),
			clitable::Column("Sedative",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 11, clitable::Column::NON_RESIZABLE)
		};

		//add column to the table
		for (int i = 0; i < 7; i++) { table.addColumn(c[i]); }

		while ((row = mysql_fetch_row(res))) {
			vector<string> rowData; //create a vector to store the row data

			for (int i = 0; i < num_fields; i++) { //loop through the row data
				if (i != 5) { //exclude unwanter column
					if (i == 7) {//convert int to readable string

						int roleValue = atoi(row[i]);
						rowData.push_back(roleValue == 1 ? "Yes" : "No");
					}
					else {
						//add other field to the vector
						rowData.push_back(row[i] ? row[i] : "NULL");
					}
				}
			}
			//convert vector to array
			string* medsData = &rowData[0];

			//add data to the table
			table.addRow(medsData);
		}
		cout << table.draw() << endl;
		mysql_free_result(res);

		string medsID;
		do {

			cout << "Please enter medicine's ID : ";
			cin >> medsID;
			cin.clear();
			cin.ignore(INT_MAX, '\n');

			//define sql statement and connection state
			string query = "SELECT batches.* FROM medicines "
				"INNER JOIN batches ON "
				"medicines.medsID = batches.medsID "
				"WHERE medicines.medsID = '" + medsID + "'";

			const char* q = query.c_str();
			conState = mysql_query(connection, q);

			if (!conState) {
				res = mysql_store_result(connection);
				if (mysql_num_rows(res) > 0) {

					cout << "\n\x1B[32mMedicine's batches found\033[0m\n\n";
					system("pause");
					system("cls");
					art.logoArt();
					art.directoryArt("MSMM/Storage Management sub-module/Get batch ID");

					clitable::Table batchTable;
					clitable::Column bc[4] = {
						clitable::Column("Batch ID",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
						clitable::Column("Batch Quantity",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 19, clitable::Column::NON_RESIZABLE),
						clitable::Column("Batch Date Entry",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 40, clitable::Column::NON_RESIZABLE),
						clitable::Column("Batch Expiry Date",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 40, clitable::Column::NON_RESIZABLE)
					};

					for (int i = 0; i < 4; i++) batchTable.addColumn(bc[i]);
					int x = 0;
					while (row = mysql_fetch_row(res)) {

						vector<string> batchData;

						for (int i = 0; i < 5; i++) {
							if (i == 1) {
								continue;
								x++;
							}
							else {
								batchData.push_back(row[i] ? row[i] : "NULL");
								x++;
							}
						}

						string* newBatchData = &batchData[0];
						batchTable.addRow(newBatchData);
					}
					cout << batchTable.draw() << endl;
					break;
				}
				else {
					cout << "\n\x1B[31mThere's no recorded batches for such medicine, please try again\033[0m\n\n";
					system("pause");
					StorageMgmntMenu();
				}
			}
			else {
				cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
				exit(0);
			}

		} while (true);

		string batchID;
		do {
			cout << "Please enter batch's ID : ";
			cin >> batchID;
			cin.clear();
			cin.ignore(INT_MAX, '\n');

			//define sql statement and connection state
			string query = "SELECT * FROM batches WHERE batchID = '" + batchID + "' AND medsID = '" + medsID + "'";
			const char* q = query.c_str();
			conState = mysql_query(connection, q);

			if (!conState) {
				res = mysql_store_result(connection);
				if (mysql_num_rows(res) > 0) {
					cout << "\n\x1B[32mBatches found\033[0m\n\n";
					return batchID;
					break;
				}
				else {
					cout << "\n\x1B[31mBatch for that medicines not found, Please try again\033[0m\n\n";
					system("pause");
					StorageMgmntMenu();
				}
			}
			else {
				cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
				exit(0);
			}
		} while (true);
	}
	else {
		cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}
}

//this function receive the batch ID to display the batch's information
//and ask the user to choose which information that they want to update
void StorageMgmntSub::StorageMgmntUpdateBatches(string ID) {

	system("cls");
	art.logoArt();
	art.directoryArt("MSMM/Storage Management sub-module/Update batch info");

	//define sql statement and connection state
	string query = "SELECT * FROM batches WHERE batchID = '" + ID + "'";
	const char* q = query.c_str();
	conState = mysql_query(connection, q);

	if (!conState) {
		res = mysql_store_result(connection);
		row = mysql_fetch_row(res);

		clitable::Table batchTable;
		clitable::Column bc[4] = {
			clitable::Column("Batch ID",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
			clitable::Column("Batch Quantity",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 19, clitable::Column::NON_RESIZABLE),
			clitable::Column("Batch Date Entry",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 40, clitable::Column::NON_RESIZABLE),
			clitable::Column("Batch Expiry Date",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 40, clitable::Column::NON_RESIZABLE)
		};

		for (int i = 0; i < 4; i++) batchTable.addColumn(bc[i]);

		string updatedData[4];
		int x = 0;

		for (int i = 0; i < 5; i++) {
			if (i == 1) {
				continue;
				x++;
			}
			else {
				updatedData[x] = row[i];
				x++;
			}
		}

		batchTable.addRow(updatedData);
		cout << batchTable.draw() << endl;
		mysql_free_result(res);

		int option = 0;
		cout << "\n\x1B[94mYou're about to modify the information of this batches.\033[0m";
		cout << "\n\x1B[94mPlease make sure the information you provide matches the inventory in stocks\033[0m";
		cout << "\n\x1B[94mPlease select the information that you want to update\033[0m\n\n";

		cout << "1 - Batch Quantity\n";
		cout << "2 - Batch Date Expiry\n";
		cout << "3 - Go back to previous menu\n\n";
		cout << "Please enter your choice : ";
		cin >> option;

		if (cin.fail()) {
			cin.clear();
			cin.ignore();
			cout << "\n\x1B[31mInvalid input, please try again\033[0m\n";
			system("pause");
			StorageMgmntUpdateBatches(ID);
		}
		else {
			switch (option) {
			case 1:
				StorageMgmntUpdateBatchesQuantity(ID, updatedData[1]);
				break;
			case 2:
				StorageMgmntUpdateBatchesExpiry(ID, updatedData[2]);
				break;
			case 3:
				cout << "\nRedirecting you back to previous menu\n";
				system("pause");
				StorageMgmntMenu();
				break;
			}
		}
	}
	else {
		cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}
}

//this function take batches ID and prompt the user to enter the new quantity
//then update the quantity in the database
void StorageMgmntSub::StorageMgmntUpdateBatchesQuantity(string ID, string oldQty) {

	string newQuantity;

	do {
		cout << "\n\nCurrent batch quantity " << oldQty;
		cout << "\n\x1B[94mPlease enter batch expiry date using this format YYYY-MM-DD ex: 2029-12-01\033[0m";
		cout << "\nEnter new quantity : ";
		cin >> newQuantity;

		if (!misc.isNumeric(newQuantity)) {
			newQuantity.clear();
			cin.clear();
			cin.ignore(INT_MAX, '\n');
			cout << "\x1B[33mInvalid response, please try again\033[0m\n";
		}
		else if (stoi(newQuantity) < 0) {
			newQuantity.clear();
			cin.clear();
			cin.ignore(INT_MAX, '\n');
			cout << "\x1B[33mQuantity must above 1, please try again\033[0m\n";
		}
		else {
			break;
		}
	} while (true);

	string query = "UPDATE batches SET batchQuantity = '" + newQuantity + "' WHERE batchID = '" + ID + "'";
	const char* q = query.c_str();
	conState = mysql_query(connection, q);

	cout << "\n\x1B[32mBatch's quantity has been successfully updated\033[0m\n";
	system("pause");
	StorageMgmntMenu();
}

//this function take batches ID and prompt the user to enter the new expiry date
//then update the expiry date in the database
void StorageMgmntSub::StorageMgmntUpdateBatchesExpiry(string ID, string oldQty) {

	string newExpiryDate;

	do {
		cout << "\n\nCurrent batch expiry date " << oldQty << endl;
		cout << "\x1B[94mPlease enter batch expiry date\033[0m";
		cout << "\nEnter new expiry date : ";
		cin >> newExpiryDate;

		if (misc.isValidDateFormat(newExpiryDate)) {
			break;
		}
	} while (true);

	string query = "UPDATE batches SET batchExpiry = '" + newExpiryDate + "' WHERE batchID = '" + ID + "'";
	const char* q = query.c_str();
	conState = mysql_query(connection, q);

	cout << "\n\x1B[32mBatch's expiry date has been successfully updated\033[0m\n";
	system("pause");
	StorageMgmntMenu();
}

//this function take batches ID and prompt the user to confirm the deletion
//then delete the batch from the database
void StorageMgmntSub::StorageMgmntDeleteBatches(string ID) {

	system("cls");
	art.logoArt();
	art.directoryArt("MSMM/Storage Management sub-module/Delete batch record");

	//define sql statement and connection state
	string query = "SELECT * FROM batches WHERE batchID = '" + ID + "'";
	const char* q = query.c_str();
	conState = mysql_query(connection, q);

	if (!conState) {

		res = mysql_store_result(connection);
		row = mysql_fetch_row(res);

		clitable::Table batchTable;
		clitable::Column bc[4] = {
			clitable::Column("Batch ID",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
			clitable::Column("Batch Quantity",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 19, clitable::Column::NON_RESIZABLE),
			clitable::Column("Batch Date Entry",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 40, clitable::Column::NON_RESIZABLE),
			clitable::Column("Batch Expiry Date",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 40, clitable::Column::NON_RESIZABLE)
		};

		for (int i = 0; i < 4; i++) batchTable.addColumn(bc[i]);

		string updatedData[4];
		int x = 0;

		for (int i = 0; i < 5; i++) {
			if (i == 1) {
				continue;
				x++;
			}
			else {
				updatedData[x] = row[i];
				x++;
			}
		}

		batchTable.addRow(updatedData);
		cout << batchTable.draw() << endl;
		mysql_free_result(res);

		bool continueLoop = true;

		do {

			char confirm;
			cout << "\x1B[33mYou are about to remove this batch record, please make sure the inventory in stock is allign with this action\033[0m\n";
			cout << "\x1B[33mAre you sure want to remove this batch?\033[0m\n";
			cout << "\nEnter your choice (Y/N) : ";
			cin >> confirm;

			switch (confirm) {
			case 'Y':
			case 'y':
				query = "DELETE FROM batches WHERE batchID = '" + ID + "'";
				q = query.c_str();
				conState = mysql_query(connection, q);

				cout << "\n\x1B[32mBatch's record has been successfully deleted\033[0m\n";
				system("pause");
				StorageMgmntMenu();
				break;
			case 'N':
			case 'n':
				cout << "\n\x1B[31mAction cancelled\033[0m\n";
				cout << "Redirecting you back to previous menu\n";
				system("pause");
				StorageMgmntMenu();
				break;
			}
		} while (continueLoop);
	}
	else {
		cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}
}

//This function going to filter the medicine that can be split with this requirement
//1. The medicine must be in tablet form
//2. The medicine must have at least 1 batch
//3. The medicine mustn't have been split before
//The user will be asked to choose which medicine that they want to split and ask for confirmation
//then the system will split the medicine and add splitted medicine as new medicine and add the splitted batch 
void StorageMgmntSub::splitMedicineBatches() {

	system("cls");
	art.logoArt();
	art.directoryArt("MSMM/Storage Management sub-module/Split medicine");
	

	//define sql statement and connection state
	string query = "SELECT DISTINCT medicines.* "
		"FROM medicines "
		"JOIN batches ON medicines.medsID = batches.medsID "
		"WHERE medicines.medsName NOT LIKE '%splitted%' "
		"AND medicines.medsType = 'tablet' ";

	const char* q = query.c_str();
	conState = mysql_query(connection, q);

	if (!conState) {
		res = mysql_store_result(connection);
		if (mysql_num_rows(res) > 0) {

			int num_fields = mysql_num_fields(res);

			clitable::Table table;
			clitable::Column c[7] = {
				clitable::Column("Meds ID",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
				clitable::Column("Meds Name",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 26, clitable::Column::NON_RESIZABLE),
				clitable::Column("Meds  Dosage",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 6, clitable::Column::NON_RESIZABLE),
				clitable::Column("Meds Type",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 11, clitable::Column::NON_RESIZABLE),
				clitable::Column("Meds Price",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 11, clitable::Column::NON_RESIZABLE),
				clitable::Column("Meds Brand",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 25, clitable::Column::NON_RESIZABLE),
				clitable::Column("Sedative",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 11, clitable::Column::NON_RESIZABLE)
			};

			for (int i = 0; i < 7; i++) { table.addColumn(c[i]); }

			while ((row = mysql_fetch_row(res))) {
				vector<string> rowData; //create a vector to store the row data

				for (int i = 0; i < num_fields; i++) { //loop through the row data
					if (i != 5) { //exclude unwanter column
						if (i == 7) {//convert int to readable string

							int roleValue = atoi(row[i]);
							rowData.push_back(roleValue == 1 ? "Yes" : "No");
						}
						else {
							//add other field to the vector
							rowData.push_back(row[i] ? row[i] : "NULL");
						}
					}
				}
				//convert vector to array
				string* medsData = &rowData[0];

				//add data to the table
				table.addRow(medsData);
			}

			cout << table.draw() << endl;
			mysql_free_result(res);

			string medsID;
			do {

				cout << "Please enter medicine's ID : ";
				cin >> medsID;
				cin.clear();
				cin.ignore(INT_MAX, '\n');

				//define sql statement and connection state
				string query = "SELECT DISTINCT medicines.* "
					"FROM medicines "
					"JOIN batches ON medicines.medsID = batches.medsID "
					"WHERE medicines.medsName NOT LIKE '%splitted%' "
					"AND medicines.medsType = 'tablet' "
					"AND medicines.medsID = 'MED0001' ";

				const char* q = query.c_str();
				conState = mysql_query(connection, q);

				if (!conState) {
					res = mysql_store_result(connection);

					if (mysql_num_rows(res) > 0) {

						cout << "\n\x1B[32mMedicine's batches found\033[0m\n\n";
						system("pause");
						break;
					}
					else {
						cout << "\n\x1B[31mThere's no recorded batches for such medicine, please try again\033[0m\n\n";
						system("pause");
						StorageMgmntMenu();
					}
				}
				else {
					cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
					exit(0);
				}
			} while (true);

			system("cls");
			art.logoArt();
			art.directoryArt("MSMM/Storage Management sub-module/Split medicine/Get Batch ID");

			string query = "SELECT * FROM batches WHERE medsID = '" + medsID + "'";
			const char* q = query.c_str();
			conState = mysql_query(connection, q);

			if (!conState) {
				res = mysql_store_result(connection);

				if (mysql_num_rows(res) > 0) {
					int num_fields = mysql_num_fields(res);

					clitable::Table batchTable;
					clitable::Column bc[4] = {
						clitable::Column("Batch ID",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
						clitable::Column("Batch Quantity",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 19, clitable::Column::NON_RESIZABLE),
						clitable::Column("Batch Date Entry",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 40, clitable::Column::NON_RESIZABLE),
						clitable::Column("Batch Expiry Date",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 40, clitable::Column::NON_RESIZABLE)
					};

					for (int i = 0; i < 4; i++) batchTable.addColumn(bc[i]);

					int x = 0;
					while (row = mysql_fetch_row(res)) {

						vector<string> batchData;

						for (int i = 0; i < 5; i++) {
							if (i == 1) {
								continue;
								x++;
							}
							else {
								batchData.push_back(row[i] ? row[i] : "NULL");
								x++;
							}
						}

						string* newBatchData = &batchData[0];
						batchTable.addRow(newBatchData);
					}
					cout << batchTable.draw() << endl;
					mysql_free_result(res);

					string batchID;
					do {
						cout << "Please enter batch's ID : ";
						cin >> batchID;
						cin.clear();
						cin.ignore(INT_MAX, '\n');

						//define sql statement and connection state
						string query = "SELECT * FROM batches WHERE batchID = '" + batchID + "' AND medsID = '" + medsID + "'";
						const char* q = query.c_str();
						conState = mysql_query(connection, q);

						if (!conState) {
							res = mysql_store_result(connection);
							if (mysql_num_rows(res) > 0) {
								cout << "\n\x1B[32mBatches found\033[0m\n\n";
								break;
							}
							else {
								cout << "\n\x1B[31mBatch for that medicines not found, Please try again\033[0m\n\n";
								system("pause");
							}
						}
						else {
							cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
							exit(0);
						}
					} while (true);

					bool continueLoop = true;
					do {
						char confirmation;
						cout << "\n\x1B[94mYou are about to split the dosage of this medicine\033[0m";
						cout << "\n\x1B[94mBy splitting this medicine, it will create spliited record of this medicine with quantitiy you provide\033[0m";
						cout << "\n\x1B[94mPlease make sure the information you going to provide is alligned with your stocks in inventory\033[0m";
						cout << "\nAre you sure to proceed this action? (Y/N) : ";
						cin >> confirmation;

						switch (confirmation) {
						case 'Y':
						case'y':
							continueLoop = false;
							break;
						case 'N':
						case'n':
							cout << "\n\x1B[31mAction cancelled\033[0m\n";
							cout << "Redirecting you back to previous menu\n";
							system("pause");
							StorageMgmntMenu();
							break;
						}
					} while (continueLoop);

					string currentQty, newQty, splitQty;
					do {
						string qty;
						cout << "\n\x1B[94mThe ammount of the splitted medicine must be less than or equal to the quantity of this batch\033[0m";
						cout << "\n\x1B[94mPlease enter quantity of this medicine you want to split\033[0m";
						cout << "\nEnter quantity : ";
						cin >> qty;

						if (!misc.isNumeric(qty)) {
							qty.clear();
							cin.clear();
							cin.ignore(INT_MAX, '\n');
							cout << "\x1B[33mInvalid response, please try again\033[0m\n";
						}
						else {
							splitQty = qty;
							break;
						}
					} while (true);

					//create row for batch
					string splitBatchID, splitDateExpiry;

					//create batch ID for spliited batch
					string currentID;
					string getBatIDQuery = "SELECT * FROM batches ORDER BY batchID DESC LIMIT 1";
					const char* q = getBatIDQuery.c_str();
					conState = mysql_query(connection, q);
					if (!conState) {
						res = mysql_store_result(connection);
						row = mysql_fetch_row(res);

						if (row) {
							currentID = row[0];
						}
						else {
							currentID = "0000";
						}
					}
					else {
						cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
						exit(0);
					}
					splitBatchID = misc.createID(currentID, "BAT");

					//get and set splitted batch info
					string getBatInfoQuery = "SELECT * FROM batches WHERE batchID = '" + batchID + "'";
					q = getBatInfoQuery.c_str();
					conState = mysql_query(connection, q);

					if (!conState) {
						res = mysql_store_result(connection);
						row = mysql_fetch_row(res);

						if (splitQty <= row[2]) {
							currentQty = row[2];
							newQty = to_string(stoi(currentQty) - stoi(splitQty));
						}
						else {
							cout << "\n\x1B[31mThe quantity you entered is more than the quantity of this batch, please try again\033[0m\n";
							system("pause");
							StorageMgmntMenu();
						}

						splitDateExpiry = row[4];
					}
					else {
						cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
						exit(0);
					}

					string insertQuery = "INSERT INTO batches "
						"(batchID, medsID, batchQuantity, batchDateEntry, batchExpiry ) "
						"VALUES "
						"('" + splitBatchID + "', '" + medsID + "', '" + splitQty + "',  CURRENT_DATE() , '" + splitDateExpiry + "')";
					const char* q2 = insertQuery.c_str();
					conState = mysql_query(connection, q2);

					//create new splitted medicine row
					string splittedMedsID, splittedMedsName, splittedMedsDosage, splittedMedsType, splittedMedsPrice, passedID;
					string spliitedMedsDesc, splittedMedsBrand;
					int splittedMedsSedative;

					//create medsID for splitted medicine
					string query = "SELECT * FROM medicines ORDER BY medsID DESC LIMIT 1";
					q = query.c_str();
					conState = mysql_query(connection, q);
					if (!conState) {
						res = mysql_store_result(connection);
						row = mysql_fetch_row(res);

						if (row) {
							passedID = row[0];
						}
						else {
							passedID = "0000";
						}
					}
					else {
						cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
						exit(0);
					}
					splittedMedsID = misc.createID(passedID, "MED");

					//get current medicine info and split the dosage
					query = "SELECT * FROM medicines WHERE medsID = '" + medsID + "'";
					const char* q1 = query.c_str();
					conState = mysql_query(connection, q1);

					if (!conState) {
						res = mysql_store_result(connection);
						row = mysql_fetch_row(res);

						//meds name
						splittedMedsName = row[1];
						splittedMedsName += " splitted";

						//meds dosage
						splittedMedsDosage = to_string(stoi(row[2]) / 2);

						//meds type
						splittedMedsType = row[3];

						//meds price
						splittedMedsPrice = row[4];

						//meds description
						spliitedMedsDesc = row[5];

						//meds brand
						splittedMedsBrand = row[6];

						//meds sedative
						splittedMedsSedative = atoi(row[7]);
					}
					else {
						cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
						exit(0);
					}
					
					//create new splitted medicine row
					string insertBatQuery = "INSERT INTO medicines "
						"VALUES ('" + splittedMedsID + "', '" + splittedMedsName + "', '" + splittedMedsDosage + "', '"
						+ splittedMedsType + "', '" + splittedMedsPrice + "', '" + spliitedMedsDesc + "', '" + splittedMedsBrand + 
						"', '" + to_string(splittedMedsSedative) + "')";

					q2 = insertBatQuery.c_str();
					conState = mysql_query(connection, q2);

					//update old batch quantity
					string updateQuery = "UPDATE batches SET "
						"batchQuantity = '" + newQty + "' "
						"WHERE batchID ='" + batchID + "' ";
					q2 = updateQuery.c_str();
					conState = mysql_query(connection, q2);

					cout << "\n\x1B[32mMedicines has been succesfully splitted\033[0m\n";
					system("pause");
					StorageMgmntMenu();
				}
				else {
					cout << "\n\x1B[31mThere's no recorded batches for such medicine, please try again\033[0m\n\n";
					system("pause");
					StorageMgmntMenu();
				}
			}
			else {
				cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
				exit(0);
			}
		}
		else {
			cout << "\n\x1B[31mThere's no medicine that can be splitted\033[0m\n";
			system("pause");
			StorageMgmntMenu();
		}
	}
	else {
		cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}
}