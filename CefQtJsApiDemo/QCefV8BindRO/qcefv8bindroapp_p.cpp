#include "qcefv8bindroapp_p.h"
#include "../QtCefBrowser/qcefcoreapp.h"
//void QCefV8BindAppROPrivate::callReplicaMethod_slot() {
//	cefv8bind_protcool::PendingcallReq req;
void QCefV8BindAppROPrivate::callReplicaMethod_slot(cefv8bind_protcool::PendingcallReq req) {

	if (!m_pDynamicClientTreeHelper->getObjectsIdMap().contains(req.objctId)) {
		return;
	}
	QSharedPointer<DynamicClient> pDynamicClient = m_pDynamicClientTreeHelper->getObjectsIdMap().value(req.objctId);
	if (pDynamicClient.isNull()) {
		return;;
	}
	QObject* apiObject = pDynamicClient->getReplica().data();
	if (!apiObject) {
		return;
	}
	QMetaMethod metaMethod = apiObject->metaObject()->method(req.methodIndex);
	MetaInvoker metaInvoker;
	metaInvoker.object = apiObject;
	metaInvoker.metaMethod = metaMethod;
	metaInvoker.callbackId = req.callBackId;
	metaInvoker.frameId = req.frameId;

	CefRefPtr<CefValue> args = CefValue::Create();
	args->SetList(req.methodArgs);
	metaInvoker.args = QCefValueConverter::convertFromCefListToVariantList(args);

	metaInvoker.run();
	//if (metaMethod.returnType() != QMetaType::Void) {
	//	//需要返回值
	//}
}

void QCefV8BindAppROPrivate::pendingCallResult(QRemoteObjectPendingCallWatcher* call) {
	qint64 callbackId = call->property("callbackId").toLongLong();
	qint64 frameId = call->property("frameId").toLongLong();
	cefv8bind_protcool::PendingcallResp resp;
	resp.callBackId = callbackId;
	resp.frameId = frameId;
	resp.invokeResult = (call->error() == QRemoteObjectPendingCall::NoError);
	resp.returnValue = QCefValueConverter::to(call->returnValue());
	m_renderDelegate->onPendingcallResp(resp, frameId);
	call->deleteLater(); //释放对象。
}

void QCefV8BindAppROPrivate::connectReplicaSignal_slot(cefv8bind_protcool::ConnectReplicaSignal req) {

	if (!m_pDynamicClientTreeHelper->getObjectsIdMap().contains(req.objctId)) {
		return;
	}
	QSharedPointer<DynamicClient> pDynamicClient = m_pDynamicClientTreeHelper->getObjectsIdMap().value(req.objctId);
	if (pDynamicClient.isNull()) {
		return;;
	}
	QObject* apiObject = pDynamicClient->getReplica().data();
	if (!apiObject) {
		return;
	}
	QMetaMethod metaMethod = apiObject->metaObject()->method(req.methodIndex);
	if (metaMethod.methodType() != QMetaMethod::Signal) {
		return;
	}
	//to do: same the autoEmitter, and check if the autoEmitter exist. if exist do not new.
	//to do: connect the signal of the object
	QString key = QString("%1_%2").arg(req.objctId).arg(req.methodIndex);
	if (m_signalMap.contains(key)) {
		return;
	}
	AutoSignalsEmitter* autoEmitter = new AutoSignalsEmitter(metaMethod,req.objctId, apiObject);
	QObject::connect(apiObject, QString("2").append(metaMethod.methodSignature()).toStdString().c_str(),
		autoEmitter, SLOT(proxySlot()), Qt::DirectConnection);
	m_signalMap.insert(key, autoEmitter);

}

void QCefV8BindAppROPrivate::cefApiWindowCreated_slot(int browserId) {
	//browser进程
	//为id为broserId的browser启动CefAPIWindow  source objcet
	CefApi::Window* apiWindow = QCefCoreApp::getInstance()->getApiWindow(browserId);
	if (!apiWindow) {
		return;
	}
	//to do 
}