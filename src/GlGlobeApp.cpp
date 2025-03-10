/* -*- Mode: c++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2018 rpf
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <psc_i18n.hpp>
#include <psc_format.hpp>
#include <gtkmm.h>
#include <exception>
#include <thread>
#include <future>
#include <clocale>

#include "GlGlobeWindow.hpp"
#include "GlGlobeApp.hpp"
#include "GlSphereView.hpp"


GlGlobeApp::GlGlobeApp(int argc, char **argv)
: Gtk::Application{argc, argv, "de.pfeifer_syscon.glglobe"}
, m_glglobeAppWindow{nullptr}
{
}


void
GlGlobeApp::on_activate()
{
    add_window(*m_glglobeAppWindow);
    m_glglobeAppWindow->show();
}


void
GlGlobeApp::on_action_quit()
{
    m_glglobeAppWindow->hide();
    // Not really necessary, when Gtk::Widget::hide() is called, unless
    // Gio::Application::hold() has been called without a corresponding call
    // to Gio::Application::release().
    quit();
}

void
GlGlobeApp::on_shutdown()
{
    if (m_glglobeAppWindow) {
        delete m_glglobeAppWindow;
        m_glglobeAppWindow = nullptr;
    }
}

void
GlGlobeApp::showMessage(const Glib::ustring& msg, Gtk::MessageType msgType)
{
    Gtk::MessageDialog messagedialog(msg, false, msgType);
    messagedialog.run();
    messagedialog.hide();
}

void
GlGlobeApp::on_startup() {
    // Call the base class's implementation.
    Gtk::Application::on_startup();
    m_glglobeAppWindow = new GlGlobeWindow();
    signal_shutdown().connect(sigc::mem_fun(*this, &GlGlobeApp::on_shutdown));

    // Add actions and keyboard accelerators for the application menu.
    add_action("quit", sigc::mem_fun(*this, &GlGlobeApp::on_action_quit));
    set_accel_for_action("app.quit", "<Ctrl>Q");

    auto refBuilder = Gtk::Builder::create();
    try {
        std::string res = RESOURCE::resource("app-menu.ui");
        refBuilder->add_from_resource(res);
        auto object = refBuilder->get_object("appmenu");
        auto app_menu = Glib::RefPtr<Gio::MenuModel>::cast_dynamic(object);
        if (app_menu)
            set_app_menu(app_menu);
        else
            showMessage(
                psc::fmt::vformat(
                      _("No \"{}\" object in {}")
                    , psc::fmt::make_format_args("appmenu", "app_menu.ui"))
                , Gtk::MessageType::MESSAGE_ERROR);
    }
    catch (const Glib::Error& ex) {
        showMessage(
                psc::fmt::vformat(
                      _("Error {} while loading {}")
                    , psc::fmt::make_format_args(ex, "app_menu.ui"))
                , Gtk::MessageType::MESSAGE_ERROR);
    }
}

int
main(int argc, char** argv) {

    //std::locale::global(std::locale("")); // Use the current user/system locale
    char* loc = std::setlocale(LC_ALL, "");  // this seems not to work with individual categories
    if (loc != nullptr) {
        //std::cout << "setlocale " << loc << std::endl;
        // sync c++
        std::locale::global(std::locale(loc));
    }
    else {
        std::cout << "error setlocale " << std::endl;
    }
    bindtextdomain(PACKAGE, PACKAGE_LOCALE_DIR);
    textdomain(PACKAGE);
    Glib::init();

    g_log_writer_default_set_use_stderr(true);
    auto app = GlGlobeApp(argc, argv);

    return app.run();
}
