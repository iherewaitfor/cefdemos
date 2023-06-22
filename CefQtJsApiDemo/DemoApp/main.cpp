#include <QtCore>
#include <QApplication>
#include "..\QtCefBrowser\qcefclient.h"
#include "..\QtCefBrowser\qcefcoreapp.h"
#include <QtWidgets>


int main(int argc, char** argv )
{
    QApplication app(argc, argv);
	
    QCefClient cefClient;
    cefClient.initCef();
    QCefCoreApp::getInstance()->createBrowser("https://www.baidu.com");
    QObject::connect(QCefCoreApp::getInstance(), SIGNAL(allClosed()), &cefClient, SLOT(shutDownCef()));
    QObject::connect(&cefClient, SIGNAL(shutdown()), &app, SLOT(quit()));
	
    app.exec();
    return 0;
}