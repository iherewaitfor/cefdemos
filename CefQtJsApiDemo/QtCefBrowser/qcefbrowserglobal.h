#ifndef QCEFBROWSERGLOBAL_H
#define QCEFBROWSERGLOBAL_H

#ifdef QCEFBROWSER_IMPL
	#define QCEFBROWSER_DECL_EXPORT __declspec(dllexport)
#else 
	#define QCEFBROWSER_DECL_EXPORT __declspec(dllimport)
#endif 

#define QCEFBROWSER_DECLARE_PRIVATE(Class)                                                                               \
    Class##Private* d_ptr;                                                                                             \
    Q_DECLARE_PRIVATE(Class);                                                                                          \
    friend class Class##Private;

#endif