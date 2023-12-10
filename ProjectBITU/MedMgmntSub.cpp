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
void MedMgmntSub::MedMgmntMenu(){

	system("cls");
	art.logoArt();
	art.directoryArt("MSMM/Meds Management sub-module");

	cout << "\x1B[94mPlease select your next action\033[0m\n\n";

	cout << "1 - Add new medications\n";
	cout << "2 - View existing staff\n";
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
		MedMgmntMenu();
	}
	else {
		switch (option) {
		case 1:
			MedMgmntAdd();
			break;
		case 2:
			medMgmntView();
			break;
		case 3:
			cout << "\nRedirecting you back to previous menu\n";
			system("pause");
			medStrgModule.nurseModuleMenu();
			break;
		case 4:
			system("pause");
			if (!auth.logout()) {
				MedMgmntMenu();
			}
			break;

		}
	}
}

//function to add new medicine to database
void MedMgmntSub::MedMgmntAdd() {

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
		MedMgmntMenu();
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
		case 1 :
			medType = "Tablet";
			continueLoop = false;
			break;
		case 2 :
			medType = "Capsule";
			continueLoop = false;
			break;
		case 3 :
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
	getline(cin , medDesc);
	
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
		float price = 0;
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
		MedMgmntMenu();
	}
	else {
		cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}
}

//function to display meds list
void MedMgmntSub::medMgmntView(){
	
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
		for (int i = 0; i < 7; i++) {table.addColumn(c[i]);}

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


		//debug
		system("pause");
	}
}

