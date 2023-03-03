#pragma once
#include "PyHelper.h"
#include <vector>
#include <string>

class PythonConnector
{
public:
	PythonConnector();
private:
	CPyInstance hInstance;
	CPyObject pythonClass;
	CPyObject object;
	bool ErrorCondition;
public:
	int predictDigit();
};