#pragma once
#include <Python.h>

class CPyInstance
{
public:
	CPyInstance()
	{
		//You can check required paths by starting certain python script in pycharm. On the top screen of terminal you will see all paths that were used to start script. Copy it and add to this path. Also you must add path to folder(in this its relase)
		Py_SetPath(L"C:\\Users\\sss\\anaconda3\\lib;C:\\Projects\\SudokuSolverFromWebCam\\src\\python;C:\\Users\\sss\\anaconda3\\DLLs;C:\\Users\\sss\\anaconda3\\lib\\site-packages");
		Py_Initialize();
	}
	~CPyInstance()
	{
		Py_Finalize();
	}
};

class CPyObject
{
private:
	PyObject* p;
public:
	CPyObject() : p(NULL)
	{}

	CPyObject(PyObject* _p) : p(_p)
	{}


	~CPyObject()
	{
		Release();
	}

	PyObject* getObject()
	{
		return p;
	}

	PyObject* setObject(PyObject* _p)
	{
		return (p = _p);
	}

	PyObject* AddRef()
	{
		if (p)
		{
			Py_INCREF(p);
		}
		return p;
	}

	void Release()
	{
		if (p)
		{
			Py_DECREF(p);
		}

		p = NULL;
	}

	PyObject* operator ->()
	{
		return p;
	}

	bool is()
	{
		return p ? true : false;
	}

	operator PyObject* ()
	{
		return p;
	}

	PyObject* operator = (PyObject* pp)
	{
		p = pp;
		return p;
	}

	operator bool()
	{
		return p ? true : false;
	}
};