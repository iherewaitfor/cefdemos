#pragma once
#include <Windows.h>

#include "optional.hpp"
#include <vector>
#include <string>

struct BrowserWindowOptions
{
    nonstd::optional<std::string> name;
    nonstd::optional<std::string> title;
    nonstd::optional<bool> offscreen;
    nonstd::optional<bool> transparent;
    nonstd::optional<bool> lifeGuard;
    nonstd::optional<bool> visible;
    nonstd::optional<bool> topMost;

    nonstd::optional<int> ncBorderWidth;
    nonstd::optional<int> width;
    nonstd::optional<int> height;
    nonstd::optional<int> x;
    nonstd::optional<int> y;
    nonstd::optional<int> minWidth;
    nonstd::optional<int> minHeight;
    nonstd::optional<int> maxWidth;
    nonstd::optional<int> maxHeight;
    nonstd::optional<bool> resizable;
    nonstd::optional<bool> frameless;

    nonstd::optional<bool> ignoreSysClose;
    nonstd::optional<bool> showInTaskbar;
    nonstd::optional<bool> showTitlebar;

    nonstd::optional<bool> modalMode;
    nonstd::optional<unsigned int> parentHandle;
    nonstd::optional<bool> asChild;

    nonstd::optional<std::vector<RECT>> captionAreas;//
    nonstd::optional<std::string> url;//

    nonstd::optional<bool> isDevTools; //

    nonstd::optional<int> osrMode;

    nonstd::optional<std::string> sharemem;
};
