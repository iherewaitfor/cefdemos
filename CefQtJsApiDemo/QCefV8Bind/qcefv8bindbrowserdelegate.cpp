
#include <QVariant>
#include <QtCore>
#include "qcefipcprotocol.h"
#include "qcefv8bindapp.h"
#include "qcefv8bindapp_p.h"
#include "qcefv8objecthelper.h"
#include "qcefvalue_qvar_convert.h"
#include "qcefv8bindbrowserdelegate.h"

#include "metainvoker.h"


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
void QCefV8BindBrowserDelegate::sendProcessMessage(CefProcessId cefProcEd, CefRefPtr<CefProcessMessage> message)
{
    base::AutoLock scopeLock(lock);
    foreach(int id, m_subscribeBrowsers)
    {
        if (m_browsers.contains(id) && m_browsers[id])
        {
            m_browsers[id]->GetMainFrame()->SendProcessMessage(cefProcEd, message);
        }
    }
}

void QCefV8BindBrowserDelegate::OnBrowserCreated(CefRefPtr<CefBrowser> browser)
{
    base::AutoLock scopeLock(lock);
    m_browsers.insert(browser->GetIdentifier(), browser);
}

void QCefV8BindBrowserDelegate::OnBrowserClosing(CefRefPtr<CefBrowser> browser) {

}

void QCefV8BindBrowserDelegate::OnBrowserClosed(CefRefPtr<CefBrowser> browser)
{
    base::AutoLock scopeLock(lock);
    m_browsers.remove(browser->GetIdentifier());
    m_subscribeBrowsers.remove(browser->GetIdentifier());
}

bool QCefV8BindBrowserDelegate::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefProcessId source_process, CefRefPtr<CefProcessMessage> message)
{
    //本回调运行在cef消息循环线程
    CefString message_name = message->GetName();
    if (message_name == cefv8bind_protcool::CefApiMetaDatasReq::message_name()
        || message_name == cefv8bind_protcool::InvokeReq::message_name())
    {
        //通过Qt信号把数据切到主线程进行处理。也可以使用CefPostTask切换。
        emit processMessageReceived(browser, frame, source_process, message);
        return true;
    }
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
            int64 fFrameId = frame->GetIdentifier();
            int renderPid = req.render_pid;
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

        base::AutoLock scopeLock(lock);
        if (!m_subscribeBrowsers.contains(browser->GetIdentifier()))
        {
            m_subscribeBrowsers.insert(browser->GetIdentifier());
        }
    } else if (message_name == cefv8bind_protcool::InvokeReq::message_name()) {
        cefv8bind_protcool::InvokeReq req;
        if (req.unPack(message->GetArgumentList()))
        {
            QVariant retVar;

            QObject* apiObject = QCefV8BindApp::getInstance()->d_func()->getObjectMgr()->findBrowserObject(req.objctId);
            if (apiObject) {
                QMetaMethod metaMethod = apiObject->metaObject()->method(req.methodIndex);
                MetaInvoker metaInvoker;
                metaInvoker.object = apiObject;
                metaInvoker.metaMethod = metaMethod;
                
                CefRefPtr<CefValue> args = CefValue::Create();
                args->SetList(req.methodArgs);
                metaInvoker.args = QCefValueConverter::convertFromCefListToVariantList(args);
                metaInvoker.run();
                if (metaMethod.returnType() != QMetaType::Void) {
                    cefv8bind_protcool::InvokeResp rsp;
                    rsp.callBackId = req.callBackId;
                    rsp.invokeResult = metaInvoker.ok;
                    rsp.objctId = req.objctId;
                    rsp.methodIndex = req.methodIndex;
                    rsp.methodName = req.methodName;
                    rsp.returnValue = QCefValueConverter::to(metaInvoker.result);
                    frame->SendProcessMessage(PID_RENDERER, rsp.makeIPCMessage());
                }
            }
        }

    }
}
