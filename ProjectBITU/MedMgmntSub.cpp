#include "MedMgmntSub.h"
#include "Artwork.h"
#include <iostream>
#include "Misc.h"
#include "MedStrgModule.h"
#include "Auth.h"
#include "table.h"


using namespace std;

extern Artwork art;
extern Auth auth;
extern Misc misc;
extern MedStrgModule medStrgModule;

//function to display the meds management sub-module menu
void MedMgmntSub::MedsMgmntMenu() {

	system("cls");
	art.logoArt();
	art.directoryArt("MSMM/Meds Management sub-module");

	cout << "\x1B[94mPlease select your next action\033[0m\n\n";

	cout << "1 - Add new medicines\n";
	cout << "2 - View existing medicines\n";
	cout << "3 - Go back to previous menu\n";
	cout << "4 - Log out\n";

	cout << "\nEnter your choice : ";

	int option;
	cin >> option;

	if (cin.fail()) {
		cin.clear();
		cin.ignore();
		cout << "\n\x1B[31mInvalid input, please try again\033[0m\n";
		system("pause");
		MedsMgmntMenu();
	}
	else {
		switch (option) {
		case 1:
			MedsMgmntAdd();
			break;
		case 2:
			medsMgmntView();
			break;
		case 3:
			cout << "\nRedirecting you back to previous menu\n";
			system("pause");
			medStrgModule.nurseModuleMenu();
			break;
		case 4:
			system("pause");
			if (!auth.logout()) {
				MedsMgmntMenu();
			}
			break;
		}
	}
}

//function to add new medicine to database
void MedMgmntSub::MedsMgmntAdd() {

	system("cls");
	art.logoArt();
	art.directoryArt("MSMM/Meds Management sub-module/Add New Medicines");

	string medName, medType, medDesc, medDosage, medPrice, medsBrand;
	int isSedative;

	cout << "You are about to add new medicines, to avoid redundancy of data please provide the new medicine's name\n";
	cout << "\x1B[94mPlease enter the medicine's name exactly as the packaging\033[0m\n\n";

	//meds name
	cout << "Enter new medicine's name : ";
	cin.clear();
	cin.ignore(INT_MAX, '\n');
	getline(cin, medName);


	//check if the meds name is already in the database
	if (misc.checkMedsNameExist(medName)) {
		cout << "\n\x1B[91mMedicine's name already exist in database, please try again\033[0m\n";
		system("pause");
		MedsMgmntMenu();
	}

	cout << "Medicine did not exist in database\n";
	cout << "\n\x1B[94mPlease provide more information\n\033[0m\n";

	//meds ID
	//retrieve last record on DB
	string query = "SELECT * FROM medicines ORDER BY medsID DESC LIMIT 1";

	const char* q = query.c_str();
	conState = mysql_query(connection, q);

	if (!conState) {
		res = mysql_store_result(connection);
		row = mysql_fetch_row(res);
	}
	else {
		cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}

	//convert SQL array to normal string and pass the value to createID
	string passedID;
	if (row) {
		passedID = row[0];
	}
	else {
		passedID = "0000";
	}
	string medsID = misc.createID(passedID, "MED");

	//meds type
	bool continueLoop = true;
	do {
		int option;

		cout << "Select medicine's type\n\n";
		cout << "1 - Tablet\n";
		cout << "2 - Capsule\n";
		cout << "3 - Syrup\n";

		cout << "\nEnter medicine's type : ";
		cin >> option;

		switch (option) {
		case 1:
			medType = "Tablet";
			continueLoop = false;
			break;
		case 2:
			medType = "Capsule";
			continueLoop = false;
			break;
		case 3:
			medType = "Syrup";
			continueLoop = false;
			break;
		default:
			cout << "\n\x1B[91mInvalid input, please try again\033[0m\n";
			option = 0;
			system("pause");
			break;
		}
	} while (continueLoop);


	//meds description 
	cout << "\n\x1B[94mMedication description include what is the purpose of the medication\n\033[0m";
	cout << "Enter medicine's description : ";
	cin.clear();
	cin.ignore(INT_MAX, '\n');
	getline(cin, medDesc);

	//meds dosage
	do {

		float dosage;

		cout << "\n\x1B[94mPlease enter the medication's dosage per serving in miligram (mg)\033[0m";
		cout << "\nEnter medicine's dosage : ";
		if (!(cin >> dosage)) {
			dosage = 0;
			cin.clear();
			cin.ignore(INT_MAX, '\n');
			cout << "\x1B[33mInvalid response, please try again\033[0m\n";
			continue;
		}
		else {
			medDosage = to_string(dosage);
			break;
		}

	} while (true);

	//meds price
	do {
		double price = 0;
		cout << "\n\x1B[94mPlease enter medication price\033[0m";
		cout << "\nEnter medicine's price (RM) : ";

		if (!(cin >> price)) {
			price = 0;
			cin.clear();
			cin.ignore(INT_MAX, '\n');
			cout << "\x1B[33mInvalid response, please try again\033[0m\n";
			continue;
		}
		else {
			price = price + 0.00;
			medPrice = to_string(price);
			break;
		}
	} while (true);

	//meds sedative
	int option = 0;
	do {
		cout << "\n\x1B[94mIs the medicine a sedative?\033[0m\n\n";
		cout << "1 - Yes\n";
		cout << "2 - No\n";

		cout << "Enter your choice : ";
		cin >> option;

		switch (option) {
		case 1:
			isSedative = 1;
			break;
		case 2:
			isSedative = 0;
			break;
		default:
			cout << "\n\x1B[91mInvalid input, please try again\033[0m\n";
			option = 0;
			system("pause");
			break;
		}
	} while (option == 0);

	cout << "\n\x1B[94mPlease enter medication brand\033[0m";
	cout << "\nMedication's Brand : ";
	cin >> medsBrand;

	string insertQuery =
		"INSERT INTO medicines "
		"(medsID, medsName, medsDosage, medsType, medsPrice, medsDesc, medsBrand, isSedative) "
		"VALUES "
		"('" + medsID + "', '" + medName + "', '" + medDosage + "', '" + medType + "', '" + medPrice + "', '" + medDesc + "', '" + medsBrand + "', '" + to_string(isSedative) + "')";

	const char* q2 = insertQuery.c_str();
	conState = mysql_query(connection, q2);

	//check query execution
	if (!conState) {
		cout << "\n\x1B[32mMedicine's information successfully added to database\033[0m\n";
		cout << "\nDirecting you back to previous menu\n";
		system("pause");
		MedsMgmntMenu();
	}
	else {
		cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}
}

//function to display meds list
void MedMgmntSub::medsMgmntView() {

	system("cls");
	art.logoArt();
	art.directoryArt("MSMM/Meds Management sub-module/View Medicines");

	//initialize query and table
	string query = "SELECT * FROM medicines";
	const char* q = query.c_str();
	conState = mysql_query(connection, q);
	clitable::Table table;

	//check query execution
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


		bool continueLoop3 = true;

		do {
			int option = 0;

			cout << "\x1B[94mPlease enter your next action\033[0m\n\n";
			cout << "1 - Edit or remove medicine\n";
			cout << "2 - Go back to previous menu\n\n";
			cout << "Enter your choice : ";

			if (cin >> option) {

				switch (option) {
				case 1:
					continueLoop3 = false;
					option = 0;
					medsSearch();
					break;
				case 2:
					cout << "\nRedirecting you back to previous menu\n";
					system("pause");
					MedsMgmntMenu();
					break;
				}
			}
			else {
				continueLoop3 = true;
				option = 0;
				cout << "\n\x1B[31mInvalid input\033[0m\n";
				cout << "\x1B[31mPlease try again\033[0m\n\n";
				cin.clear();
				cin.ignore();
			}


		} while (continueLoop3);
	}
	else {
		cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}
}

//function to search meds for edit or remove user
void MedMgmntSub::medsSearch() {

	string medsID;
	cin.clear();
	cin.ignore(INT_MAX, '\n');

	do {

		cout << "Please enter medicine's ID : ";
		cin >> medsID;
		cin.clear();
		cin.ignore(INT_MAX, '\n');

		//define sql statement and connection state
		string query = "SELECT * FROM medicines WHERE medsID = '" + medsID + "'";
		const char* q = query.c_str();
		conState = mysql_query(connection, q);

		if (!conState) {
			res = mysql_store_result(connection);
			if (mysql_num_rows(res) > 0) {
				cout << "\n\x1B[32mMedicines found\033[0m\n\n";
				break;
			}
			else {
				cout << "\n\x1B[31mMedicines not found\033[0m\n\n";
				system("pause");
				medsMgmntView();
			}
		}
		else {
			cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
			exit(0);
		}

	} while (true);

	row = mysql_fetch_row(res);
	string medsData[8];
	boolean continueLoop = true;
	int option = 0;
	int medsDataIndex = 0;

	//looping through column from database
	for (int i = 0; i < 8; i++) {
		if (row[i] != nullptr) {
			medsData[medsDataIndex] = row[i];
			medsDataIndex++;
		}
		else {
			medsData[medsDataIndex] = "NULL";
			medsDataIndex++;
		}
	}

	do {

		cout << "\x1B[94mPlease select your next option\033[0m\n\n";

		cout << "1 - Edit medicine's information\n";
		cout << "2 - Remove medicine from database\n";
		cout << "3 - Add new batch of stock for this medicines\n";
		cout << "4 - Go back to previous menu\n";
		cout << "Enter your option : ";

		cin >> option;

		if (cin.fail()) {
			cin.clear();
			cin.ignore();
			cout << "\n\x1B[31mInvalid input\033[0m\n";
			cout << "\x1B[31mPlease try again\033[0m\n\n";
			option = 0;
			continue;
		}

		switch (option) {
		case 1:
			medsUpdate(medsData);
			break;
		case 2:
			medsRemove(medsData);
			break;
		case 3:
			batchMgmntAdd(medsData);
			break;
		case 4:
			medsMgmntView();
			break;
		}

	} while (continueLoop);
}

void MedMgmntSub::medsUpdate(string* medsData) {
	system("cls");
	art.logoArt();
	art.directoryArt("MSMM/Meds Management sub-module/View Medicines/Edit Medicine");

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

	for (int i = 0; i < 7; i++) table.addColumn(c[i]);


	string updatedData[7];
	int j = 0;
	for (int i = 0; i < 8; i++) {

		if (i == 4) {

			updatedData[j] = "RM " + medsData[i];
			j++;
		}
		else if (i == 5) {
			continue;
			j++;
		}
		else if (i == 7) {

			updatedData[j] = stoi(medsData[i]) == 1 ? "Yes" : "No";
			j++;
		}
		else {
			updatedData[j] = medsData[i];
			j++;
		}
	}

	table.addRow(updatedData);
	cout << table.draw();
	cout << "Meds Description : " << medsData[5] << endl << endl;

	int option = 0;
	boolean continueLoop = true;

	do {
		cout << "\x1B[96mTo maintain accurate stock records, some column cannot be modified directly. \033[0m";
		cout << "\n\x1B[96mIf there are significant changes to these aspects, please remove the medication\033[0m";
		cout << "\n\x1B[96mand add it again with the correct details.\033[0m\n\n";

		cout << "\n\x1B[94mSelect Column you want to edit\033[0m\n\n";
		cout << "1 - Meds Price\n";
		cout << "2 - Meds Description\n";
		cout << "3 - Go back to previous menu\n";
		cout << "Enter your choice : ";
		cin >> option;

		if (cin.fail()) {
			cin.clear();
			cout << "\n\x1B[31mInvalid input\033[0m\n";
			cout << "\x1B[31mPlease try again\033[0m\n\n";
			option = 0;
			continue;
		}

		switch (option) {
		case 1:
			medsUpdatePrice(medsData);
			break;
		case 2:
			medsUpdateDesc(medsData);
			break;
		case 3:
			medsMgmntView();
			break;
		}
	} while (continueLoop);

}

//Function to update meds price
void MedMgmntSub::medsUpdatePrice(string* medsData) {

	double price;

	//meds price
	do {
		cout << "\n\nCurrent medicine's price : RM " << medsData[4] << endl;
		cout << "\n\x1B[94mPlease enter new medication price\033[0m";
		cout << "\nEnter medicine's price (RM) : ";

		if (!(cin >> price)) {
			price = 0;
			cin.clear();
			cin.ignore(INT_MAX, '\n');
			cout << "\x1B[33mInvalid response, please try again\033[0m\n";
			continue;
		}
		else {
			price = price + 0.00;
			break;
		}
	} while (true);

	string query = "UPDATE medicines SET "
		"medsPrice = '" + to_string(price) + "' "
		"WHERE medsID = '" + medsData[0] + "'";

	const char* q = query.c_str();
	conState = mysql_query(connection, q);

	cout << "\n\x1B[32mMedicine's price successfully updated\033[0m\n";
	system("pause");
	medsMgmntView();
}

void MedMgmntSub::medsUpdateDesc(string* medsData) {
	string medDesc;

	cout << "\n\nCurrent medicine's description : " << medsData[5] << endl;
	cout << "\n\x1B[94mPlease enter medication description\033[0m";
	cout << "\nEnter medicine's description : ";
	cin.clear();
	cin.ignore(INT_MAX, '\n');
	getline(cin, medDesc);

	string query = "UPDATE medicines SET "
		"medsDesc = '" + medDesc + "' "
		"WHERE medsID = '" + medsData[0] + "'";

	const char* q = query.c_str();
	conState = mysql_query(connection, q);

	cout << "\n\x1B[32mMedicine's description successfully updated\033[0m\n";
	system("pause");
	medsMgmntView();
}

//TODO
void MedMgmntSub::medsRemove(string* medsData) {

	system("cls");
	art.logoArt();
	art.directoryArt("MSMM/Meds Management sub-module/View Medicines/Remove Medicine");

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

	for (int i = 0; i < 7; i++) table.addColumn(c[i]);

	string updatedData[7];
	int j = 0;
	for (int i = 0; i < 8; i++) {

		if (i == 4) {

			updatedData[j] = "RM " + medsData[i];
			j++;
		}
		else if (i == 5) {
			continue;
			j++;
		}
		else if (i == 7) {

			updatedData[j] = stoi(medsData[i]) == 1 ? "Yes" : "No";
			j++;
		}
		else {
			updatedData[j] = medsData[i];
			j++;
		}
	}

	table.addRow(updatedData);
	cout << table.draw();

	bool continueLoop = true;

	do {
		char confirm;
		cout << "\x1B[33mBy deleting this medicines, the record of stocks inside the inventory also will be deleted.\033[0m\n";
		cout << "\x1B[33mAre you sure want to remove this medicines?\033[0m\n";
		cout << "\nEnter your choice (Y/N) : ";
		cin >> confirm;

		switch (confirm) {
		case 'Y': {
			string query = "DELETE FROM medicines WHERE medsID = '" + medsData[0] + "'";
			const char* q = query.c_str();
			conState = mysql_query(connection, q);

			if (!conState) {
				cout << "\n\x1B[32mMedicine successfully removed\033[0m\n";
				system("pause");
				medsMgmntView();
			}
			else {
				cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
				exit(0);
			}
			break;
		}
		case 'N':
			cout << "Redirecting you to previous page\n";
			system("pause");
			medsMgmntView();
			break;
		default:
			cout << "\n\x1B[31mInvalid input, please try again\033[0m\n";
			system("pause");
			medsRemove(medsData);
		}

	} while (continueLoop);
}

//function to add new batch of stock for a meds
void MedMgmntSub::batchMgmntAdd(string* medsData) {
	system("cls");
	art.logoArt();
	art.directoryArt("MSMM/Meds Management sub-module/View Medicines/Add New Meds Batch");

	string queryBatch = "SELECT * FROM batches WHERE medsID = '" + medsData[0] + "'";
	const char* cu = queryBatch.c_str();
	conState = mysql_query(connection, cu);
	int batchCount;

	if (!conState) {
		res = mysql_store_result(connection);
		batchCount = static_cast<int>(mysql_num_rows(res));

		//check if there's any batch of this meds
		if (batchCount == 0) {
			art.directoryArt("There's no recorded stock for " + medsData[1] + " (medsID - " + medsData[0] + ")");
		}
		else {
			clitable::Table batchTable;
			clitable::Column bc[4] = {
				clitable::Column("Batch ID",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
				clitable::Column("Batch Quantity",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 19, clitable::Column::NON_RESIZABLE),
				clitable::Column("Batch Date Entry",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 40, clitable::Column::NON_RESIZABLE),
				clitable::Column("Batch Expiry Date",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 40, clitable::Column::NON_RESIZABLE)
			};

			for (int i = 0; i < 4; i++) batchTable.addColumn(bc[i]);

			batchTable.addTitle("Batch Details For : " + medsData[1] + " (medsID - " + medsData[0] + ")");

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

			char confirm;
			cout << "\n\x1B[94mYou are about to add new batch of stocks of this medicines.\033[0m\n";
			cout << "\x1B[94mDo you want to proceed this action?\033[0m\n";
			cout << "\nEnter your choice (Y/N) : ";
			cin >> confirm;

			switch (confirm) {
			case 'Y':
				break;
			case 'N':
				medsMgmntView();
				break;
			default:
				cout << "\n\x1B[31mInvalid input, please try again\033[0m\n";
				medsMgmntView();
			}
		}
	}
	else {
		cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}

	string batchID, batchQty, batchExpDate, medsID, batchDateEntry;

	//batchID instance
	//retrieve last record on DB
	string queryGetLast = "SELECT * FROM batches  WHERE medsID = '" + medsData[0] + " 'ORDER BY batchID DESC LIMIT 1";

	const char* q = queryGetLast.c_str();
	conState = mysql_query(connection, q);

	if (!conState) {
		res = mysql_store_result(connection);
		row = mysql_fetch_row(res);
	}
	else {
		cout << "\x1B[31m\nQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}

	//medsID instance
	medsID = medsData[0];
	//convert SQL array to normal string and pass the value to createID
	string passedID;
	if (row) {
		passedID = row[0];
	}
	else {
		passedID = "0000";
	}
	batchID = misc.createID(passedID, "BAT");

	//batchQty instance
	do {
		string qty;
		cout << "\n\x1B[94mPlease enter quantity of stock of this batch\033[0m";
		cout << "\nEnter batch quantity : ";
		cin >> qty;

		if (!misc.isNumeric(qty)) {
			qty.clear();
			cin.clear();
			cin.ignore(INT_MAX, '\n');
			cout << "\x1B[33mInvalid response, please try again\033[0m\n";
		}
		else {
			batchQty = qty;

			break;
		}
	} while (true);

	//Expiry date instance
	do {
		cout << "\n\x1B[94mPlease enter batch expiry date using this format YYYY-MM-DD ex: 2029-12-01\033[0m";
		cout << "\nEnter batch expiry date : ";
		cin >> batchExpDate;

		if (misc.isValidDateFormat(batchExpDate)) {
			break;
		}
	} while (true);

	string insertQuery = "INSERT INTO batches "
		"(batchID, medsID, batchQuantity, batchDateEntry, batchExpiry ) "
		"VALUES "
		"('" + batchID + "', '" + medsID + "', '" + batchQty + "',  CURRENT_DATE() , '" + batchExpDate + "')";

	const char* q2 = insertQuery.c_str();
	conState = mysql_query(connection, q2);

	//check query execution
	if (!conState) {
		cout << "\n\x1B[32mBatch successfully added to database\033[0m\n";
	}
	else {
		cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}

	cout << "Directing you back to previous menu\n";

	system("pause");
	MedsMgmntMenu();
}
