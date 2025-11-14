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
#include <thread>
#include <future>
#include <GenericGlmCompat.hpp>
#include <StringUtils.hpp>
#include <filesystem>
#include <psc_i18n.hpp>
#include <psc_format.hpp>

#include "GlGlobeWindow.hpp"
#include "GlGlobeApp.hpp"
#include "ConfigDialog.hpp"
#include "SphereGlArea.hpp"
#include "WeatherDialog.hpp"
#include "PlotDialog.hpp"


GlGlobeWindow::GlGlobeWindow()
: Gtk::ApplicationWindow()
, m_config{std::make_shared<Config>()}
{
    m_config->read();
    m_sphereView = new GlSphereView(m_config);
    auto naviGlArea = Gtk::manage(new SphereGlArea(m_sphereView));
	#ifdef USE_GLES
    //naviGlArea->set_required_version (3, 0);
	naviGlArea->set_use_es(true);
	#endif
    add(*naviGlArea);

    add_action("timer", sigc::mem_fun(*this, &GlGlobeWindow::on_action_timer));
    add_action("plot", sigc::mem_fun(*this, &GlGlobeWindow::on_action_plot));
    add_action("preferences", sigc::mem_fun(*this, &GlGlobeWindow::on_action_preferences));
    add_action("about", sigc::mem_fun(*this, &GlGlobeWindow::on_action_about));
    // radio actions are the best way to get a string parameter action, no radio involved
    add_action_radio_string("weatherEdit",
                    sigc::bind(
                        sigc::mem_fun(*this, &GlGlobeWindow::on_action_weather), false), "");
    add_action_radio_string("weatherAdd",
                    sigc::bind(
                        sigc::mem_fun(*this, &GlGlobeWindow::on_action_weather), true), "");

    Glib::RefPtr<Gdk::Pixbuf> icon = Gdk::Pixbuf::create_from_resource(RESOURCE::resource("glglobe.png"));
    set_icon(icon);

    set_default_size(500, 320);
    show_all_children();
}


void
GlGlobeWindow::save_config()
{
    if (!m_config->save()) {
        showMessage(_("Error saving config"), Gtk::MessageType::MESSAGE_ERROR);
    }
}

void
GlGlobeWindow::showMessage(const Glib::ustring& msg, Gtk::MessageType msgType)
{
    Gtk::MessageDialog messagedialog(*this, msg, false, msgType);
    messagedialog.run();
    messagedialog.hide();
}

void
GlGlobeWindow::on_action_preferences()
{
    m_cfgdlg = ConfigDialog::create(m_sphereView);
    if (m_cfgdlg) {
        m_cfgdlg->set_transient_for(*this);
        m_cfgdlg->run();
        closeConfigDlg();
    }
}

void
GlGlobeWindow::closeConfigDlg()
{
    if (m_cfgdlg) {
        m_cfgdlg->hide();
        save_config();
        delete m_cfgdlg;      // cleanup
        m_cfgdlg = nullptr;
    }
}

void
GlGlobeWindow::on_action_about()
{
    auto refBuilder = Gtk::Builder::create();
    try {
        refBuilder->add_from_resource(RESOURCE::resource("abt-dlg.ui"));
        auto object = refBuilder->get_object("abt-dlg");
        auto abtdlg = Glib::RefPtr<Gtk::AboutDialog>::cast_dynamic(object);
        if (abtdlg) {
            Glib::RefPtr<Gdk::Pixbuf> pix = Gdk::Pixbuf::create_from_resource(RESOURCE::resource("glglobe.png"));
            abtdlg->set_logo(pix);
            abtdlg->set_transient_for(*this);
            abtdlg->run();
            abtdlg->hide();
        }
        else
            showMessage(
                psc::fmt::vformat(
                      _("No \"{}\" object in {}")
                    , psc::fmt::make_format_args("abt-dlg", "abt-dlg.ui"))
                , Gtk::MessageType::MESSAGE_ERROR);
    }
    catch (const Glib::Error& ex) {
        showMessage(
                psc::fmt::vformat(
                      _("Error {} while loading {}")
                    , psc::fmt::make_format_args(ex, "abt-dlg.ui"))
                , Gtk::MessageType::MESSAGE_ERROR);
    }
}

void
GlGlobeWindow::on_action_weather(const Glib::ustring& idStr, bool add)
{
    //std::cout << "GlGlobeWindow::on_action_weather id" << idStr << std::endl;
    // to simplify the overall handling
    //   close weather dialog here and reopen when weather setup is done
    closeConfigDlg();
    auto weatherDlg = WeatherDialog::create(m_config, idStr, add);
    if (weatherDlg) {
        weatherDlg->set_transient_for(*this);
        int ret = weatherDlg->run();
        weatherDlg->hide();
        if (ret == Gtk::RESPONSE_OK) {
            save_config();
            on_action_preferences();    // reopen config
        }
        delete weatherDlg;      // cleanup
    }
}

void
GlGlobeWindow::on_action_timer()
{
    auto refBuilder = Gtk::Builder::create();
    try {
        refBuilder->add_from_resource(RESOURCE::resource("timer-dlg.ui"));
        TimerChrono* timer{nullptr};     // may choose other implementaion ... TimerGlib depending of what smell seems preferable for you
        refBuilder->get_widget_derived("timer-dlg", timer, m_config);
        if (timer) {
            timer->set_transient_for(*this);
            timer->run();
            timer->hide();
            delete timer; // keep running in background will not allow to hide after second show/run
        }
        else {
            showMessage(
                    psc::fmt::vformat(
                          _("No \"{}\" object in {}")
                        , psc::fmt::make_format_args("timer-dlg", "timer-dlg.ui"))
                    , Gtk::MessageType::MESSAGE_ERROR);
        }
    }
    catch (const Glib::Error& ex) {
        showMessage(
                psc::fmt::vformat(
                      _("Error {} while loading {}")
                    , psc::fmt::make_format_args(ex, "timer-dlg.ui"))
                , Gtk::MessageType::MESSAGE_ERROR);
    }
}

void
GlGlobeWindow::on_action_plot()
{
    auto refBuilder = Gtk::Builder::create();
    try {
        refBuilder->add_from_resource(RESOURCE::resource("plot-dlg.ui"));
        PlotDialog* plotDialog;
        refBuilder->get_widget_derived("PlotDialog", plotDialog);
        if (plotDialog) {
            plotDialog->set_transient_for(*this);
            plotDialog->run();
            plotDialog->hide();
            delete plotDialog;
        }
        else {
            showMessage(
                    psc::fmt::vformat(
                          _("No \"{}\" object in {}")
                        , psc::fmt::make_format_args("plot-dlg", "plot-dlg.ui"))
                    , Gtk::MessageType::MESSAGE_ERROR);
        }
    }
    catch (const Glib::Error& ex) {
        showMessage(
                psc::fmt::vformat(
                      _("Error {} while loading {}")
                    , psc::fmt::make_format_args(ex, "plot-dlg.ui"))
                , Gtk::MessageType::MESSAGE_ERROR);
    }

}
