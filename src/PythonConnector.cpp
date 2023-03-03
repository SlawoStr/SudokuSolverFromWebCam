#include "PythonConnector.h"
#include <iostream>

PythonConnector::PythonConnector()
{
	// Python script name without .py
	CPyObject pName = PyUnicode_FromString("DigitRecognizer");
	// Import module
	CPyObject pModule = PyImport_Import(pName);

	CPyObject dict;

	ErrorCondition = false;
	if (pModule)
	{
		//Get dictionary object that implements module's namespace (__dict__)
		dict = PyModule_GetDict(pModule);
		if (dict == nullptr) {
			PyErr_Print();
			std::cerr << "Fails to get the dictionary.\n";
			ErrorCondition = true;
		}
		else
		{
			object = PyObject_GetAttrString(pModule, "check_digit");
		}
	}
	else
	{
		std::cerr << "ERROR: Module not imported\n" << std::endl;
		ErrorCondition = true;
	}
}

int PythonConnector::predictDigit()
{
	CPyObject value = PyObject_CallFunction(object, "");
	return PyLong_AsLong(value);
}
