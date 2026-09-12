/* -*- Mode: c++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2023 RPf 
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
#include <format>

#include "ConfigDialog.hpp"
#include "GlSphereView.hpp"
#include "Config.hpp"
#include "Weather.hpp"

ConfigCoordGrid::ConfigCoordGrid(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, GlSphereView* sphereView)
: BaseConfigGrid(cobject, refBuilder, sphereView)
{
    auto config = std::dynamic_pointer_cast<Config>(m_sphereView->get_config());
    Gtk::SpinButton* pLat{nullptr};
    refBuilder->get_widget("lat", pLat);
    if(pLat) {
        pLat->set_increments(1, 10);
        pLat->set_range(-90, 90);
        pLat->set_value(config->getLatitude());
        pLat->signal_value_changed().connect(sigc::bind<Gtk::SpinButton *>(
                                  sigc::mem_fun(*getSphereView(), &GlSphereView::lat_changed),
                                  pLat));
    }
    Gtk::SpinButton* pLon{nullptr};
    refBuilder->get_widget("lon", pLon);
    if(pLon) {
        pLon->set_increments(1, 10);
        pLon->set_range(-180, 180);
        pLon->set_value(config->getLongitude());
        pLon->signal_value_changed().connect(sigc::bind<Gtk::SpinButton *>(
                                   sigc::mem_fun(*getSphereView(), &GlSphereView::lon_changed),
                                   pLon));
    }
    Gtk::Entry* pTimeFormat{nullptr};
    refBuilder->get_widget("time_format", pTimeFormat);
    if (pTimeFormat) {
        pTimeFormat->set_text(config->getTimeFormat());
        pTimeFormat->signal_changed().connect(sigc::bind<Gtk::Entry *>(
                                   sigc::mem_fun(*getSphereView(), &GlSphereView::time_format_changed),
                                   pTimeFormat));
    }
}

GlSphereView*
ConfigCoordGrid::getSphereView()
{
    return dynamic_cast<GlSphereView*>(m_sphereView);
}

ConfigTextureGrid::ConfigTextureGrid(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, GlSphereView* sphereView)
: BaseConfigGrid(cobject, refBuilder, sphereView)
{
    auto config = std::dynamic_pointer_cast<Config>(m_sphereView->get_config());
    Gtk::FileChooserButton* dayFcBtn{nullptr};
    refBuilder->get_widget("day", dayFcBtn);
    if (dayFcBtn) {
        //std::cout << "day " << this.getDayTexureFile() << std::endl;
        dayFcBtn->set_filename(config->getDayTextureFile());
        //if (this.getDayTexureFile().length() > 0) {
        //    Glib::RefPtr<Gio::File> file = Gio::File::create_for_path(this.getDayTexureFile());
        //    dayFcBtn->set_current_name(file->get_parse_name());
        //}
        dayFcBtn->signal_file_set().connect(sigc::bind<Gtk::FileChooserButton *>(
                                   sigc::mem_fun(*this, &ConfigTextureGrid::daytex_changed),
                                   dayFcBtn));
    }
    Gtk::Button* clearDay{nullptr};
    refBuilder->get_widget("clearDay", clearDay);
    if (clearDay) {
        clearDay->signal_clicked().connect(sigc::bind<Gtk::FileChooserButton *>(
                                   sigc::mem_fun(*this, &ConfigTextureGrid::clearDayTextureFile),
                                   dayFcBtn));
    }
    Gtk::FileChooserButton* nightFcBtn{nullptr};
    refBuilder->get_widget("night", nightFcBtn);
    if (nightFcBtn) {
        nightFcBtn->set_filename(config->getNightTextureFile());
        nightFcBtn->signal_file_set().connect(sigc::bind<Gtk::FileChooserButton *>(
                                   sigc::mem_fun(*this, &ConfigTextureGrid::nighttex_changed),
                                   nightFcBtn));
    }
    Gtk::Button* clearNight{nullptr};
    refBuilder->get_widget("clearNight", clearNight);
    if (clearNight) {
        clearNight->signal_clicked().connect(sigc::bind<Gtk::FileChooserButton *>(
                                   sigc::mem_fun(*this, &ConfigTextureGrid::clearNightTextureFile),
                                   nightFcBtn));
    }
}

GlSphereView*
ConfigTextureGrid::getSphereView()
{
    return dynamic_cast<GlSphereView*>(m_sphereView);
}

void
ConfigTextureGrid::clearNightTextureFile(Gtk::FileChooserButton* nightFcBtn)
{
    std::string cl("");
    getSphereView()->setNightTextureFile(cl);
    nightFcBtn->set_filename(cl);
}

void
ConfigTextureGrid::clearDayTextureFile(Gtk::FileChooserButton* dayFcBtn)
{
    std::string cl("");
    getSphereView()->setDayTextureFile(cl);
    dayFcBtn->set_filename(cl);
}

void
ConfigTextureGrid::daytex_changed(Gtk::FileChooserButton* dayFcBtn)
{
    std::string file = dayFcBtn->get_filename();
    getSphereView()->setDayTextureFile(file);
}

void
ConfigTextureGrid::nighttex_changed(Gtk::FileChooserButton* nightFcBtn)
{
    std::string file = nightFcBtn->get_filename();
    getSphereView()->setNightTextureFile(file);
}

ConfigLigthingGrid::ConfigLigthingGrid(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, GlSphereView* sphereView)
: BaseConfigGrid(cobject, refBuilder, sphereView)
{
    //Gtk::Label* pFormatLink{nullptr};
    //refBuilder->get_widget("formatLink", pFormatLink);
    //pFormatLink->set_label(psc::fmt::vformat(
    //        _("<a href=\"{}\">glib DateTime format</a> and <a href=\"{}\">strftime</a>"),
    //            psc::fmt::make_format_args(
    //                "https://gnome.pages.gitlab.gnome.org/glibmm/classGlib_1_1DateTime.html#a7795905c8db173a973f965a7f27c7f51"
    //              , "https://cplusplus.com/reference/ctime/strftime/")));
    auto config = std::dynamic_pointer_cast<Config>(m_sphereView->get_config());
    Gtk::Scale* pAmbient{nullptr};
    refBuilder->get_widget("ambient", pAmbient);
    if (pAmbient) {
        pAmbient->set_increments(0.01, 0.1);
        pAmbient->set_range(0.1, 1.0);
        pAmbient->set_value(config->getAmbient());
        pAmbient->signal_value_changed().connect(sigc::bind<Gtk::Scale *>(
                                   sigc::mem_fun(*sphereView, &GlSphereView::ambient_changed),
                                   pAmbient));
    }
    Gtk::Scale* pDiffuse{nullptr};
    refBuilder->get_widget("diffuse", pDiffuse);
    if (pDiffuse) {
        pDiffuse->set_increments(10, 50);
        pDiffuse->set_range(100, 2000);
        pDiffuse->set_value(config->getDiffuse());
        pDiffuse->signal_value_changed().connect(sigc::bind<Gtk::Scale *>(
                                   sigc::mem_fun(*sphereView, &GlSphereView::diffuse_changed),
                                   pDiffuse));
    }
    Gtk::Scale* pSpecular{nullptr};
    refBuilder->get_widget("specular", pSpecular);
    if (pSpecular) {
        pSpecular->set_increments(10, 50);
        pSpecular->set_range(100, 2000);
        pSpecular->set_value(config->getSpecular());
        pSpecular->signal_value_changed().connect(sigc::bind<Gtk::Scale *>(
                                   sigc::mem_fun(*sphereView, &GlSphereView::specular_changed),
                                   pSpecular));
    }
    Gtk::Scale* pSpecularPower{nullptr};
    refBuilder->get_widget("specular_power", pSpecularPower);
    if (pSpecularPower) {
        pSpecularPower->set_increments(1, 10);
        pSpecularPower->set_range(1, 25);
        pSpecularPower->set_value(config->getSpecularPower());
        pSpecularPower->signal_value_changed().connect(sigc::bind<Gtk::Scale *>(
                                   sigc::mem_fun(*sphereView, &GlSphereView::specular_power_changed),
                                   pSpecularPower));
    }
    Gtk::Scale* pTwilight{nullptr};
    refBuilder->get_widget("twilight", pTwilight);
    if (pTwilight) {
        pTwilight->set_increments(1, 10);
        pTwilight->set_range(0.0, 40.0);  // use degree
        pTwilight->set_value(config->getTwilight() * 180.0f);  // dot -1..1 to degree -180..180
        pTwilight->signal_value_changed().connect(sigc::bind<Gtk::Scale *>(
                                   sigc::mem_fun(*sphereView, &GlSphereView::twilight_changed),
                                   pTwilight));
    }
    Gtk::CheckButton* pDebug{nullptr};
    refBuilder->get_widget("debug", pDebug);
    if (pDebug) {
        pDebug->set_active(config->getDebug() != 0);
        pDebug->signal_toggled().connect(sigc::bind<Gtk::CheckButton *>(
                                   sigc::mem_fun(*sphereView, &GlSphereView::debug_changed),
                                   pDebug));
    }
    Gtk::Scale* pScaleDistance{nullptr};
    refBuilder->get_widget("distance", pScaleDistance);
    if (pScaleDistance) {
        pScaleDistance->set_increments(1, 10);
        pScaleDistance->set_range(10, 100);
        pScaleDistance->set_value(config->getDistance());
        pScaleDistance->signal_value_changed().connect(sigc::bind<Gtk::Scale *>(
                                   sigc::mem_fun(*sphereView, &GlSphereView::distance_changed),
                                   pScaleDistance));
    }
}

GlSphereView*
ConfigLigthingGrid::getSphereView()
{
    return dynamic_cast<GlSphereView*>(m_sphereView);
}

ConfigGeoJsonGrid::ConfigGeoJsonGrid(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, GlSphereView* sphereView)
: BaseConfigGrid(cobject, refBuilder, sphereView)
{
    auto config = std::dynamic_pointer_cast<Config>(m_sphereView->get_config());
    refBuilder->get_widget("geoFileButton", m_geoJsonButton);
    if (m_geoJsonButton) {
        m_geoJsonButton->set_filename(config->getGeoJsonFile());
        m_geoJsonButton->signal_file_set()
                .connect(sigc::mem_fun(*this, &ConfigGeoJsonGrid::geojsonfile_changed));
    }
    Gtk::Button* geoClearFile{nullptr};
    refBuilder->get_widget("geoClearFile", geoClearFile);
    if (geoClearFile) {
        geoClearFile->signal_clicked()
                .connect(sigc::mem_fun(*this, &ConfigGeoJsonGrid::clearGeoFile));
    }
}



void
ConfigGeoJsonGrid::geojsonfile_changed()
{
    Glib::ustring file = m_geoJsonButton->get_filename();
    auto config = std::dynamic_pointer_cast<Config>(m_sphereView->get_config());
    bool success = getSphereView()->setGeoJsonFile(file);
    if (!success) {
        file = "";
    }
    m_geoJsonButton->set_filename(file);
    config->setGeoJsonFile(file);
}

GlSphereView*
ConfigGeoJsonGrid::getSphereView()
{
    return dynamic_cast<GlSphereView*>(m_sphereView);
}
void
ConfigGeoJsonGrid::clearGeoFile()
{
    auto config = std::dynamic_pointer_cast<Config>(m_sphereView->get_config());
    config->setGeoJsonFile("");
    m_geoJsonButton->set_filename("");
    getSphereView()->setGeoJsonFile("");
}


ConfigDialog::ConfigDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, GlSphereView* sphereView)
: Gtk::Dialog(cobject)
{
    refBuilder->get_widget_derived("configCoordGrid", m_configCoordGrid, sphereView);
    refBuilder->get_widget_derived("configTextureGrid", m_configTextureGrid, sphereView);
    refBuilder->get_widget_derived("configLigthingGrid", m_configLigthingGrid, sphereView);
    refBuilder->get_widget_derived("configWeatherGrid", m_configWeatherGrid, sphereView);
    refBuilder->get_widget_derived("configGeoJsonGrid", m_configGeoJsonGrid, sphereView);
}


ConfigDialog*
ConfigDialog::create(GlSphereView* sphereView)
{
    auto refBuilder = Gtk::Builder::create();
    try {
        refBuilder->add_from_resource(RESOURCE::resource("cfg-dlg.ui"));
        ConfigDialog* cfgdlg;
        refBuilder->get_widget_derived("cfg-dlg", cfgdlg, sphereView);
        //auto cfgdlg = Glib::RefPtr<>::cast_dynamic(object);
        if (cfgdlg) {
            return cfgdlg;
        }
        else {
            sphereView->showMessage(
                psc::fmt::vformat(
                      _("No \"{}\" object in {}")
                    , psc::fmt::make_format_args("cfg-dlg", "cfg-dlg.ui"))
                , Gtk::MessageType::MESSAGE_ERROR);
        }
    }
    catch (const Glib::Error& ex) {
        sphereView->showMessage(
                psc::fmt::vformat(
                      _("Error {} while loading {}")
                    , psc::fmt::make_format_args(ex, "cfg-dlg.ui"))
                , Gtk::MessageType::MESSAGE_ERROR);
    }
    return nullptr;
}

