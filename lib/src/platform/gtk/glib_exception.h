#ifndef gtk_glib_exception_h
#define gtk_glib_exception_h

#include <exception>
#include <gtk/gtk.h>
#include <string>

namespace DeskGap {
	class GlibException: public std::exception {
	private:
		std::string message_;
		GQuark domain_;
		gint code_;
		GlibException(GError*);
	public:
		int code() const;
		std::string domain() const;
		std::string message() const;
		const char * what () const noexcept override;

		inline static void ThrowAndFree(GError* gerror) {
			if (gerror == nullptr) return;
			GlibException glibException(gerror);
			g_error_free(gerror);
			throw std::move(glibException);
		}
	};
}

#endif
