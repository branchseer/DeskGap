#include "../menu/menu.h"
#include "menu_impl.h"

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
            }
        }
        gtk_widget_show(GTK_WIDGET(impl_->gtkMenuItem));
    }
    void MenuItem::Impl::HandleActivate(GtkMenuItem*, MenuItem* menuItem) {
        menuItem->impl_->callbacks.onClick();
    }
    MenuItem::~MenuItem() {
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

    }

    Menu::Menu(const Type& type): impl_(std::make_unique<Impl>()) {
        if (type == Type::MAIN) {
            impl_->gtkMenuShell =GTK_MENU_SHELL(g_object_ref_sink(gtk_menu_bar_new()));
        }
        else {
            impl_->gtkMenuShell =GTK_MENU_SHELL(g_object_ref_sink(gtk_menu_new()));
        }
        gtk_widget_show(GTK_WIDGET(impl_->gtkMenuShell));
    }

    void Menu::AppendItem(const MenuItem& menuItem) {
        gtk_menu_shell_append(impl_->gtkMenuShell, GTK_WIDGET(menuItem.impl_->gtkMenuItem));
    }

    Menu::~Menu() {
        g_object_unref(impl_->gtkMenuShell);
    };
}
