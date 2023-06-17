#ifndef QCEFBROWSERGLOBAL_H
#define QCEFBROWSERGLOBAL_H

#ifdef QCEFBROWSER_IMPL
	#define QCEFBROWSER_DECL_EXPORT __declspec(dllexport)
#else 
	#define QCEFBROWSER_DECL_EXPORT __declspec(dllimport)
#endif 

#endif