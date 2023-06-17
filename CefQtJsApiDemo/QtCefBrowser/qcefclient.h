#ifndef QCEFCLIENT_H
#define QCEFCLIENT_H
#include "qcefbrowserglobal.h"
#include <QtCore>

class QCEFBROWSER_DECL_EXPORT QCefClient : public QObject
{
    Q_OBJECT;
public:
    explicit QCefClient(QObject* parent = nullptr);
    ~QCefClient();
};

#endif

