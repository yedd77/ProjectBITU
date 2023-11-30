#ifndef AUTH_H
#define AUTH_H

#include<string>

using namespace std;

class Auth {

public:
	bool firstTimeSetup();
	void login();
	void hasPassChanged(string* userData);
	void seperateUserMenu(string* userData);
	bool logout();
};

#endif // !AUTH_H
