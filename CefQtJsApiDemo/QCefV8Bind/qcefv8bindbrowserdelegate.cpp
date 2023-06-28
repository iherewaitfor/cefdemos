
#include <QVariant>
#include "qcefipcprotocol.h"
#include "qcefv8bindapp.h"
#include "qcefv8bindapp_p.h"
#include "qcefv8objecthelper.h"
#include "qcefvalue_qvar_convert.h"
#include "qcefv8bindbrowserdelegate.h"


QCefV8BindBrowserDelegate::QCefV8BindBrowserDelegate()
{
	connect(this, SIGNAL(processMessageReceived(
		CefRefPtr<CefBrowser>,
		CefRefPtr<CefFrame>,
		CefProcessId,
		CefRefPtr<CefProcessMessage>)),
		SLOT(OnProcessMessageReceivedSlot(
			CefRefPtr<CefBrowser>,
			CefRefPtr<CefFrame>,
			CefProcessId,
			CefRefPtr<CefProcessMessage>)));
}

QCefV8BindBrowserDelegate::~QCefV8BindBrowserDelegate()
{

}

void QCefV8BindBrowserDelegate::OnBrowserCreated(CefRefPtr<CefBrowser> browser, int)
{

}

void QCefV8BindBrowserDelegate::OnBrowserClosed(CefRefPtr<CefBrowser> browser, int)
{
}

bool QCefV8BindBrowserDelegate::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
    //本回调运行在cef消息循环线程
    CefString message_name = "123";
    message;
    //message->GetName();
//    emit processMessageReceived(browser, frame, source_process, message);
    //CefString message_name = message->GetName();
//    if (message_name == cefv8bind_protcool::CefApiMetaDatasReq::message_name())
//    {
//        //通过Qt信号把数据切到主线程进行处理。也可以可能CefPostTask切换。
////        emit processMessageReceived(browser, frame, source_process, message);
//        return true;
//    }
	return false;
}

void QCefV8BindBrowserDelegate::OnProcessMessageReceivedSlot(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
    //在主线程处理。
    std::string message_name = message->GetName();
    if (message_name == cefv8bind_protcool::CefApiMetaDatasReq::message_name())
    {
        cefv8bind_protcool::CefApiMetaDatasReq req;
        if (req.unPack(message->GetArgumentList()))
        {
            int bId = browser->GetIdentifier();
            int fFrameId = frame->GetIdentifier();
            QString bfid = QString("%1_%2").arg(bId).arg(fFrameId);
            int renderPid = req.render_pid;
            bfid;
            renderPid;
            bId;
            fFrameId;
        }
        QList<cefv8bind_protcool::CefMetaObject>  cef_metaObjects;
        QCefV8ObjectHelper objectHelper;
        objectHelper.convertQObjectToCefObjects(QCefV8BindApp::getInstance()->d_func()->getRootObject(), nullptr, cef_metaObjects);
        cefv8bind_protcool::CefApiMetaDatasResponse response;
        response.cef_metaObjects = cef_metaObjects;
        frame->SendProcessMessage(PID_RENDERER, response.makeIPCMessage());
    }
}
