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
	
    app.exec();
    return 0;
}