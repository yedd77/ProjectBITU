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
void StorageMgmntSub::StorageMgmntMenu(){

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
			//StorageMgmntView();
			break;
		case 3:
			//StorageMgmntView();
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
string StorageMgmntSub::getMedicineBatchesID(){

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
			string query = "SELECT * FROM batches WHERE batchID = '" + batchID + "' AND medsID = '" +medsID+"'";
			const char* q = query.c_str();
			conState = mysql_query(connection, q);

			if (!conState) {
				res = mysql_store_result(connection);
				if (mysql_num_rows(res) > 0) {
					system("pause");
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
void StorageMgmntSub::StorageMgmntUpdateBatches(string ID){
	
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
		system("pause");
	}
	else {
		cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
	}
}

