#ifndef QCEFV8BINDROGLOBAL_H
#define QCEFV8BINDROGLOBAL_H

#ifdef QCEFV8BINDRO_IMPL
	#define QCEFV8BINDRO_DECL_EXPORT __declspec(dllexport)
#else 
	#define QCEFV8BINDRO_DECL_EXPORT __declspec(dllimport)
#endif 

#define QCEFV8BINDRO_DECLARE_PRIVATE(Class)                                                                               \
    Class##Private* d_ptr;                                                                                             \
    Q_DECLARE_PRIVATE(Class);                                                                                          \
    friend class Class##Private;

#define QCEFV8BINDRO_DECLARE_PUBLIC(Class)                                                                                \
    Class* q_ptr;                                                                                                      \
    Q_DECLARE_PUBLIC(Class);                                                                                           \
    friend class Class;

#endif