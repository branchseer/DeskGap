#include "../dispatch/ui_dispatch_platform.h"

namespace {
   std::optional<DeskGap::PlatformException> ExecuteAction(const std::function<void()>& action) {
      action();
      return std::nullopt;
   }
}

std::optional<DeskGap::PlatformException> DeskGap::UISyncPlatform(std::function<void()>&& action) {
   return std::nullopt;
}

void DeskGap::UIASyncPlatform(std::function<void()>&& action, std::function<void(std::optional<PlatformException>&&)> callback) {

}
