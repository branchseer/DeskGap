#include "../dispatch/ui_dispatch_platform.h"
#include "./util/string_convert.h"

using namespace System;
using namespace System::Threading;

namespace {
   ref class StdFunctionSendOrPostCallback {
   private:
      std::function<void()>* func_;
   public:
      ~StdFunctionSendOrPostCallback() { delete func_; }
      StdFunctionSendOrPostCallback(std::function<void()>* func): func_(new std::function<void()>(std::move(*func))) { }

      void Invoke(Object^) {
         (*func_)();
      }
   };

   std::optional<DeskGap::PlatformException> ExecuteAction(const std::function<void()>& action) {
      try {
         action();
      }
      catch(Exception^ e) {
         return DeskGap::PlatformException {
            StdStr(e->GetType()->FullName),
            StdStr(e->Message)
         };
      }
      return std::nullopt;
   }
}

std::optional<DeskGap::PlatformException> DeskGap::UISyncPlatform(std::function<void()>&& action) {
   std::optional<PlatformException> optionalException;
   std::function<void()> actionCatchingException([
      action = std::move(action),
      &optionalException
   ]() {
      optionalException = ExecuteAction(action);
   });
   SynchronizationContext::Current->Send(
      gcnew SendOrPostCallback(
         gcnew StdFunctionSendOrPostCallback(&actionCatchingException), 
         &StdFunctionSendOrPostCallback::Invoke
      ),
      nullptr
   );

   return optionalException;
}

void DeskGap::UIASyncPlatform(std::function<void()>&& action, std::function<void(std::optional<PlatformException>&&)> callback) {
   std::function<void()> actionCatchingException([
      action = std::move(action),
      callback = std::move(callback)
   ]() {
      std::optional<PlatformException> optionalException = ExecuteAction(action);
      callback(std::move(optionalException));
   });


   SynchronizationContext::Current->Post(
      gcnew SendOrPostCallback(
         gcnew StdFunctionSendOrPostCallback(&actionCatchingException), 
         &StdFunctionSendOrPostCallback::Invoke
      ),
      nullptr
   );
}
