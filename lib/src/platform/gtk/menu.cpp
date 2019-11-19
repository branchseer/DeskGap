#include "menu.hpp"
#include "menu_impl.h"

#include <unordered_map>

namespace DeskGap {

    MenuItem::MenuItem(const std::string& role, const Type& type, const Menu* submenu, EventCallbacks&& eventCallbacks): impl_(std::make_unique<Impl>()) {
        if (type == Type::SEPARATOR) {
            impl_->gtkMenuItem = GTK_MENU_ITEM(g_object_ref_sink(gtk_separator_menu_item_new()));
            impl_->activateConnection = 0;
        }
        else {
            if (type == Type::CHECKBOX) {
                impl_->gtkMenuItem = GTK_MENU_ITEM(g_object_ref_sink(gtk_check_menu_item_new()));
            }
            else if (type == Type::RADIO) {
                impl_->gtkMenuItem = GTK_MENU_ITEM(g_object_ref_sink(gtk_radio_menu_item_new(nullptr)));
            }
            else {
                impl_->gtkMenuItem = GTK_MENU_ITEM(g_object_ref_sink(gtk_menu_item_new()));
            }
            impl_->callbacks = std::move(eventCallbacks);
            impl_->activateConnection = g_signal_connect(impl_->gtkMenuItem, "activate", G_CALLBACK(Impl::HandleActivate), this);

            if (submenu != nullptr) {
                gtk_menu_item_set_submenu(impl_->gtkMenuItem, GTK_WIDGET(submenu->impl_->gtkMenuShell));
                impl_->submenu = std::make_optional<std::reference_wrapper<const Menu>>(*submenu);
            }
            gtk_widget_show(GTK_WIDGET(impl_->gtkMenuItem));
        }
    }
    void MenuItem::Impl::HandleActivate(GtkMenuItem*, MenuItem* menuItem) {
        menuItem->impl_->callbacks.onClick();
    }
    MenuItem::~MenuItem() {
        if (impl_->activateConnection > 0) {
            g_signal_handler_disconnect(impl_->gtkMenuItem, impl_->activateConnection);
        }
        g_object_unref(impl_->gtkMenuItem);
    }

    void MenuItem::SetLabel(const std::string& utf8label) {
        gtk_menu_item_set_label(impl_->gtkMenuItem, utf8label.c_str());
    }
    
    void MenuItem::SetEnabled(bool enabled) {
        gtk_widget_set_sensitive(GTK_WIDGET(impl_->gtkMenuItem), enabled);
    }


    void MenuItem::SetChecked(bool checked) {
        if (GTK_IS_CHECK_MENU_ITEM(impl_->gtkMenuItem)) {
            gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(impl_->gtkMenuItem), checked);
        }
    }
    
    std::string MenuItem::GetLabel() {
        return std::string(gtk_menu_item_get_label(impl_->gtkMenuItem));
    }

    void MenuItem::SetAccelerator(const std::vector<std::string>& tokens) {
        static const std::unordered_map<std::string, GdkModifierType> kModifiersByToken {
            { "ctrl", GDK_CONTROL_MASK },
            { "shift", GDK_SHIFT_MASK },
            { "alt", GDK_MOD1_MASK }
        };
        static const std::unordered_map<std::string, guint> kKeysByToken {
            { "up", GDK_KEY_Up },
            { "down", GDK_KEY_Down },
            { "left", GDK_KEY_Left },
            { "right", GDK_KEY_Right },
            { "plus", GDK_KEY_plus },
            { "space", GDK_KEY_space },
            { "enter", GDK_KEY_Return },
            { "tab", GDK_KEY_Tab },
            { "backspace", GDK_KEY_BackSpace },
            { "delete",  GDK_KEY_Delete },
            { "insert", GDK_KEY_Insert },
            { "home", GDK_KEY_Home },
            { "end", GDK_KEY_End },
            { "pageup", GDK_KEY_Page_Up },
            { "pagedown", GDK_KEY_Page_Down },
            { "escape", GDK_KEY_Escape },
            { "volumedown", GDK_KEY_AudioLowerVolume },
            { "Volumeup", GDK_KEY_AudioRaiseVolume },
            { "volumemute", GDK_KEY_AudioMute },
            { "medianexttrack", GDK_KEY_AudioNext },
            { "mediaprevioustrack", GDK_KEY_AudioPrev },
            { "mediastop", GDK_KEY_AudioStop },
            { "mediaplaypause", GDK_KEY_AudioPause },
            { "printscreen", GDK_KEY_Print }
        };

        if (tokens.empty()) {
            impl_->accelInfo.reset();
            return;
        }

        guint key = GDK_KEY_VoidSymbol;
        GdkModifierType mods = GdkModifierType(0);
        for (const std::string& token: tokens)
        {
            if (auto it = kModifiersByToken.find(token); it != kModifiersByToken.end()) {
                mods = GdkModifierType(mods | it->second);
            }
            else if (auto it = kKeysByToken.find(token); it != kKeysByToken.end()) {
                key = it->second;
            }
            else if (!token.empty()) {
                key = gdk_unicode_to_keyval(token.front());
            }
        }
        impl_->accelInfo.emplace(Impl::AccelInfo {
            key, mods
        });
    }

    Menu::Menu(const Type& type): impl_(std::make_unique<Impl>()) {
        if (type == Type::MAIN) {
            impl_->gtkMenuShell = GTK_MENU_SHELL(g_object_ref_sink(gtk_menu_bar_new()));
        }
        else {
            impl_->gtkMenuShell = GTK_MENU_SHELL(g_object_ref_sink(gtk_menu_new()));
        }
        gtk_widget_show(GTK_WIDGET(impl_->gtkMenuShell));
    }

    void Menu::AppendItem(const MenuItem& menuItem) {
        impl_->items.emplace_back(menuItem);
        gtk_menu_shell_append(impl_->gtkMenuShell, GTK_WIDGET(menuItem.impl_->gtkMenuItem));
    }

    void Menu::Impl::SetAccelGroup(GtkAccelGroup* accelGroup) const {
        for (const MenuItem& menuItem: items) {
            if (const auto& accelInfo = menuItem.impl_->accelInfo; accelInfo.has_value()) {
                gtk_widget_add_accelerator(
                    GTK_WIDGET(menuItem.impl_->gtkMenuItem),
                    "activate",
                    accelGroup,
                    accelInfo->key, accelInfo->mods,
                    GTK_ACCEL_VISIBLE
                );
            }
            if (menuItem.impl_->submenu.has_value()) {
                menuItem.impl_->submenu->get().impl_->SetAccelGroup(accelGroup);
            }
        }
    }


    Menu::~Menu() {
        g_object_unref(impl_->gtkMenuShell);
    };
}
