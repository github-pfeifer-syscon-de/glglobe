/*
 * Copyright (C) 2023 RPf <gpl3@pfeifer-syscon.de>
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


#include "ConfigDialog.hpp"
#include "GlSphereView.hpp"
#include "Config.hpp"

ConfigDialog::ConfigDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, GlSphereView* sphereView)
: Gtk::Dialog(cobject)
, m_sphereView{sphereView}
{
    Config* config = m_sphereView->get_config();
    Gtk::SpinButton* pLat = nullptr;
    refBuilder->get_widget("lat", pLat);
    if(pLat) {
        pLat->set_increments(1, 10);
        pLat->set_range(-90, 90);
        pLat->set_value(config->getLatitude());
        pLat->signal_changed().connect(sigc::bind<Gtk::SpinButton *>(
                                  sigc::mem_fun(*m_sphereView, &GlSphereView::lat_changed),
                                  pLat));
    }
    Gtk::SpinButton* pLon = nullptr;
    refBuilder->get_widget("lon", pLon);
    if(pLon) {
        pLon->set_increments(1, 10);
        pLon->set_range(-180, 180);
        pLon->set_value(config->getLongitude());
        pLon->signal_changed().connect(sigc::bind<Gtk::SpinButton *>(
                                   sigc::mem_fun(*m_sphereView, &GlSphereView::lon_changed),
                                   pLon));
    }
    Gtk::FileChooserButton* dayFcBtn = nullptr;
    refBuilder->get_widget("day", dayFcBtn);
    if (dayFcBtn) {
        //std::cout << "day " << this.getDayTexureFile() << std::endl;
        dayFcBtn->set_filename(config->getDayTextureFile());
        //if (this.getDayTexureFile().length() > 0) {
        //    Glib::RefPtr<Gio::File> file = Gio::File::create_for_path(this.getDayTexureFile());
        //    dayFcBtn->set_current_name(file->get_parse_name());
        //}
        dayFcBtn->signal_file_set().connect(sigc::bind<Gtk::FileChooserButton *>(
                                   sigc::mem_fun(*this, &ConfigDialog::daytex_changed),
                                   dayFcBtn));
    }
    Gtk::Button* clearDay = nullptr;
    refBuilder->get_widget("clearDay", clearDay);
    if (clearDay) {
        clearDay->signal_clicked().connect(sigc::bind<Gtk::FileChooserButton *>(
                                   sigc::mem_fun(*this, &ConfigDialog::clearDayTextureFile),
                                   dayFcBtn));
    }
    Gtk::FileChooserButton* nightFcBtn = nullptr;
    refBuilder->get_widget("night", nightFcBtn);
    if (nightFcBtn) {
        nightFcBtn->set_filename(config->getNightTextureFile());
        nightFcBtn->signal_file_set().connect(sigc::bind<Gtk::FileChooserButton *>(
                                   sigc::mem_fun(*this, &ConfigDialog::nighttex_changed),
                                   nightFcBtn));
    }
    Gtk::Button* clearNight = nullptr;
    refBuilder->get_widget("clearNight", clearNight);
    if (clearNight) {
        clearNight->signal_clicked().connect(sigc::bind<Gtk::FileChooserButton *>(
                                   sigc::mem_fun(*this, &ConfigDialog::clearNightTextureFile),
                                   nightFcBtn));
    }
    Gtk::Scale* pAmbient = nullptr;
    refBuilder->get_widget("ambient", pAmbient);
    if (pAmbient) {
        pAmbient->set_increments(0.01, 0.1);
        pAmbient->set_range(0.1, 1.0);
        pAmbient->set_value(config->getAmbient());
        pAmbient->signal_value_changed().connect(sigc::bind<Gtk::Scale *>(
                                   sigc::mem_fun(*m_sphereView, &GlSphereView::ambient_changed),
                                   pAmbient));
    }
    Gtk::Scale* pDiffuse = nullptr;
    refBuilder->get_widget("diffuse", pDiffuse);
    if (pDiffuse) {
        pDiffuse->set_increments(10, 50);
        pDiffuse->set_range(100, 2000);
        pDiffuse->set_value(config->getDiffuse());
        pDiffuse->signal_value_changed().connect(sigc::bind<Gtk::Scale *>(
                                   sigc::mem_fun(*m_sphereView, &GlSphereView::diffuse_changed),
                                   pDiffuse));
    }
    Gtk::Scale* pSpecular = nullptr;
    refBuilder->get_widget("specular", pSpecular);
    if (pSpecular) {
        pSpecular->set_increments(10, 50);
        pSpecular->set_range(100, 2000);
        pSpecular->set_value(config->getSpecular());
        pSpecular->signal_value_changed().connect(sigc::bind<Gtk::Scale *>(
                                   sigc::mem_fun(*m_sphereView, &GlSphereView::specular_changed),
                                   pSpecular));
    }
    Gtk::Scale* pSpecularPower = nullptr;
    refBuilder->get_widget("specular_power", pSpecularPower);
    if (pSpecularPower) {
        pSpecularPower->set_increments(1, 10);
        pSpecularPower->set_range(1, 25);
        pSpecularPower->set_value(config->getSpecularPower());
        pSpecularPower->signal_value_changed().connect(sigc::bind<Gtk::Scale *>(
                                   sigc::mem_fun(*m_sphereView, &GlSphereView::specular_power_changed),
                                   pSpecularPower));
    }
    Gtk::Scale* pTwilight = nullptr;
    refBuilder->get_widget("twilight", pTwilight);
    if (pTwilight) {
        pTwilight->set_increments(1, 10);
        pTwilight->set_range(0.0, 40.0);  // use degree
        pTwilight->set_value(config->getTwilight() * 180.0f);  // dot -1..1 to degree -180..180
        pTwilight->signal_value_changed().connect(sigc::bind<Gtk::Scale *>(
                                   sigc::mem_fun(*m_sphereView, &GlSphereView::twilight_changed),
                                   pTwilight));
    }
    Gtk::CheckButton* pDebug = nullptr;
    refBuilder->get_widget("debug", pDebug);
    if (pDebug) {
        pDebug->set_active(config->getDebug() != 0);
        pDebug->signal_toggled().connect(sigc::bind<Gtk::CheckButton *>(
                                   sigc::mem_fun(*m_sphereView, &GlSphereView::debug_changed),
                                   pDebug));
    }
    Gtk::Scale* pDistance = nullptr;
    refBuilder->get_widget("distance", pDistance);
    if (pDistance) {
        pDistance->set_increments(1, 10);
        pDistance->set_range(10, 100);
        pDistance->set_value(config->getDistance());
        pDistance->signal_value_changed().connect(sigc::bind<Gtk::Scale *>(
                                   sigc::mem_fun(*m_sphereView, &GlSphereView::distance_changed),
                                   pDistance));
    }
    Gtk::Entry* pTimeFormat = nullptr;
    refBuilder->get_widget("time_format", pTimeFormat);
    if (pDistance) {
        pTimeFormat->set_text(config->getTimeFormat());
        pTimeFormat->signal_changed().connect(sigc::bind<Gtk::Entry *>(
                                   sigc::mem_fun(*m_sphereView, &GlSphereView::time_format_changed),
                                   pTimeFormat));
    }
    Gtk::Scale* pWeatherTransp = nullptr;
    refBuilder->get_widget("scaleWeather", pWeatherTransp);
    if (pWeatherTransp) {
        pWeatherTransp->set_increments(0.01, 0.1);
        pWeatherTransp->set_range(0.1, 2.0);
        pWeatherTransp->set_value(config->getWeatherTransparency());
        pWeatherTransp->signal_value_changed().connect(sigc::bind<Gtk::Scale *>(
                                   sigc::mem_fun(*m_sphereView, &GlSphereView::weather_transparency_changed),
                                   pWeatherTransp));
    }
    refBuilder->get_widget("descWeather", m_DescWeather);
    refBuilder->get_widget("legendWeather", m_LegendWeather);
    setWeatherDescription();

    Gtk::ComboBoxText* pWeatherCombo = nullptr;
    refBuilder->get_widget("comboWeather", pWeatherCombo);
    if (pWeatherCombo) {
        auto products = m_sphereView->get_weather()->get_products();
        pWeatherCombo->append("", "");  // allow empty selection
        for (auto product : products) {
            if (product->is_displayable()) {
                pWeatherCombo->append(product->get_id(), product->get_name());
            }
        }
        pWeatherCombo->set_active_id(config->getWeatherProductId());
        pWeatherCombo->signal_changed().connect(
                sigc::bind<Gtk::ComboBox*>(
                    sigc::mem_fun(*this, &ConfigDialog::weather_product_changed),
                        pWeatherCombo));
    }
    refBuilder->get_widget("geoFileButton", m_geoJsonButton);
    if (m_geoJsonButton) {
        m_geoJsonButton->set_filename(config->getGeoJsonFile());
        m_geoJsonButton->signal_file_set().connect(
                                   sigc::mem_fun(*this, &ConfigDialog::geojsonfile_changed));
    }
    Gtk::Button* geoClearFile = nullptr;
    refBuilder->get_widget("geoClearFile", geoClearFile);
    if (geoClearFile) {
        geoClearFile->signal_clicked().connect(
                                   sigc::mem_fun(*this, &ConfigDialog::clearGeoFile));
    }
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
            std::cerr << "ConfigDialog::create: No \"cfg-dlg\" object in cfg-dlg.ui"
                << std::endl;
        }
    }
    catch (const Glib::Error& ex) {
        std::cerr << "ConfigDialog::create " << ex.what() << std::endl;
    }
    return nullptr;
}

void
ConfigDialog::setLegendWeather(Glib::RefPtr<Gdk::Pixbuf> legend)
{
    if (m_LegendWeather) {
        m_LegendWeather->set(legend);
    }
}

void
ConfigDialog::setWeatherDescription()
{
    auto weatherProdId = m_sphereView->get_config()->getWeatherProductId();
    auto weatherProd = m_sphereView->get_weather()->find_product(weatherProdId);
    Glib::ustring desc;
    if (weatherProd) {
        desc = weatherProd->get_description();
    }
    if (m_DescWeather) {
        m_DescWeather->get_buffer()->set_text(desc);
    }
    if (m_LegendWeather && weatherProd) {
        auto legend = m_sphereView->get_weather()->get_legend(weatherProd);
        if (legend) {
            m_LegendWeather->set(legend);
        }
        else {  // notify if legend becomes available
            weatherProd->signal_legend().connect(
                    sigc::mem_fun(*this, &ConfigDialog::setLegendWeather));
        }
    }
}


void
ConfigDialog::geojsonfile_changed()
{
    Glib::ustring file = m_geoJsonButton->get_filename();
    bool success = m_sphereView->setGeoJsonFile(file);
    if (!success) {
        file = "";
    }
    m_geoJsonButton->set_filename(file);
    m_sphereView->get_config()->setGeoJsonFile(file);
}

void
ConfigDialog::clearGeoFile()
{
    m_sphereView->get_config()->setGeoJsonFile("");
    m_geoJsonButton->set_filename("");
    m_sphereView->setGeoJsonFile("");
}

void
ConfigDialog::weather_product_changed(Gtk::ComboBox *weather_product)
{
    auto id = weather_product->get_active_id();
    m_sphereView->get_config()->setWeatherProductId(id);
    setWeatherDescription();
    m_sphereView->request_weather_product();
}

void
ConfigDialog::clearNightTextureFile(Gtk::FileChooserButton* nightFcBtn)
{
    std::string cl("");
    m_sphereView->setNightTextureFile(cl);
    nightFcBtn->set_filename(cl);
}

void
ConfigDialog::clearDayTextureFile(Gtk::FileChooserButton* dayFcBtn)
{
    std::string cl("");
    m_sphereView->setDayTextureFile(cl);
    dayFcBtn->set_filename(cl);
}

void
ConfigDialog::daytex_changed(Gtk::FileChooserButton* dayFcBtn)
{
    std::string file = dayFcBtn->get_filename();
    m_sphereView->setDayTextureFile(file);
}

void
ConfigDialog::nighttex_changed(Gtk::FileChooserButton* nightFcBtn)
{
    std::string file = nightFcBtn->get_filename();
    m_sphereView->setNightTextureFile(file);
}
