#include <QtCore>
#include <QApplication>
#include "QtCefBrowser/qcefclient.h"
#include "QtCefBrowser/qcefcoreapp.h"
#include <QtWidgets>

#include "numberlogic.h"
#include "subnumberlogic.h"
//#include "QCefV8Bind/qcefv8bindapp.h"
#include "../QCefV8BindRO/qcefv8bindroapp.h"

int main(int argc, char** argv )
{
    QApplication app(argc, argv);

    QCefClient cefClient;
    cefClient.initCef();
    //QCefV8BindApp::getInstance();
    ////testAPI
    //NumberLogic* numberLogic = new NumberLogic();
    //SubNumberLogic* subNumberLogic = new SubNumberLogic(numberLogic);
    //subNumberLogic;
    //QCefV8BindApp::getInstance()->setV8RootObject(numberLogic);

    QCefV8BindAppRO::getInstance();
    //testAPI
    NumberLogic* numberLogic = new NumberLogic();
    SubNumberLogic* subNumberLogic = new SubNumberLogic(numberLogic);
    subNumberLogic;
    QCefV8BindAppRO::getInstance()->addV8RootObject(numberLogic);

    BrowserWindowOptions options;
    options.url = "https://www.baidu.com";
    options.width = 1200;
    options.height = 900;
    options.minWidth = 300;
    options.minHeight = 300;
    options.showInTaskbar = true;
    options.visible = true;
    options.frameless = false;
    std::vector<RECT> capVector;
    RECT rect = { 0, 0, 1200, 100 };
    capVector.push_back(rect);
    options.captionAreas = capVector;
    QCefCoreApp::getInstance()->createBrowser(options);
    QObject::connect(QCefCoreApp::getInstance(), SIGNAL(allClosed()), &cefClient, SLOT(shutDownCef()));
    QObject::connect(&cefClient, SIGNAL(shutdown()), &app, SLOT(quit()));
	
    app.exec();
    return 0;
}