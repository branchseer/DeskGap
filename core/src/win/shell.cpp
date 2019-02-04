#include "../shell/shell.h"
#include "./util/string_convert.h"

bool DeskGap::Shell::OpenExternal(const std::string& urlString) {
    try {
        return System::Diagnostics::Process::Start(ClrStr(urlString)) != nullptr;
    }
    catch (System::Exception^) {
        return false;
    }
}
