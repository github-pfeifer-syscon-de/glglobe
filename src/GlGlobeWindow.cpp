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

#include "GlGlobeWindow.hpp"
#include "GlGlobeApp.hpp"
#include "ConfigDialog.hpp"
#include "SphereGlArea.hpp"
#include "Timer.hpp"

GlGlobeWindow::GlGlobeWindow()
: Gtk::ApplicationWindow()
, m_config{new Config()}
{
    m_config->read();
    m_sphereView = new GlSphereView(m_config);
    auto naviGlArea = Gtk::manage(new SphereGlArea(m_sphereView));
	#ifdef USE_GLES
    //naviGlArea->set_required_version (3, 0);
	naviGlArea->set_use_es(true);
	#endif
    add(*naviGlArea);

    add_action("timer", sigc::mem_fun(*this, &GlGlobeWindow::on_action_Timer));
    add_action("preferences", sigc::mem_fun(*this, &GlGlobeWindow::on_action_preferences));
    add_action("about", sigc::mem_fun(*this, &GlGlobeWindow::on_action_about));

    Glib::RefPtr<Gdk::Pixbuf> icon = Gdk::Pixbuf::create_from_resource(RESOURCE::resource("glglobe.png"));
    set_icon(icon);

    set_default_size(500, 320);
    show_all_children();
}


GlGlobeWindow::~GlGlobeWindow()
{
}

void
GlGlobeWindow::save_config()
{
    m_config->save();
}

void
GlGlobeWindow::import_dialog_setup(Glib::RefPtr<Gtk::Builder> refBuilder, Glib::RefPtr<Gtk::Dialog> importdlg)
{
    Gtk::FileChooserButton* fileFcBtn = nullptr;
    refBuilder->get_widget("file", fileFcBtn);
    if (fileFcBtn) {
    }

    Gtk::ProgressBar *progress = nullptr;
    refBuilder->get_widget("progress", progress);

    Gtk::Button* import = nullptr;
    refBuilder->get_widget("import", import);
    if (import) {
        //GeoDb *geoDb = m_sphereView.getGeoDb();
        //import->signal_clicked().connect(sigc::bind<Gtk::FileChooserButton *, Gtk::ProgressBar *, Glib::RefPtr<Gtk::Dialog>, Gtk::Button*>(
        //                           sigc::mem_fun(geoDb, &GeoDb::import),
        //                           fileFcBtn, progress, importdlg, import));
    }
}


void
GlGlobeWindow::on_action_preferences()
{
    ConfigDialog *cfgdlg = ConfigDialog::create(m_sphereView);
    if (cfgdlg) {
        m_sphereView->set_config_dialog(cfgdlg);
        cfgdlg->set_transient_for(*this);
        cfgdlg->run();
        cfgdlg->hide();
        m_sphereView->set_config_dialog(nullptr);
        save_config();
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
        } else
            std::cerr << "GlGlobe::on_action_about(): No \"abt-dlg\" object in abt-dlg.ui"
                << std::endl;
    } catch (const Glib::Error& ex) {
        std::cerr << "GlGlobe::on_action_about(): " << ex.what() << std::endl;
    }
}

void
GlGlobeWindow::on_action_Timer()
{
    auto refBuilder = Gtk::Builder::create();
    try {
        refBuilder->add_from_resource(RESOURCE::resource("timer-dlg.ui"));
        Timer* timer;
        refBuilder->get_widget_derived("timer-dlg", timer, m_config);
        if (timer) {
            timer->set_transient_for(*this);
            timer->run();
            timer->hide();
            //timer->unreference();     this calls destructor and crashes afterwards...
        }
        else {
            std::cerr << "GlGlobe::on_action_Timer(): No \"timer-dlg\" object in timer-dlg.ui" << std::endl;
        }
    }
    catch (const Glib::Error& ex) {
        std::cerr << "GlGlobe::on_action_Timer(): " << ex.what() << std::endl;
    }
}
