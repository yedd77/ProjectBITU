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

	cout << "\x1B[96mPlease select your next action\033[0m\n\n";

	cout << "1 - Register new medicine\n";
	cout << "2 - Update existing medicine\n";
	cout << "3 - Remove existing medicine\n"; 
	cout << "4 - View medicines list\n";
	cout << "5 - Search existing medicine\n";
	cout << "6 - Go back to previous menu\n"; 

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
		
		if (option == 1) {
			cout <<"\nRedirecting you to register new medicine page\n";
			system("pause");
			MedsMgmntAdd();
		}
		else if (option == 2) {
			cout << "\nRedirecting you to update existing medicine page\n";
			system("pause");
			medsMgmntView();
			string* medsData = medsSearch();
			system("pause");
			medsUpdate(medsData);
		}
		else if (option == 3) {
			cout << "\nRedirecting you to remove medicine record page\n";
			system("pause");
			medsMgmntView();
			string* medsData = medsSearch();
			system("pause");
			medsRemove(medsData);
		}
		else if (option == 4) {
			cout << "\nRedirecting you to medicine list page\n";
			system("pause");
			medsMgmntView();
			string* medsData = medsSearch();
			medsNextAction(medsData);
		}
		else if (option == 5) {
			cout << "\nRedirecting you to previous menu\n";
			system("pause");
			medStrgModule.nurseModuleMenu();
		}
		else if (option == 6) {
			cout << "\nRedirecting you to previous menu\n";
			system("pause");
			medStrgModule.nurseModuleMenu();
		}
		else {
			cout << "\n\x1B[31mInvalid input, please try again\033[0m\n";
			system("pause");
			MedsMgmntMenu();
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
	cout << "\x1B[96mPlease enter the medicine's name exactly as the packaging\033[0m\n\n";

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
	cout << "\n\x1B[96mPlease provide more information\n\033[0m\n";

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
	cout << "\n\x1B[96mMedication description include what is the purpose of the medication\n\033[0m";
	cout << "Enter medicine's description : ";
	cin.clear();
	cin.ignore(INT_MAX, '\n');
	getline(cin, medDesc);

	//meds dosage
	do {

		float dosage;

		cout << "\n\x1B[96mPlease enter the medication's dosage per serving in miligram (mg)\033[0m";
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
		cout << "\n\x1B[96mPlease enter medication price\033[0m";
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
		cout << "\n\x1B[96mIs the medicine a sedative?\033[0m\n\n";
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

	cout << "\n\x1B[96mPlease enter medication brand\033[0m";
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
	}
	else {
		cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}
}

//function to search meds for edit or remove user
string* MedMgmntSub::medsSearch() {

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
	string* medsData = new string[8];

	//looping through column from database
	for (int i = 0; i < 8; i++) {
		medsData[i] = row[i];
	}

	return medsData;
}

//function that is called from medsMgmntMenu() function
//asking user what they want to update
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

		cout << "\n\x1B[96mSelect Column you want to edit\033[0m\n\n";
		cout << "1 - Meds Price\n";
		cout << "2 - Meds Description\n";
		cout << "3 - Return to medicine management menu\n";
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
			MedsMgmntMenu();
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
		cout << "\n\x1B[96mPlease enter new medication price\033[0m";
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
	MedsMgmntMenu();
}

//Function to update meds description
void MedMgmntSub::medsUpdateDesc(string* medsData) {
	string medDesc;

	cout << "\n\nCurrent medicine's description : " << medsData[5] << endl;
	cout << "\n\x1B[96mPlease enter medication description\033[0m";
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
	MedsMgmntMenu();
}

//function to remove medicine record from DB
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
		cout << "\x1B[33mBy deleting this medicines, the record of stocks inside the inventory \033[0m\n";
		cout << "\x1B[33mand prescription that have this medicine also will be deleted\033[0m\n";
		cout << "\x1B[33mIt is inadvisable to delete this medicine. Please make sure all inventory matches your current action\033[0m\n";
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
				MedsMgmntMenu();
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
			MedsMgmntMenu();
			break;
		default:
			cout << "\n\x1B[31mInvalid input, please try again\033[0m\n";
			system("pause");
			medsRemove(medsData);
		}

	} while (continueLoop);
}

//this function is called from medsMgmntMenu() function
//if the user select view medicine list option and search by medicine properties
//this function will ask what thier next action
void MedMgmntSub::medsNextAction(string* medsData){

	int choice = 0;

	cout << "\n\x1B[96mPlease select your next action\033[0m\n\n";
	cout << "1 - Update medicine\n";
	cout << "2 - Remove medicine\n";
	cout << "3 - Go back to medicine menu\n\n";
	cout << "Enter your choice : ";
	cin >> choice;

	if (cin.fail()) {
		cin.clear();
		cin.ignore();
		cout << "\n\x1B[31mInvalid input, please try again\033[0m\n";
		system("pause");
		medsNextAction(medsData);
	}
	else {

		switch (choice) {
		case 1:
			medsUpdate(medsData);
			break;
		case 2 :
			medsRemove(medsData);
			break;
		case 3:
			MedsMgmntMenu();
			break;
		default:
			medsNextAction(medsData);
			break;
		}
	}
}

//this function is to search the properties of the medicine record in DB
//such as medsID, medsName, medsDesc, medsBrand
//and will ask user for selected meds and past the data to medsNextAction
void MedMgmntSub::medsGeneralSearch(){

	medsMgmntView();

	string searchProperty;

	cout << "\n\x1B[96mYou can search meds from the database via their medsID, name, description and brand\033[0m\n\n";
	cout << "Enter your medicine properties : ";
	cin >> searchProperty;

	string query =
		"SELECT medsID, medsName, medsDosage, medsType, medsPrice, medsBrand FROM medicines "
		"WHERE medsID LIKE '%" + searchProperty + "%' "
		"OR medsName LIKE '%" + searchProperty + "%' "
		"OR medsDesc LIKE '%" + searchProperty + "%' "
		"OR medsBrand LIKE '%" + searchProperty + "%' ";

	const char* cu = query.c_str();
	conState = mysql_query(connection, cu);

	if (!conState) {
		res = mysql_store_result(connection);
		int num_fields = mysql_num_fields(res);

		if (num_fields > 0) {

			system("pause");
			system("cls");
			art.logoArt();
			art.directoryArt("MSMM/Meds Management sub-module/Search medicine");

			clitable::Table table;
			clitable::Column c[6] = {
					clitable::Column("Meds ID",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
					clitable::Column("Meds Name",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 35, clitable::Column::NON_RESIZABLE),
					clitable::Column("Meds  Dosage",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 6, clitable::Column::NON_RESIZABLE),
					clitable::Column("Meds Type",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 11, clitable::Column::NON_RESIZABLE),
					clitable::Column("Meds Price",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 11, clitable::Column::NON_RESIZABLE),
					clitable::Column("Meds Brand",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 25, clitable::Column::NON_RESIZABLE)
			};

			for (int i = 0; i < 6; i++) table.addColumn(c[i]);

			while (row = mysql_fetch_row(res)) {
				vector<string>row_data;
				for(int i = 0; i < num_fields; i++) row_data.push_back(row[i] ? row[i] : "NULL");
				string* medsData = &row_data[0];

				table.addRow(medsData);
			}
			cout << table.draw() << endl;
			mysql_free_result(res);
		}
		else {
			cout << "\n\x1B[31mMedicine doesn't exist within such search properties. Please try again\033[0m\n";
			system("pause");
			medsGeneralSearch();
		}
	}
	else {
		cout << "\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}
	
	do {
		char option;
		cout << "\n\x1B[96mDo you found your medicine properties?\033[0m\n\n";
		cout << "Enter your choice (Y/N) : ";
		cin >> option;

		switch (option) {
		case'y':
		case'Y':
			break;
		case'n':
		case 'N':
			cout << "\n\x1B[96mRedirecting you back to search page\033[0m\n";
			system("pause");
			medsGeneralSearch();
			break;
		default :
			cout << "\x1B[31m\nInvalid input, please try again\033[0m\n" << mysql_errno(connection) << endl;
			system("pause");
			medsGeneralSearch();
		}
		break;
	} while (true);

	do {

		string medsID;
		cout << "\nPlease enter medsID : ";
		cin >> medsID;	

		string selectQuery = "SELECT * FROM medicines WHERE medsID ='" + medsID + "'";
		const char* sq = selectQuery.c_str();
		conState = mysql_query(connection, sq);

		if (!conState) {
			res = mysql_store_result(connection);
			int numfields = mysql_num_fields(res);

			if (mysql_num_rows(res) > 0) {

				row = mysql_fetch_row(res);
				string* medsData = new string[8];
				
				for (int i = 0; i < numfields; i++) {

					medsData[i] = row[i];
				}
				medsNextAction(medsData);
				break;
			}
			else {
				cout << "\n\x1B[31mMedicine doesn't exist. Please try again\033[0m\n";
				system("pause");
				medsGeneralSearch();
			}
		}
		else {
			cout << "\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
			exit(0);
		}
	} while (true);
}