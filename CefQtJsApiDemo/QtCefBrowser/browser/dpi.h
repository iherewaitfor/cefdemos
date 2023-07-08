#pragma once
#include "../qcefbrowserglobal.h"
#include <Windows.h>
namespace windowlib {

    class QCEFBROWSER_DECL_EXPORT DPI
    {
        int dpiX_;
        int dpiY_;
        const int defDpi_;
    public:
        DPI();

        static DPI* instance();

        static void enableHIDPI(bool v);
        static bool isHIDPIEnabled();
        float getDPIScale() const { return (float)dpiX_ / (float)defDpi_; }

        int getDPIX() const { return dpiX_; }
        int getDPIY() const { return dpiY_; }

        int scaleX(int x) const { return MulDiv(x, dpiX_, defDpi_); }
        int scaleY(int y) const { return MulDiv(y, dpiY_, defDpi_); }
        int unscaleX(int x) const { return MulDiv(x, defDpi_, dpiX_); }
        int unscaleY(int y) const { return MulDiv(y, defDpi_, dpiY_); }

        RECT scaleRect(RECT rect) const
        {
            RECT rc = { scaleX(rect.left), scaleX(rect.right), scaleY(rect.top), scaleY(rect.bottom) };
            return rc;
        }

        POINT scalePoint(POINT point) const
        {
            POINT pt = { scaleX(point.x), scaleY(point.y) };
            return pt;
        }

        SIZE scaleSize(SIZE size) const
        {
            SIZE sz = { scaleX(size.cx), scaleY(size.cy) };
            return sz;
        }
    };
}

#define DPI_SCALE_X(x) windowlib::DPI::instance()->scaleX(x)
#define DPI_SCALE_Y(x) windowlib::DPI::instance()->scaleY(x)

#define DPI_SCALE(x) windowlib::DPI::instance()->scaleX(x)
#define DPI_UNSCALE(x) windowlib::DPI::instance()->unscaleX(x)
