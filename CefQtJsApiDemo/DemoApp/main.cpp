#include <QtCore>
#include <QApplication>
#include "..\QtCefBrowser\qcefclient.h"
#include "..\QtCefBrowser\qcefcoreapp.h"
#include <QtWidgets>

#include "numberlogic.h"
#include "subnumberlogic.h"
#include "..\QCefV8Bind\qcefv8bindapp.h"

int main(int argc, char** argv )
{
    QApplication app(argc, argv);

    QCefClient cefClient;
    cefClient.initCef();
    QCefV8BindApp::getInstance();

    //testAPI
    NumberLogic* numberLogic = new NumberLogic();
    SubNumberLogic* subNumberLogic = new SubNumberLogic(numberLogic);
    subNumberLogic;
    QCefV8BindApp::getInstance()->setV8RootObject(numberLogic);
    QCefCoreApp::getInstance()->createBrowser("https://www.baidu.com");
    QObject::connect(QCefCoreApp::getInstance(), SIGNAL(allClosed()), &cefClient, SLOT(shutDownCef()));
    QObject::connect(&cefClient, SIGNAL(shutdown()), &app, SLOT(quit()));
	
    app.exec();
    return 0;
}