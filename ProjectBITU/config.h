#ifndef CONFIG_H
#define CONFIG_H

#include <iostream>
#include <string>

using namespace std;

class config {

public :
	string currentUserID;

	string getCurrentUserID() {
		return currentUserID;
	}

	void setCurrentUserID(string currentUserID) {
		this->currentUserID = currentUserID;
	}
};
#endif // !CONFIG_H
