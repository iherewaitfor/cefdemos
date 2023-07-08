#pragma once
#include "window.h"
#include "include/internal/cef_types.h"

class QCefBrowserPrivate;
class QCefBrowserWindow : public windowlib::Window
{
protected:
    QCefBrowserPrivate* m_browser;
public:
    explicit QCefBrowserWindow(QCefBrowserPrivate* browser);
    ~QCefBrowserWindow();

    void SetTitle(const std::wstring& text);

    void SetAppLifeGuard();

    void UpdateWindow(const RECT* rc = NULL);

    bool onChildHitTest(WPARAM wParam, LPARAM lParam, LRESULT& result) ;

protected:
    void onWindowVisbleChanged(bool visible) override;
    void onWindowStyleChanged(windowlib::Window::WindowStyleHint) override;

    bool onHitTest(WPARAM wParam, LPARAM lParam, LRESULT& result) override;
    bool onSysCommand(WPARAM wParam, LPARAM lParam, LRESULT& result) override;
    bool onFocus(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result) override;
    bool onClose(WPARAM wParam, LPARAM lParam, LRESULT& result) override;
    bool onDestory(WPARAM wParam, LPARAM lParam, LRESULT& result) override;
    bool onGeomertry(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result) override;
    bool onActivate(WPARAM wParam, LPARAM lParam, LRESULT& result) override;
    bool onDpiChange(WPARAM wParam, LPARAM lParam, LRESULT& result);
};

class QCefBrowserChildWindow : public windowlib::Window
{
    QCefBrowserPrivate* m_browser;
public:
    QCefBrowserChildWindow(HWND window, QCefBrowserPrivate* parent);
    ~QCefBrowserChildWindow();

protected:
    bool processMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result) override;
    bool onHitTest(WPARAM wParam, LPARAM lParam, LRESULT& result) override;
};
