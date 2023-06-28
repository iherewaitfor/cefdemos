#include "qcefv8handler.h"
#include "include/wrapper/cef_helpers.h"
#include "../QtCefBrowser/qcefipcprotocol.h"
#include "../QtCefBrowser/qcefipcvalue.h"
#include "../QtCefBrowser/qcefv8objecthelper.h"

using namespace cefv8bind_protcool;

QCefV8Handler::QCefV8Handler(CefRefPtr<CefFrame> frame)
: m_frame(frame)
{
	
}

QCefV8Handler::~QCefV8Handler()
{

}
void QCefV8Handler::clear()
{
    CEF_REQUIRE_RENDERER_THREAD();
    m_frame = nullptr;
}

CefRefPtr<CefFrame> QCefV8Handler::getFrame()
{
	return m_frame;
}

bool QCefV8Handler::Execute(const CefString& name, CefRefPtr<CefV8Value> v8Object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception)
{
	return false;
}
void QCefV8Handler::on_cefMetaDatasResponse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefProcessMessage> message)
{
    if (m_frame == NULL)
    {
        return;
    }
    CefRefPtr<CefV8Context> context = m_frame->GetV8Context();

	//CefApiMetaDatasResponse ipc_protocol;
	//if (!ipc_protocol.unPack(message->GetArgumentList()))
	//{
	//	return;
	//}
}