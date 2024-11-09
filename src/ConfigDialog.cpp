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

#include <iostream>

#include "ConfigDialog.hpp"
#include "GlSphereView.hpp"
#include "Config.hpp"
#include "Weather.hpp"

BaseConfigGrid::BaseConfigGrid(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, GlSphereView* sphereView)
: Gtk::Grid(cobject)
, m_sphereView{sphereView}
{

}

ConfigCoordGrid::ConfigCoordGrid(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, GlSphereView* sphereView)
: BaseConfigGrid(cobject, refBuilder, sphereView)
{
    auto config = m_sphereView->get_config();
    Gtk::SpinButton* pLat = nullptr;
    refBuilder->get_widget("lat", pLat);
    if(pLat) {
        pLat->set_increments(1, 10);
        pLat->set_range(-90, 90);
        pLat->set_value(config->getLatitude());
        pLat->signal_value_changed().connect(sigc::bind<Gtk::SpinButton *>(
                                  sigc::mem_fun(*m_sphereView, &GlSphereView::lat_changed),
                                  pLat));
    }
    Gtk::SpinButton* pLon = nullptr;
    refBuilder->get_widget("lon", pLon);
    if(pLon) {
        pLon->set_increments(1, 10);
        pLon->set_range(-180, 180);
        pLon->set_value(config->getLongitude());
        pLon->signal_value_changed().connect(sigc::bind<Gtk::SpinButton *>(
                                   sigc::mem_fun(*m_sphereView, &GlSphereView::lon_changed),
                                   pLon));
    }
    Gtk::Entry* pTimeFormat = nullptr;
    refBuilder->get_widget("time_format", pTimeFormat);
    if (pTimeFormat) {
        pTimeFormat->set_text(config->getTimeFormat());
        pTimeFormat->signal_changed().connect(sigc::bind<Gtk::Entry *>(
                                   sigc::mem_fun(*m_sphereView, &GlSphereView::time_format_changed),
                                   pTimeFormat));
    }
}

ConfigTextureGrid::ConfigTextureGrid(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, GlSphereView* sphereView)
: BaseConfigGrid(cobject, refBuilder, sphereView)
{
    auto config = m_sphereView->get_config();
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
                                   sigc::mem_fun(*this, &ConfigTextureGrid::daytex_changed),
                                   dayFcBtn));
    }
    Gtk::Button* clearDay = nullptr;
    refBuilder->get_widget("clearDay", clearDay);
    if (clearDay) {
        clearDay->signal_clicked().connect(sigc::bind<Gtk::FileChooserButton *>(
                                   sigc::mem_fun(*this, &ConfigTextureGrid::clearDayTextureFile),
                                   dayFcBtn));
    }
    Gtk::FileChooserButton* nightFcBtn = nullptr;
    refBuilder->get_widget("night", nightFcBtn);
    if (nightFcBtn) {
        nightFcBtn->set_filename(config->getNightTextureFile());
        nightFcBtn->signal_file_set().connect(sigc::bind<Gtk::FileChooserButton *>(
                                   sigc::mem_fun(*this, &ConfigTextureGrid::nighttex_changed),
                                   nightFcBtn));
    }
    Gtk::Button* clearNight = nullptr;
    refBuilder->get_widget("clearNight", clearNight);
    if (clearNight) {
        clearNight->signal_clicked().connect(sigc::bind<Gtk::FileChooserButton *>(
                                   sigc::mem_fun(*this, &ConfigTextureGrid::clearNightTextureFile),
                                   nightFcBtn));
    }
}

void
ConfigTextureGrid::clearNightTextureFile(Gtk::FileChooserButton* nightFcBtn)
{
    std::string cl("");
    m_sphereView->setNightTextureFile(cl);
    nightFcBtn->set_filename(cl);
}

void
ConfigTextureGrid::clearDayTextureFile(Gtk::FileChooserButton* dayFcBtn)
{
    std::string cl("");
    m_sphereView->setDayTextureFile(cl);
    dayFcBtn->set_filename(cl);
}

void
ConfigTextureGrid::daytex_changed(Gtk::FileChooserButton* dayFcBtn)
{
    std::string file = dayFcBtn->get_filename();
    m_sphereView->setDayTextureFile(file);
}

void
ConfigTextureGrid::nighttex_changed(Gtk::FileChooserButton* nightFcBtn)
{
    std::string file = nightFcBtn->get_filename();
    m_sphereView->setNightTextureFile(file);
}

ConfigLigthingGrid::ConfigLigthingGrid(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, GlSphereView* sphereView)
: BaseConfigGrid(cobject, refBuilder, sphereView)
{
    auto config = m_sphereView->get_config();
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
    Gtk::Scale* pScaleDistance = nullptr;
    refBuilder->get_widget("distance", pScaleDistance);
    if (pScaleDistance) {
        pScaleDistance->set_increments(1, 10);
        pScaleDistance->set_range(10, 100);
        pScaleDistance->set_value(config->getDistance());
        pScaleDistance->signal_value_changed().connect(sigc::bind<Gtk::Scale *>(
                                   sigc::mem_fun(*m_sphereView, &GlSphereView::distance_changed),
                                   pScaleDistance));
    }
}

ConfigWeatherGrid::ConfigWeatherGrid(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, GlSphereView* sphereView)
: BaseConfigGrid(cobject, refBuilder, sphereView)
{
    auto config = m_sphereView->get_config();
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
    refBuilder->get_widget("comboWeatherProduct", m_weatherProductCombo);
    if (m_weatherProductCombo) {
        m_weatherProductCombo->append("", "");  // keep empty element
        refreshWeatherProducts();
        m_weatherProductCombo->signal_changed().connect(
            sigc::mem_fun(*this, &ConfigWeatherGrid::weather_product_changed));

    }
    refBuilder->get_widget("comboWeatherService", m_weatherServiceCombo);
    if (m_weatherServiceCombo) {
        auto confs = config->getWebMapServices();
        m_weatherServiceCombo->append("", "");  // allow empty selection
        for (auto servConf : confs) {
            m_weatherServiceCombo->append(servConf->getName(), servConf->getName());
        }
        m_weatherServiceCombo->set_active_id(config->getWeatherServiceId());
        m_weatherServiceCombo->signal_changed().connect(
                    sigc::mem_fun(*this, &ConfigWeatherGrid::weather_service_changed));
    }
    refBuilder->get_widget_derived("weatherBounds", m_boundsDisplay);
    setWeatherDescription();
}

void
ConfigWeatherGrid::setLegendWeather(Glib::RefPtr<Gdk::Pixbuf> legend)
{
    if (m_LegendWeather) {
        m_LegendWeather->set(legend);
    }
}

void
ConfigWeatherGrid::setWeatherDescription()
{
    auto weatherProdId = m_sphereView->get_config()->getWeatherProductId();
    std::shared_ptr<WeatherProduct> weatherProd;
    if (m_sphereView->get_weather()) {
        weatherProd = m_sphereView->get_weather()->find_product(weatherProdId);
    }
    Glib::ustring desc;
    if (weatherProd) {
        desc = weatherProd->get_description();
    }
    if (m_DescWeather) {
        m_DescWeather->get_buffer()->set_text(desc);
    }
    if (m_LegendWeather) {
        if (weatherProd) {
            auto legend = m_sphereView->get_weather()->get_legend(weatherProd);
            if (legend) {
                m_LegendWeather->set(legend);
            }
            else {  // notify if legend becomes available
                weatherProd->signal_legend().connect(
                        sigc::mem_fun(*this, &ConfigWeatherGrid::setLegendWeather));
            }
        }
        else {
            m_LegendWeather->clear();
        }
    }
    if (m_boundsDisplay && weatherProd) {
        m_boundsDisplay->setBounds(weatherProd->getBounds());
    }
}


void
ConfigWeatherGrid::weather_product_changed()
{
    if (!m_blockWeatherProductUpdate) {
        #ifdef CONFIG_DEBUG
        std::cout << "ConfigWeatherGrid::weather_product_changed" << std::endl;
        #endif
        auto id = m_weatherProductCombo->get_active_id();
        m_sphereView->get_config()->setWeatherProductId(id);
        setWeatherDescription();
        m_sphereView->request_weather_product();
    }
}

void
ConfigWeatherGrid::weather_service_changed()
{
    #ifdef CONFIG_DEBUG
    std::cout << "ConfigWeatherGrid::weather_service_changed" << std::endl;
    #endif
    auto id = m_weatherServiceCombo->get_active_id();
    m_sphereView->get_config()->setWeatherServiceId(id);
    m_sphereView->get_config()->setWeatherProductId("");
    m_blockWeatherProductUpdate = true;
    m_weatherProductCombo->remove_all();
    std::shared_ptr<Weather> weather = m_sphereView->refresh_weather_service();
    if (weather) {
        weather->signal_products_completed().connect(
            sigc::mem_fun(*this, &ConfigWeatherGrid::refreshWeatherProducts));
    }
    setWeatherDescription();
    m_blockWeatherProductUpdate = false;
}


void
ConfigWeatherGrid::refreshWeatherProducts()
{
    #ifdef CONFIG_DEBUG
    std::cout << "ConfigWeatherGrid::refreshWeatherProducts" << std::endl;
    #endif
    m_blockWeatherProductUpdate = true;
    m_weatherProductCombo->unset_active();
    auto list = Glib::RefPtr<Gtk::ListStore>::cast_dynamic(m_weatherProductCombo->get_model());
    if (list) {
        #ifdef CONFIG_DEBUG
        std::cout << "ConfigWeatherGrid::refreshWeatherProducts got list" << std::endl;
        #endif
        auto chlds = list->children();
        int i = 0;
        for (auto chld : chlds) {
            if (i > 0) {
                list->erase(chld);
            }
            ++i;
        }
    }
    else {
        std::cout << "ConfigWeatherGrid::refreshWeatherProducts got list no!" << std::endl;
    }
    auto weather = m_sphereView->get_weather();
    if (weather) {
        auto products = weather->get_products();
        #ifdef CONFIG_DEBUG
        std::cout << "ConfigWeatherGrid::refreshWeatherProducts products " << products.size() << std::endl;
        #endif

        for (auto product : products) {
            if (product->is_displayable()) {
                m_weatherProductCombo->append(product->get_id(), product->get_name());
            }
        }
    }
    else {
        std::cout << "ConfigWeatherGrid::refreshWeatherProducts have no weather." << std::endl;
    }
    m_blockWeatherProductUpdate = false;
    auto config = m_sphereView->get_config();
    if (!config->getWeatherProductId().empty()) {
        m_weatherProductCombo->set_active_id(config->getWeatherProductId());
    }
    else {
        m_weatherProductCombo->set_active(0);
    }
}

ConfigGeoJsonGrid::ConfigGeoJsonGrid(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, GlSphereView* sphereView)
: BaseConfigGrid(cobject, refBuilder, sphereView)
{
    auto config = m_sphereView->get_config();
    refBuilder->get_widget("geoFileButton", m_geoJsonButton);
    if (m_geoJsonButton) {
        m_geoJsonButton->set_filename(config->getGeoJsonFile());
        m_geoJsonButton->signal_file_set().connect(
                                   sigc::mem_fun(*this, &ConfigGeoJsonGrid::geojsonfile_changed));
    }
    Gtk::Button* geoClearFile = nullptr;
    refBuilder->get_widget("geoClearFile", geoClearFile);
    if (geoClearFile) {
        geoClearFile->signal_clicked().connect(
                                   sigc::mem_fun(*this, &ConfigGeoJsonGrid::clearGeoFile));
    }
}



void
ConfigGeoJsonGrid::geojsonfile_changed()
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
ConfigGeoJsonGrid::clearGeoFile()
{
    m_sphereView->get_config()->setGeoJsonFile("");
    m_geoJsonButton->set_filename("");
    m_sphereView->setGeoJsonFile("");
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
            std::cerr << "ConfigDialog::create: No \"cfg-dlg\" object in cfg-dlg.ui"
                << std::endl;
        }
    }
    catch (const Glib::Error& ex) {
        std::cerr << "ConfigDialog::create " << ex.what() << std::endl;
    }
    return nullptr;
}

