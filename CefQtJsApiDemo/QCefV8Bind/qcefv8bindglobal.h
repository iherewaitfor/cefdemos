#ifndef QCEFV8BINDGLOBAL_H
#define QCEFV8BINDGLOBAL_H

#ifdef QCEFV8BIND_IMPL
	#define QCEFV8BIND_DECL_EXPORT __declspec(dllexport)
#else 
	#define QCEFV8BIND_DECL_EXPORT __declspec(dllimport)
#endif 

#define QCEFV8BIND_DECLARE_PRIVATE(Class)                                                                               \
    Class##Private* d_ptr;                                                                                             \
    Q_DECLARE_PRIVATE(Class);                                                                                          \
    friend class Class##Private;

#define QCEFV8BIND_DECLARE_PUBLIC(Class)                                                                                \
    Class* q_ptr;                                                                                                      \
    Q_DECLARE_PUBLIC(Class);                                                                                           \
    friend class Class;

#endif