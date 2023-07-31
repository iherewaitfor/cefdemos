#pragma once

#include <QtCore>
#include "qcefv8bindroglobal.h"

class QObject;
class QCefV8BindAppROPrivate;
class QCEFV8BINDRO_DECL_EXPORT QCefV8BindAppRO
{
public:
    Q_DISABLE_COPY(QCefV8BindAppRO);
    QCEFV8BINDRO_DECLARE_PRIVATE(QCefV8BindAppRO);

    QCefV8BindAppRO();
    virtual ~QCefV8BindAppRO(void);

    void setV8RootObject(QPointer<QObject> o);

    void enableRemoting(QObject* apiRemotObject);

    static QCefV8BindAppRO* getInstance();

};
