#include "DailyReportingSub.h"
#include "Artwork.h"
#include <iostream>
#include "SuperAdmin.h"
#include <string>
#include "Misc.h"
#include <vector>
#include "table.h"

using namespace std;

extern Artwork art;
SuperAdmin superAdmin;
extern Misc misc;

//this function is to display daily reporting menu
void DailyReportingSub::dailyReportMenu(){

	system("cls");
	art.logoArt();
	art.directoryArt("Main module/ Daily Reporting Menu");

	int choice;

	cout << "\x1B[96mPlease select your next action\033[0m\n\n";
	cout << "1 - View Prescriptions Report\n";
	cout << "2 - View Medicines Report\n";
	cout << "3 - Return to main menu\n";
	cout << "\nEnter your choice : ";
	cin >> choice;

	if (cin.fail()) {
		cin.clear();
		cin.ignore();
		cout << "\x1B[33mInvalid input, please try again\033[0m\n";
		system("pause");
		dailyReportMenu();
	}
	else {
		switch (choice) {
		case 1:
			displayPrescriptionReport();
			break;
		case 2:
			batchReportMenu();
			break;
		case 3:
			cout << "\nRedirecting you to main menu\n";
			system("pause");
			superAdmin.superAdminMenu();
			break;
		default:
			cout << "\x1B[33mInvalid input, please try again\033[0m\n";
			system("pause");
			dailyReportMenu();
		}
	}
}

//this function is called from dailyReportMenu() to display prescription report
void DailyReportingSub::displayPrescriptionReport() {

	//prompt user to enter the date of the report
	string date;

	do {
		cout << "\n\x1B[96mPlease enter the date of the report you want to view\033[0m\n";
		cout << "\x1B[96mThe date must be in correct format YYYY-MM-DD (Example 2023-10-23)\033[0m\n";
		cout << "\nEnter the date : ";
		cin >> date;

		if (misc.isValidDateFormat(date)) {
			break;
		}

		date.clear();
	} while (true);

	//check in the database if there is any prescription for the date entered
	string Selectquery = "SELECT "
		"CASE WHEN '" + date + "' < CURDATE() THEN TRUE ELSE FALSE END as isDatePassed, "
		"CASE WHEN COUNT(*) > 0 THEN 1 ELSE 0 END AS is_date_available "
		"FROM schedules "
		"WHERE date = '" + date + "'";
	const char* sq = Selectquery.c_str();
	conState = mysql_query(connection, sq);

	if (!conState) {

		res = mysql_store_result(connection);
		row = mysql_fetch_row(res);
		
		//check if the date entered is passed
		if (atoi(row[0]) == 0) {
			cout << "\n\x1B[31mTYou can't select future date, Please try again\033[0m\n\n";
			system("pause");
			displayPrescriptionReport();
		}

		//check if there is any prescription for the date entered
		if (atoi(row[1]) == 0) {
			cout << "\n\x1B[31mThere's no prescription on the date you have selected, Please try again\033[0m\n\n";
			system("pause");
			displayPrescriptionReport();
		}
	}
	else {
		cout << "\n\x1B[31mError, please try again\033[0m\n";
		exit(0);
	}

	//list all resident who have prescription on the date entered
	string selectResident = "SELECT "
		"Residents.resID, "
		"Residents.resIC, "
		"Residents.resName, "
		"Residents.resRoomNum, "
		"MAX(Schedules.date) AS maxDate "
		"FROM "
		"Residents "
		"JOIN "
		"Prescriptions ON Residents.resID = Prescriptions.resID "
		"JOIN "
		"Schedules ON Prescriptions.prescriptID = Schedules.prescriptID "
		"WHERE "
		"Schedules.date = '" + date + "' "
		"GROUP BY "
		"Residents.resID, Residents.resIC, Residents.resName, Residents.resRoomNum";
	const char* sr = selectResident.c_str();
	conState = mysql_query(connection, sr);

	if (!conState) {

		res = mysql_store_result(connection);

		system("cls");
		art.logoArt();
		art.directoryArt("Main module/DRM/Select Resident");

		clitable::Table residentTable;
		clitable::Table batchTable;
		clitable::Column bc[4] = {
			clitable::Column("Resident ID",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 12, clitable::Column::NON_RESIZABLE),
			clitable::Column("Resident IC",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 12, clitable::Column::NON_RESIZABLE),
			clitable::Column("Name",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 60, clitable::Column::NON_RESIZABLE),
			clitable::Column("Room   Number",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 13, clitable::Column::NON_RESIZABLE)
		};

		for (int i = 0; i < 4; i++) residentTable.addColumn(bc[i]);

		while (row = mysql_fetch_row(res)) {
			
			vector <string> residentRow;

			for(int i = 0 ; i < 4; i++) residentRow.push_back(row[i]);

			string* residentData = &residentRow[0];
			residentTable.addRow(residentData);
		}
		
		cout << residentTable.draw() << endl;
		
	}
	else {
		cout << "\n\x1B[31mError, please try again\033[0m\n";
		exit(0);
	}

	string selectedResID;
	do {
		cout << "\n\x1B[96mPlease enter the resident ID schedule history you want to view\033[0m\n";
		cout << "\nEnter the resident ID : ";
		cin >> selectedResID;

		//check if the resident ID entered is valid
		string selectedResQuery = "SELECT "
			"COUNT(Residents.resID) AS resID "
			"FROM "
			"Residents "
			"JOIN "
			"Prescriptions ON Residents.resID = Prescriptions.resID "
			"JOIN "
			"Schedules ON Prescriptions.prescriptID = Schedules.prescriptID "
			"WHERE "
			"Residents.resID = '" + selectedResID + "' "
			"AND Schedules.date = '" + date + "' ";
		const char* srq = selectedResQuery.c_str();
		conState = mysql_query(connection, srq);

		if (!conState) {
			res = mysql_store_result(connection);
			row = mysql_fetch_row(res);

			if (atoi(row[0]) == 0) {
				cout << "\n\x1B[31mInvalid resident ID, please try again\033[0m\n";
				system("pause");
				continue;
			}
			else {
				break;
			}
		}
		else {
			cout << "\n\x1B[31mError, please try again\033[0m\n";
			exit(0);
		}

	} while (true);

	//display the resident schedule history on the specified date
	clitable::Table scheduleTable;
	clitable::Column column[5] = {
		clitable::Column("Schedule ID",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 16, clitable::Column::NON_RESIZABLE),
		clitable::Column("Medicine Name",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 50, clitable::Column::NON_RESIZABLE),
		clitable::Column("Quantity",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
		clitable::Column("Time Given",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 10, clitable::Column::NON_RESIZABLE),
		clitable::Column("Status",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 10, clitable::Column::NON_RESIZABLE)
	};

	for (int i = 0; i < 5; i++) scheduleTable.addColumn(column[i]);

	string resSchedule = "SELECT "
		"Schedules.scheduleID, "
		"medicines.medsName, "
		"prescriptions.quantity, "
		"schedules.timeGiven, "
		"CASE "
		"WHEN Schedules.isGiven = 1 THEN 'Given' "
		"ELSE 'Missed' "
		"END AS Status, "
		"resName "
		"FROM "
		"Residents "
		"JOIN "
		"Prescriptions ON Residents.resID = Prescriptions.resID "
		"JOIN "
		"medicines ON medicines.medsID = Prescriptions.medsID "
		"JOIN "
		"Schedules ON Prescriptions.prescriptID = Schedules.prescriptID "
		"WHERE "
		"Residents.resID = '" + selectedResID + "' "
		"AND Schedules.date = '" + date + "' "
		"ORDER BY schedules.timeGiven ";

	const char* rs = resSchedule.c_str();
	conState = mysql_query(connection, rs);
	string resName;

	if (!conState) {
		res = mysql_store_result(connection);

		while (row = mysql_fetch_row(res)) {
			vector <string> batchRow;

			for (int i = 0; i < 5; i++) batchRow.push_back(row[i]);

			string* batchData = &batchRow[0];
			scheduleTable.addRow(batchData);
			resName = row[5];
		}
	}
	else {
		cout << "\n\x1B[31mError, please try again\033[0m\n";
		exit(0);
	}

	system("cls");
	art.logoArt();
	art.directoryArt("Main module/Select Resident/Schedule for " +resName+ " on " +date+ " ");

	cout << scheduleTable.draw() << endl;
	
	system("pause");
	cout << "\nRedirecting you to daily report menu\n";
	system("pause");
	dailyReportMenu();
}

//this function is called from dailyReportMenu() to display medicine inventory report menu
void DailyReportingSub::batchReportMenu(){

	system("cls");
	art.logoArt();
	art.directoryArt("Main module/DRM/Batch Report Menu");

	int choice;

	cout << "\x1B[96mPlease select your next action\033[0m\n\n";
	cout << "1 - View Current medicine's stock Report\n";
	cout << "2 - View expired medicine's batch\n";
	cout << "3 - View medicine's batch that are low in stock\n";
	cout << "4 - View medicine's batch that doesn't have stock\n";
	cout << "5 - Return to main menu\n";
	cout << "\nEnter your choice : ";
	cin >> choice;

	if (cin.fail()) {
		cin.clear();
		cin.ignore();
		cout << "\x1B[33mInvalid input, please try again\033[0m\n";
		system("pause");
		batchReportMenu();
	}
	else {
		switch (choice) {
		case 1:
			DisplayCurrentStock();
			break;
		case 2:
			displayExpiredBatch();
			break;
		case 3:
			lowStockBatch();
			break;
		case 4:
			displayzeroStockBatch();
			break;
		case 5:
			cout << "\nRedirecting you to daily report menu\n";
			system("pause");
			superAdmin.superAdminMenu();
			break;
		default:
			cout << "\x1B[33mInvalid input, please try again\033[0m\n";
			system("pause");
			batchReportMenu();
		}
	}
}

//this function is called from batchReportMenu() to display current stock 
// report with the number of batch that the medicine have
void DailyReportingSub::DisplayCurrentStock(){

	system("cls");
	art.logoArt();
	art.directoryArt("Main module/View current medicine stocks");

	clitable::Table medsTable;
	clitable::Column column[5] = {
		clitable::Column("Medicine ID",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 9, clitable::Column::NON_RESIZABLE),
		clitable::Column("Medicine Name",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 45, clitable::Column::NON_RESIZABLE),
		clitable::Column("Medicine Dosage",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 16, clitable::Column::NON_RESIZABLE),
		clitable::Column("Current Stock",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 16, clitable::Column::NON_RESIZABLE),
		clitable::Column("Number of Batch",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 16, clitable::Column::NON_RESIZABLE)
	};

	for (int i = 0; i < 5; i++) medsTable.addColumn(column[i]);

	string selectMedsQuery = "SELECT "
		"m.medsID, "
		"m.medsName, "
		"m.medsDosage, "
		"COALESCE(SUM(CASE WHEN b.batchExpiry >= CURRENT_DATE THEN b.batchQuantity ELSE 0 END), 0) AS totalStocks, "
		"COALESCE(COUNT(b.batchID), 0) AS numberOfBatches "
		"FROM "
		"Medicines m "
		"LEFT JOIN "
		"Batches b ON m.medsID = b.medsID "
		"GROUP BY "
		"m.medsID, m.medsName, m.medsDosage "
		"ORDER BY medsName ";
	const char* smq = selectMedsQuery.c_str();
	conState = mysql_query(connection, smq);

	if (!conState) {

		res = mysql_store_result(connection);

		while (row = mysql_fetch_row(res)) {
			vector <string> medsRow;

			for (int i = 0; i < 5; i++) medsRow.push_back(row[i]);

			string* medsData = &medsRow[0];
			medsTable.addRow(medsData);
		}
	}
	else {
		cout << "\n\x1B[31mError, please try again\033[0m\n";
		exit(0);
	}

	cout << medsTable.draw() << endl;
	system("pause");
	cout << "\nRedirecting you to batch report menu\n";
	system("pause");
	batchReportMenu();
}

//this function is called from batchReportMenu() to display expired batch report
void DailyReportingSub::displayExpiredBatch(){

	system("cls");
	art.logoArt();
	art.directoryArt("Main module/View expired batch medicine stock");

	//check if there's any expired batch
	string countExpired = "SELECT "
		"COUNT(*) AS rowCount "
		"FROM "
		"batches b "
		"JOIN medicines m ON b.medsID = m.medsID "
		"WHERE "
		"b.batchExpiry < CURRENT_DATE";
	const char* ce = countExpired.c_str();
	conState = mysql_query(connection, ce);

	res = mysql_store_result(connection);
	row = mysql_fetch_row(res);
	if (!conState) {
		if (stoi(row[0]) == 0) {
			cout << "\n\x1B[31mThere's no expired batch as today, please try again\033[0m\n";
			system("pause");
			batchReportMenu();
		}
	}
	else {
		cout << "\n\x1B[31mError, please try again\033[0m\n";
		exit(0);
	}

	clitable::Table batchTable;
	clitable::Column column[5] = {
		clitable::Column("Batch ID",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
		clitable::Column("Medicine Name",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 53, clitable::Column::NON_RESIZABLE),
		clitable::Column("Medicine Dosage",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 16, clitable::Column::NON_RESIZABLE),
		clitable::Column("Quantity",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
		clitable::Column("Expired date",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 16, clitable::Column::NON_RESIZABLE)
	};

	for (int i = 0; i < 5; i++) batchTable.addColumn(column[i]);

	string selectExpiredBatchQuery = "SELECT "
		"b.batchID, "
		"m.medsName, "
		"m.medsDosage, "
		"b.batchQuantity, "
		"b.batchExpiry "
		"FROM "
		"batches b "
		"JOIN medicines m ON b.medsID = m.medsID "
		"WHERE "
		"b.batchExpiry < CURRENT_DATE ";
	const char* sebq = selectExpiredBatchQuery.c_str();
	conState = mysql_query(connection, sebq);

	if (!conState) {
		
		res = mysql_store_result(connection);

		while (row = mysql_fetch_row(res)) {
			vector <string> batchRow;

			for (int i = 0; i < 5; i++) batchRow.push_back(row[i]);

			string* batchData = &batchRow[0];
			batchTable.addRow(batchData);
		}
	}
	else{
		cout << "\n\x1B[31mError, please try again\033[0m\n";
		exit(0);
	}

	cout << batchTable.draw() << endl;
	system("pause");
	cout << "\nRedirecting you to batch report menu\n";
	system("pause");
	batchReportMenu();
}

//this function is called from batchReportMenu() to display batch that are low in stock
void DailyReportingSub::lowStockBatch(){

	system("cls");
	art.logoArt();
	art.directoryArt("Main module/View low batch medicine stock");

	string countBatchLow = "SELECT COUNT(B.batchID) AS NumberOfBatches "
		"FROM Batches B "
		"JOIN Medicines M ON B.medsID = M.medsID "
		"WHERE B.batchQuantity <= 30 ";
	const char* cbl = countBatchLow.c_str();
	conState = mysql_query(connection, cbl);

	if (!conState) {
			res = mysql_store_result(connection);
		row = mysql_fetch_row(res);

		if (atoi(row[0]) == 0) {
			cout << "\n\x1B[31mThere's no batch that are low in stock, please try again\033[0m\n";
			system("pause");
			batchReportMenu();
		}
	}
	else {
		cout << "\n\x1B[31mError, please try again\033[0m\n";
		exit(0);
	}

	clitable::Table batchTable;
	clitable::Column column[5] = {
		clitable::Column("Batch ID",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
		clitable::Column("Medicine Name",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 53, clitable::Column::NON_RESIZABLE),
		clitable::Column("Medicine Dosage",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 16, clitable::Column::NON_RESIZABLE),
		clitable::Column("Quantity",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
		clitable::Column("Expired date",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 16, clitable::Column::NON_RESIZABLE)
	};

	for (int i = 0; i < 5; i++) batchTable.addColumn(column[i]);

	string selectExpiredBatchQuery = "SELECT "
		"b.batchID, "
		"m.medsName, "
		"m.medsDosage, "
		"b.batchQuantity, "
		"b.batchExpiry "
		"FROM "
		"batches b "
		"JOIN medicines m ON b.medsID = m.medsID "
		"WHERE B.batchQuantity <= 30 ";
	const char* sebq = selectExpiredBatchQuery.c_str();
	conState = mysql_query(connection, sebq);

	if (!conState) {
		res = mysql_store_result(connection);

		while (row = mysql_fetch_row(res)) {
			vector <string> batchRow;

			for (int i = 0; i < 5; i++) batchRow.push_back(row[i]);

			string* batchData = &batchRow[0];
			batchTable.addRow(batchData);
		}
	}
	else {
		cout << "\n\x1B[31mError, please try again\033[0m\n";
		exit(0);
	}

	cout << batchTable.draw() << endl;
	system("pause");
	cout << "\nRedirecting you to batch report menu\n";
	system("pause");
	batchReportMenu();
}

//this function is called from batchReportMenu() to display batch that doesn't have stock
void DailyReportingSub::displayzeroStockBatch(){

	system("cls");
	art.logoArt();
	art.directoryArt("Main module/View zero batch medicine stock");

	string countBatchZero = "SELECT COUNT(B.batchID) AS NumberOfBatches "
		"FROM Batches B "
		"JOIN Medicines M ON B.medsID = M.medsID "
		"WHERE B.batchQuantity = 0 ";
	const char* cbz = countBatchZero.c_str();
	conState = mysql_query(connection, cbz);

	if (!conState) {
		res = mysql_store_result(connection);
		row = mysql_fetch_row(res);

		if (atoi(row[0]) == 0) {
			cout << "\n\x1B[31mThere's no batch that doesn't have stock, please try again\033[0m\n";
			system("pause");
			batchReportMenu();
		}
	}
	else {
		cout << "\n\x1B[31mError, please try again\033[0m\n";
		exit(0);
	}

	clitable::Table batchTable;
	clitable::Column column[5] = {
		clitable::Column("Batch ID",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
		clitable::Column("Medicine Name",  clitable::Column::CENTER_ALIGN, clitable::Column::LEFT_ALIGN, 1, 53, clitable::Column::NON_RESIZABLE),
		clitable::Column("Medicine Dosage",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 16, clitable::Column::NON_RESIZABLE),
		clitable::Column("Quantity",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 8, clitable::Column::NON_RESIZABLE),
		clitable::Column("Expired date",  clitable::Column::CENTER_ALIGN, clitable::Column::CENTER_ALIGN, 1, 16, clitable::Column::NON_RESIZABLE)
	};

	for (int i = 0; i < 5; i++) batchTable.addColumn(column[i]);

	string selectExpiredBatchQuery = "SELECT "
		"b.batchID, "
		"m.medsName, "
		"m.medsDosage, "
		"b.batchQuantity, "
		"b.batchExpiry "
		"FROM "
		"batches b "
		"JOIN medicines m ON b.medsID = m.medsID "
		"WHERE B.batchQuantity = 0 ";
	const char* sebq = selectExpiredBatchQuery.c_str();
	conState = mysql_query(connection, sebq);

	if (!conState) {
		res = mysql_store_result(connection);

		while (row = mysql_fetch_row(res)) {
			vector <string> batchRow;

			for (int i = 0; i < 5; i++) batchRow.push_back(row[i]);

			string* batchData = &batchRow[0];
			batchTable.addRow(batchData);
		}
	}
	else {
		cout << "\n\x1B[31mError, please try again\033[0m\n";
		exit(0);
	}

	cout << batchTable.draw() << endl;
	system("pause");
	cout << "\nRedirecting you to batch report menu\n";
	system("pause");
	batchReportMenu();
}
