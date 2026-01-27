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


#pragma once

#include <gtkmm.h>
#include <glibmm.h>
#include <string>
#include <locale>
#include <memory>
#include <Log.hpp>
#include <Geom2.hpp>
#include <Text2.hpp>
#include <Matrix.hpp>
#include <SphereContext.hpp>
#include <TextContext.hpp>
#include <Font2.hpp>
#include <NaviGlArea.hpp>
#include <Scene.hpp>
#include <MarkContext.hpp>

#include "TimezoneInfo.hpp"
#include "MoonContext.hpp"
#include "Config.hpp"
#include "GlGlobeWindow.hpp"
#include "Weather.hpp"


class ConfigDialog;
class SphereGlArea;
class GlGlobeWindow;

class GlSphereView
: public Scene
, public WeatherConsumer
{
public:
    GlSphereView(const std::shared_ptr<Config>& config, Glib::StdStringView exec);
    virtual ~GlSphereView();
    Matrix getLookAt(Vector &position, Vector &direction, Vector &up) override;
    Position getIntialPosition() override;
    Rotational getInitalAngleDegree() override;
    float getZFar() override;

    gboolean init_shaders(Glib::Error &error) override;
    void init(Gtk::GLArea *glArea) override;
    void unrealize() override;
    void draw(Gtk::GLArea *glArea, Matrix &proj, Matrix &view) override;
    Glib::ustring customize_time(Glib::ustring prepared);
    psc::gl::aptrGeom2 on_click_select(GdkEventButton* event, float mx, float my) override;
    std::shared_ptr<Config> get_config() {
        return m_config;
    }
    std::shared_ptr<Weather> get_weather() {
        return m_weather;
    }
    void pos_update();
    void update_weather_tex();
    void lon_changed(Gtk::SpinButton* lon_spin);
    void lat_changed(Gtk::SpinButton* lat_spin);
    void setDayTextureFile(std::string &dayTex);
    void setNightTextureFile(std::string &nightTex);
    void ambient_changed(Gtk::Scale* ambient);
    void diffuse_changed(Gtk::Scale* diffuse);
    void specular_changed(Gtk::Scale* specular);
    void specular_power_changed(Gtk::Scale* specular_pow);
    void twilight_changed(Gtk::Scale* twilight);
    void distance_changed(Gtk::Scale* distance);
    void debug_changed(Gtk::CheckButton* debug);
    void time_format_changed(Gtk::Entry *time_format);
    //GeoDb *getGeoDb();
    bool on_motion_notify_event(GdkEventMotion* event, float mx, float my) override;
    void weather_image_notify(WeatherImageRequest& request) override;
    int get_weather_image_size() override;
    void weather_transparency_changed(Gtk::Scale *weather_transparency);
    bool setGeoJsonFile(const Glib::ustring& file);
    std::shared_ptr<Weather> refresh_weather_service();
    void request_weather_product();
    void showMessage(const std::string& msg, Gtk::MessageType msgType = Gtk::MessageType::MESSAGE_INFO);
    // as we have no Gis, so limit the complexity of usable files
    static constexpr goffset GEO_FILE_SIZE_LIMIT{200*1024};
    std::string findFile(const std::string& name);

protected:
    Gdk::EventMask getAddEventMask() override;
    double julianDate();
    void calcuateEarthLight();
    void color_to_alpha(Glib::RefPtr<Gdk::Pixbuf> pix);
    void calcuateMoonLight();
    gboolean init_moon_shaders(Glib::Error &error);
    gboolean init_earth_shaders(Glib::Error &error);
private:
    std::shared_ptr<Config> m_config;
    Glib::StdStringView m_exec;
    Rotational get_rotation();
    SphereContext *m_earthContext;
    TextContext *m_textContext;
    SphereGlArea *m_naviGlArea;
    Matrix m_fixView;
    psc::gl::aptrGeom2 m_earth;
    psc::gl::aptrTex2 m_dayTex;
    psc::gl::aptrTex2 m_nightTex;
    psc::gl::aptrTex2 m_normalMapTex;
    psc::gl::aptrTex2 m_speculatMapTex;
    psc::gl::aptrTex2 m_weatherTex;
    psc::gl::ptrFont2 m_font;
    sigc::connection m_timer;               // Timer for regular updates
    Vector m_light;
    float m_earth_declination_deg;
    void updateTimer();
    Glib::ustring hm(const double& timeM);
    void calcuateLight();
    gboolean view_update();
    psc::gl::aptrText2 m_text;
    psc::gl::aptrGeom2 debugGeom;
    std::shared_ptr<TimezoneInfo> m_timezoneInfo;
    MarkContext *m_markContext;
    double m_sunRise;
    double m_sunSet;
    std::shared_ptr<Weather> m_weather;
    Glib::RefPtr<Gdk::Pixbuf> m_weather_pix;
    psc::gl::aptrGeom2 geoJsonGeom;
    static constexpr float EARTH_RADIUS{30.0f};
    MoonContext *m_moonContext;
    psc::gl::aptrGeom2 m_moon;
    psc::gl::aptrTex2 m_moonTex;
    Vector m_moonLight;

    // offs moon to display on single canvas
    static constexpr auto MOON_XOFFS{30.0f};
    static constexpr auto MOON_VIEW_DIST{70.0f};
    static constexpr auto EARTH_OFFS{-20.0f};
    static constexpr auto EARTH_DIST_CENTER{50.0f};

    std::shared_ptr<psc::log::Log> m_log;
};

struct RESOURCE {
    static constexpr const char * const PREFIX = "/de/pfeifer_syscon/glsceneapp/";

    static std::string resource(const char *file);
};
