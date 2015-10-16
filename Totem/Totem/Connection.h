#pragma once
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#define string std::string

class CConnection{

public:
	CConnection();
	~CConnection();

	void Intialise();

private:

	string GetHostList(string cmd);

};