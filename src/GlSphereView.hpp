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

#include "Geometry.hpp"
#include "Matrix.hpp"
#include "SphereContext.hpp"
#include "TextContext.hpp"
#include "Font.hpp"
#include "NaviGlArea.hpp"
#include "Scene.hpp"
#include "TimezoneInfo.hpp"
#include "MarkContext.hpp"
#include "MoonContext.hpp"
#include "Config.hpp"
#include "Weather.hpp"

class ConfigDialog;
class SphereGlArea;

class GlSphereView : public Scene, public WeatherConsumer {
public:
    GlSphereView(Config *config);
    virtual ~GlSphereView();
    Matrix getLookAt(Vector &position, Vector &direction, Vector &up) override;
    Position getIntialPosition() override;
    Rotational getInitalAngleDegree() override;
    float getZFar() override;

    gboolean init_shaders(Glib::Error &error) override;
    void init(Gtk::GLArea *glArea) override;
    void unrealize() override;
    void draw(Gtk::GLArea *glArea, Matrix &proj, Matrix &view) override;
    std::string customize_time(std::string prepared);
    Geometry *on_click_select(GdkEventButton* event, float mx, float my) override;
    Config* get_config() {
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
    void set_config_dialog(ConfigDialog* cfgdlg) {
        m_cfgdlg = cfgdlg;
    }
    std::shared_ptr<Weather> refresh_weather_service();
    void request_weather_product();
    // as we have no Gis, so limit the complexity of usable files
    static constexpr goffset GEO_FILE_SIZE_LIMIT{200*1024};

protected:
    void show_error(const std::string& msg);
    Gdk::EventMask getAddEventMask() override;
    double julianDate();
    void calcuateEarthLight();
    void color_to_alpha(Glib::RefPtr<Gdk::Pixbuf> pix);
    void calcuateMoonLight();
    gboolean init_moon_shaders(Glib::Error &error);
    gboolean init_earth_shaders(Glib::Error &error);
    double moonPhase();
private:
    Config *m_config;
    Rotational get_rotation();
    SphereContext *m_earthContext;
    TextContext *m_textContext;
    SphereGlArea *m_naviGlArea;
    Matrix m_fixView;
    Geometry *m_earth;
    Tex *m_dayTex;
    Tex *m_nightTex;
    Tex *m_normalMapTex;
    Tex *m_speculatMapTex;
    Tex *m_weatherTex;
    Font *m_font;
    sigc::connection m_timer;               // Timer for regular updates
    Vector m_light;
    float m_earth_declination_deg;
    void updateTimer();
    Glib::ustring hm(const double& timeM);
    void calcuateLight();
    gboolean view_update();
    Text *m_text;
    Geometry *debugGeom;
    TimezoneInfo *m_timezoneInfo;
    MarkContext *m_markContext;
    double m_sunRise;
    double m_sunSet;
    std::shared_ptr<Weather> m_weather;
    Glib::RefPtr<Gdk::Pixbuf> m_weather_pix;
    Geometry *geoJsonGeom;
    static constexpr float EARTH_RADIUS{30.0f};
    ConfigDialog *m_cfgdlg{nullptr};
    MoonContext *m_moonContext;
    Geometry *m_moon;
    Tex *m_moonTex;
    Vector m_moonLight;
    Matrix m_moonViewMat;
};

struct RESOURCE {
    static constexpr const char * const PREFIX = "/de/pfeifer_syscon/glsceneapp/";

    static std::string resource(const char *file);
};
