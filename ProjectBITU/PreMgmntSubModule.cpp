#include "PreMgmntSubModule.h"
#include <iostream>
#include <string>
#include "Artwork.h"
#include "MedStrgModule.h"
#include "Auth.h"
#include "Misc.h"
#include "table.h"

using namespace std;

extern Artwork art;
extern Auth auth;
extern Misc misc;
extern MedStrgModule medStrgModule;

//this function displaying the prescription menu
void PreMgmntSubModule::preMgmntMenu() {
	system("cls");
	art.logoArt();
	art.directoryArt("MSMM/Prescription Management sub-module");

	cout << "\x1B[94mPlease select your next action\033[0m\n\n";

	cout << "1 - Add new prescription\n";
	cout << "2 - Remove prescription\n";
	cout << "3 - View prescription\n";
	cout << "4 - Go back to previous menu\n";
	cout << "5 - Log out\n";

	cout << "\nPlease enter your choice : ";

	int option;
	cin >> option;

	if (cin.fail()) {
		cin.clear();
		cin.ignore();
		cout << "\n\n\033[1;31mInvalid input, please try again\033[0m\n\n";
		system("pause");
		preMgmntMenu();
	}
	else {
		switch (option) {
		case 1:
			addPrescription();
			break;
		case 2:
			removePrescription();

			break;
		case 3:
			//view prescription
			break;
		case 4:
			cout << "\nRedirecting you to previous menu\n";
			system("pause");
			medStrgModule.nurseModuleMenu();
			break;
		case 5:
			system("pause");
			if (!auth.logout()) {
				preMgmntMenu();
			}
			break;
		}
	}
}

//function handle adding new prescription
//the user will be asked to enter the resident ID
//and will be shown with medicine list
//the user will be asked to enter the prescription info
void PreMgmntSubModule::addPrescription() {

	//confimation
	do {
		char confirm;
		cout << "\n\x1B[94mYou are about to create new prescription\033[0m\n";
		cout << "\x1B[94mDo you want to proceed this action?\033[0m\n";
		cout << "\nEnter your choice (Y/N) : ";
		cin >> confirm;

		if (confirm == 'Y' || confirm == 'y') {
			break;
		}
		else if (confirm == 'N' || confirm == 'n') {
			cout << "\nRedirecting you to previous menu\n";
			system("pause");
			preMgmntMenu();
		}
		else {
			cout << "\n\033[1;31mInvalid input, please try again\033[0m\n";
			system("pause");
		}
	} while (true);

	system("cls");
	art.logoArt();
	art.directoryArt("MSMM/Prescription Management sub-module/Add new prescription");

	//initialize variables for storing prescription info
	string preID, resID, medID, quantity, timePerDay, instruction;

	//resident ID
	clitable::Table residentTable;
	clitable::Column rc[4] = {
		clitable::Column("Resident ID",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
		clitable::Column("Resident IC",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 19, clitable::Column::NON_RESIZABLE),
		clitable::Column("Resident Name",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 67, clitable::Column::NON_RESIZABLE),
		clitable::Column("Room Number",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 11, clitable::Column::NON_RESIZABLE)
	};

	for (int i = 0; i < 4; i++) { residentTable.addColumn(rc[i]); }

	//defining sql statement to get resident information
	string getResident = "SELECT resID, resIC, resName, resRoomNum FROM residents";
	const char* gr = getResident.c_str();
	conState = mysql_query(connection, gr);

	if (!conState) {
		res = mysql_store_result(connection);

		while (row = mysql_fetch_row(res)) {
			vector<string> resident;
			for (int i = 0; i < 5; i++) { resident.push_back(row[i] ? row[i] : "NULL"); }
			string* residentData = &resident[0];
			residentTable.addRow(residentData);
		}
		mysql_free_result(res);
	}
	else {
		cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}
	cout << residentTable.draw() << endl;

	do {
		cout << "\n\x1B[94mPlease select the resident you want to prescribe\033[0m\n";
		cout << "Enter resident ID : ";
		cin >> resID;
		cin.clear();
		cin.ignore(INT_MAX, '\n');

		//define sql statement to find selected resident
		string findResident = "SELECT resID FROM residents "
			"WHERE resID = '" + resID + "'";
		const char* fr = findResident.c_str();
		conState = mysql_query(connection, fr);

		if (!conState) {
			res = mysql_store_result(connection);
			if (mysql_num_rows(res) > 0) {
				cout << "\n\x1B[32mResident found\033[0m\n";
				system("pause");
				break;
			}
			else {
				cout << "\n\x1B[31mSelected resident was not found, Please try again\033[0m\n\n";
				system("pause");
			}
		}
		else {
			cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
			exit(0);
		}
	} while (true);

	system("cls");
	art.logoArt();
	art.directoryArt("MSMM/Prescription Management sub-module/Add new prescription");

	//medicine ID
	clitable::Table medicineTable;
	clitable::Column c[6] = {
			clitable::Column("Meds ID",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
			clitable::Column("Meds Name",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 34, clitable::Column::NON_RESIZABLE),
			clitable::Column("Meds  Dosage",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 6, clitable::Column::NON_RESIZABLE),
			clitable::Column("Meds Type",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 11, clitable::Column::NON_RESIZABLE),
			clitable::Column("Meds Brand",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 25, clitable::Column::NON_RESIZABLE),
			clitable::Column("Sedative",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 13, clitable::Column::NON_RESIZABLE)
	};

	for (int i = 0; i < 6; i++) { medicineTable.addColumn(c[i]); }

	//defining sql statement to get medicine information
	string getMedicine = "SELECT medsID, medsName, medsDosage, medsType, medsBrand, "
		"CASE WHEN isSedative = 1 THEN 'Sedative' ELSE 'Not Sedative' END AS SedativeStatus "
		"FROM medicines ";
	const char* gm = getMedicine.c_str();
	conState = mysql_query(connection, gm);

	if (!conState) {
		res = mysql_store_result(connection);

		while (row = mysql_fetch_row(res)) {
			vector<string> medicine;
			for (int i = 0; i < 6; i++) { medicine.push_back(row[i] ? row[i] : "NULL"); }
			string* medicineData = &medicine[0];
			medicineTable.addRow(medicineData);
		}
		mysql_free_result(res);
	}
	else {
		cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}
	cout << medicineTable.draw() << endl;

	do {
		cout << "\n\x1B[94mPlease select the medicine you want to prescribe\033[0m\n";
		cout << "Enter medicines ID : ";
		cin >> medID;
		cin.clear();
		cin.ignore(INT_MAX, '\n');

		//define sql statement to find selected medicine
		string findMedicine = "SELECT medsID FROM medicines "
			"WHERE medsID = '" + medID + "'";
		const char* fm = findMedicine.c_str();
		conState = mysql_query(connection, fm);

		if (!conState) {
			res = mysql_store_result(connection);
			if (mysql_num_rows(res) > 0) {
				cout << "\n\x1B[32mMedicine found\033[0m\n";
				system("pause");
				break;
			}
			else {
				cout << "\n\x1B[31mSelected medicine was not found, Please try again\033[0m\n\n";
				system("pause");
			}
		}
		else {
			cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
			exit(0);
		}
	} while (true);

	//presciption quantity
	do {
		cout << "\n\x1B[94mPlease enter the quantity of medicine each presciption\033[0m\n";
		cout << "Enter quantity : ";
		cin >> quantity;
		cin.clear();
		cin.ignore(INT_MAX, '\n');

		if (misc.isNumeric(quantity)) {
			break;
		}
		else {
			cout << "\n\x1B[31mInvalid input, please try again\033[0m\n";
			system("pause");
		}
	} while (true);

	//prescription time per day
	do {
		cout << "\n\x1B[94mPlease specify the number of times per day the medication should be taken\033[0m\n";
		cout << "Enter times per day : ";
		cin >> timePerDay;
		cin.clear();
		cin.ignore(INT_MAX, '\n');

		if (misc.isNumeric(timePerDay)) {
			break;
		}
		else {
			cout << "\n\x1B[31mInvalid input, please try again\033[0m\n";
			system("pause");
		}
	} while (true);

	//prescription instruction
	cout << "\n\x1B[94mPlease enter specific instruction for this prescription\033[0m\n";
	cout << "Enter instruction : ";
	getline(cin, instruction);

	//generate prescription ID
	string getLastID = "SELECT prescriptID FROM prescriptions ORDER BY prescriptID DESC LIMIT 1";
	const char* gli = getLastID.c_str();
	conState = mysql_query(connection, gli);

	if (!conState) {
		string passedID;
		res = mysql_store_result(connection);
		row = mysql_fetch_row(res);
		if (row) {
			passedID = row[0];
		}
		else {
			passedID = "0000";
		}
		preID = misc.createID(passedID, "PCT");
	}
	else {
		cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}

	//define sql statement to insert new prescription
	string insertPrescription =
		"INSERT INTO prescriptions("
		"prescriptID , resID, medsID , quantity , timePerDay , instruction ) "
		"VALUES('"
		+ preID + "', '" + resID + "', '" + medID + "', '"
		+ quantity + "', '" + timePerDay + "', '" + instruction + "')";
	const char* ip = insertPrescription.c_str();
	conState = mysql_query(connection, ip);

	if (!conState) {
		cout << "\n\x1B[32mPrescription added successfully\033[0m\n";
		system("pause");
		preMgmntMenu();
	}
	else {
		cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}
}

//function handle removing prescription
//the user will be shown a list of resident with prescription and need to choose one
//the user will be shown a list of prescription of the selected resident and need to choose one
//the user will be asked to confirm the action
void PreMgmntSubModule::removePrescription(){
	
	system("cls");
	art.logoArt();
	art.directoryArt("MSMM/Prescription Management sub-module/Remove prescription");

	string resID, preID;

	//resident ID
	clitable::Table residentTable;
	clitable::Column rc[4] = {
		clitable::Column("Resident ID",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
		clitable::Column("Resident IC",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 19, clitable::Column::NON_RESIZABLE),
		clitable::Column("Resident Name",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 67, clitable::Column::NON_RESIZABLE),
		clitable::Column("Room Number",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 11, clitable::Column::NON_RESIZABLE)
	};

	for (int i = 0; i < 4; i++) { residentTable.addColumn(rc[i]); }

	//defining sql statement to get resident information
	string getResident = "SELECT residents.resID, residents.resIC, residents.resName, residents.resRoomNum "
		"FROM residents "
		"JOIN prescriptions ON residents.resID = prescriptions.resID ";
	const char* gr = getResident.c_str();
	conState = mysql_query(connection, gr);

	if (!conState) {
		res = mysql_store_result(connection);

		while (row = mysql_fetch_row(res)) {
			vector<string> resident;
			for (int i = 0; i < 5; i++) { resident.push_back(row[i] ? row[i] : "NULL"); }
			string* residentData = &resident[0];
			residentTable.addRow(residentData);
		}
		mysql_free_result(res);
	}
	else {
		cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}
	cout << residentTable.draw() << endl;

	do {
		cout << "\n\x1B[94mPlease select the resident you want to prescribe\033[0m\n";
		cout << "Enter resident ID : ";
		cin >> resID;
		cin.clear();
		cin.ignore(INT_MAX, '\n');

		//define sql statement to find selected resident
		string findResident = "SELECT residents.resID "
			"FROM residents "
			"WHERE residents.resID = '" + resID + "' "
			"AND EXISTS (SELECT 1 FROM prescriptions "
			"WHERE prescriptions.resId = '" + resID + "')";

		const char* fr = findResident.c_str();
		conState = mysql_query(connection, fr);

		if (!conState) {
			res = mysql_store_result(connection);
			if (mysql_num_rows(res) > 0) {
				cout << "\n\x1B[32mResident found\033[0m\n";
				system("pause");
				break;
			}
			else {
				cout << "\n\x1B[31mSelected resident was not found, Please try again\033[0m\n\n";
				system("pause");
			}
		}
		else {
			cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
			exit(0);
		}
	} while (true);

	system("cls");
	art.logoArt();
	art.directoryArt("MSMM/Prescription Management sub-module/Remove prescription");

	//prescription ID
	clitable::Table prescriptionTable;
	clitable::Column column[4]{
		clitable::Column("Prescription ID",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 15, clitable::Column::NON_RESIZABLE),
		clitable::Column("Resident Name",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 35, clitable::Column::NON_RESIZABLE),
		clitable::Column("Medicine Name",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 35, clitable::Column::NON_RESIZABLE),
		clitable::Column("Quantity",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE)
	};

	for (int i = 0; i < 4; i++) { prescriptionTable.addColumn(column[i]); }

	//defining sql statement to get prescription information
	string getPrescription = R"(
	SELECT 
		prescriptions.prescriptID,
		residents.resName,
		medicines.medsName,
		prescriptions.quantity
		
	FROM 
		prescriptions
	JOIN 
		medicines ON prescriptions.medsID = medicines.medsID
	JOIN 
		residents ON prescriptions.resID = residents.resID
	WHERE 
		residents.resID = 'RST0001'
	)";
	const char* gp = getPrescription.c_str();
	conState = mysql_query(connection, gp);

	if (!conState) {
		res = mysql_store_result(connection);

		while (row = mysql_fetch_row(res)) {
			vector<string> prescription;
			for (int i = 0; i < 4; i++) { prescription.push_back(row[i] ? row[i] : "NULL"); }
			string* prescriptionData = &prescription[0];
			prescriptionTable.addRow(prescriptionData);
		}
		mysql_free_result(res);
	}
	else {
		cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}
	cout << prescriptionTable.draw() << endl;
	
	do {
		cout << "\n\x1B[94mPlease select the prescription you want to remove\033[0m\n";
		cout << "Enter prescription ID : ";
		cin >> preID;
		cin.clear();
		cin.ignore(INT_MAX, '\n');

		//define sql statement to find selected prescription
		string findPrescription = "SELECT prescriptions.prescriptID "
			"FROM prescriptions "
			"WHERE prescriptions.prescriptID = '" + preID + "' "
			"AND prescriptions.resID = '" + resID + "'";
		const char* fp = findPrescription.c_str();
		conState = mysql_query(connection, fp);

		if (!conState) {
			res = mysql_store_result(connection);
			if (mysql_num_rows(res) > 0) {
				cout << "\n\x1B[32mPrescription found\033[0m\n";
				system("pause");
				break;
			}
			else {
				cout << "\n\x1B[31mSelected prescription was not found, Please try again\033[0m\n\n";
				system("pause");
			}
		}
		else {
			cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
			exit(0);
		}
	} while (true);

	//Confirmation
	do {
		char confirm;
		cout << "\n\x1B[94mYou are about to remove prescription\033[0m\n";
		cout << "\x1B[94mDo you want to proceed this action?\033[0m\n";
		cout << "\nEnter your choice (Y/N) : ";
		cin >> confirm;
		
		if (confirm == 'Y' || confirm == 'y') {
			break;
		}
		else if (confirm == 'N' || confirm == 'n') {
			cout << "\nRedirecting you to previous menu\n";
			system("pause");
			preMgmntMenu();
		}
		else {
			cout << "\n\033[1;31mInvalid input, please try again\033[0m\n";
			system("pause");
		}
	} while (true);

	//define sql statement to remove selected prescription
	string removePrescription = "DELETE FROM prescriptions "
		"WHERE prescriptID = '" + preID + "'";
	const char* rp = removePrescription.c_str();
	conState = mysql_query(connection, rp);

	if (!conState) {
		cout << "\n\x1B[32mPrescription removed successfully\033[0m\n";
		system("pause");
		preMgmntMenu();
	}
	else {
		cout << "\n\x1B[31mQuery error\033[0m\n" << mysql_errno(connection) << endl;
		exit(0);
	}
}
