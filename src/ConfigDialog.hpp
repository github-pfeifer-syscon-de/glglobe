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

#pragma once

#include <gtkmm.h>

#include "BoundsDisplay.hpp"

#undef CONFIG_DEBUG

class GlSphereView;
class Config;
class WeatherProduct;

class BaseConfigGrid
: public Gtk::Grid
{
protected:
    BaseConfigGrid(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, GlSphereView* sphereView);
    virtual ~BaseConfigGrid() = default;
    GlSphereView* m_sphereView;

};

class ConfigCoordGrid
: public BaseConfigGrid
{
public:
    ConfigCoordGrid(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, GlSphereView* sphereView);
    virtual ~ConfigCoordGrid() = default;
};

class ConfigTextureGrid
: public BaseConfigGrid
{
public:
    ConfigTextureGrid(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, GlSphereView* sphereView);
    virtual ~ConfigTextureGrid() = default;

    void clearNightTextureFile(Gtk::FileChooserButton* nightFcBtn);
    void clearDayTextureFile(Gtk::FileChooserButton* dayFcBtn);
    void daytex_changed(Gtk::FileChooserButton* dayFcBtn);
    void nighttex_changed(Gtk::FileChooserButton* nightFcBtn);
};

class ConfigLigthingGrid
: public BaseConfigGrid
{
public:
    ConfigLigthingGrid(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, GlSphereView* sphereView);
    virtual ~ConfigLigthingGrid() = default;

};

class ConfigWeatherGrid
: public BaseConfigGrid
{
public:
    ConfigWeatherGrid(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, GlSphereView* sphereView);
    virtual ~ConfigWeatherGrid() = default;
    void setLegendWeather(Glib::RefPtr<Gdk::Pixbuf> legend);
    void setWeatherDescription();
    void refreshWeatherProducts();

protected:
    void weather_product_changed();
    void weather_service_changed();
private:
    Gtk::Image* m_LegendWeather{nullptr};
    Gtk::TextView* m_DescWeather{nullptr};
    Gtk::ComboBoxText* m_weatherProductCombo{nullptr};
    Gtk::ComboBoxText* m_weatherServiceCombo{nullptr};
    bool m_blockWeatherProductUpdate{false};
    BoundsDisplay *m_boundsDisplay{nullptr};
};

class ConfigGeoJsonGrid
: public BaseConfigGrid
{
public:
    ConfigGeoJsonGrid(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, GlSphereView* sphereView);
    virtual ~ConfigGeoJsonGrid() = default;
protected:
    void geojsonfile_changed();
    void clearGeoFile();
private:
    Gtk::FileChooserButton* m_geoJsonButton{nullptr};

};

class ConfigDialog : public Gtk::Dialog
{
public:
    ConfigDialog(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, GlSphereView* sphereView);
    virtual ~ConfigDialog() = default;
    static ConfigDialog* create(GlSphereView* sphereView);

private:
    ConfigCoordGrid* m_configCoordGrid{nullptr};
    ConfigTextureGrid* m_configTextureGrid{nullptr};
    ConfigLigthingGrid* m_configLigthingGrid{nullptr};
    ConfigWeatherGrid* m_configWeatherGrid{nullptr};
    ConfigGeoJsonGrid* m_configGeoJsonGrid{nullptr};

};

