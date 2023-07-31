#include "qcefv8bindutility.h"
#include "include/cef_base.h"
#include "include/cef_v8.h"




V8ContextCaller::V8ContextCaller(CefRefPtr<CefV8Context> context)
: m_context(context)
{
	m_context->Enter();
}

V8ContextCaller::~V8ContextCaller()
{
	m_context->Exit();
}