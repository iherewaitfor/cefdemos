#pragma once

#include <QtCore>
#include "qcefv8bindglobal.h"

class QObject;
class QCefV8BindAppPrivate;
class QCEFV8BIND_DECL_EXPORT QCefV8BindApp
{
public:
    Q_DISABLE_COPY(QCefV8BindApp);
    QCEFV8BIND_DECLARE_PRIVATE(QCefV8BindApp);

    QCefV8BindApp();
    virtual ~QCefV8BindApp(void);

    void setV8RootObject(QObject* o);

    static QCefV8BindApp* getInstance();
};
