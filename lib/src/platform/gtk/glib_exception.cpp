#include "glib_exception.h"

namespace DeskGap {
	GlibException::GlibException(GError* gerror):
		message_(gerror->message),
		domain_(gerror->domain),
		code_(gerror->code)
		{ }
	std::string GlibException::domain() const {
		return g_quark_to_string(domain_);
	}
	std::string GlibException::message() const {
		return message_;
	}
	int GlibException::code() const {
		return code_;
	}
	const char * GlibException::what() const noexcept {
		return message_.c_str();
	}
}
