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

#pragma once

#include <gtkmm.h>
#include <BoundsDisplay.hpp>

#include "WeatherConfigGrid.hpp"

#undef CONFIG_DEBUG

class GlSphereView;
class Config;


class ConfigCoordGrid
: public BaseConfigGrid
{
public:
    ConfigCoordGrid(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, GlSphereView* sphereView);
    virtual ~ConfigCoordGrid() = default;
protected:
    GlSphereView* getSphereView();
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
protected:
    GlSphereView* getSphereView();
};

class ConfigLigthingGrid
: public BaseConfigGrid
{
public:
    ConfigLigthingGrid(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, GlSphereView* sphereView);
    virtual ~ConfigLigthingGrid() = default;
protected:
    GlSphereView* getSphereView();
};


class ConfigGeoJsonGrid
: public BaseConfigGrid
{
public:
    ConfigGeoJsonGrid(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, GlSphereView* sphereView);
    virtual ~ConfigGeoJsonGrid() = default;
protected:
    GlSphereView* getSphereView();
    void geojsonfile_changed();
    void clearGeoFile();
private:
    Gtk::FileChooserButton* m_geoJsonButton{nullptr};

};

class ConfigDialog
: public Gtk::Dialog
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

