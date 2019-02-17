#ifndef win_browserwindow_impl_h
#define win_browserwindow_impl_h

#include <vcclr.h>
#include "../window/browser_window.h"

using namespace System;
using namespace System::Windows::Forms;

namespace DeskGap {
    struct BrowserWindow::Impl {
        gcroot<Form^> form;
        gcroot<Object^> callbackHandler;
        gcroot<String^> formTitle = String::Empty;
        bool resizable = true;
        bool hasFrame = true;
        void updateFrame();
    };
}

#endif
