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

#include <glibmm.h>


class Config {
public:
    Config();
    virtual ~Config();

    void read();
    void save();

    std::string &getDayTextureFile();
    void setDayTextureFile(std::string dayTex);
    std::string &getNightTextureFile();
    void setNightTexureFile(std::string nightText);
    int getLatitude();
    void setLatitude(int lat);
    int getLongitude();
    void setLongitude(int lon);
    float getAmbient();
    void setAmbient(float ambient);
    float getDiffuse();
    void setDiffuse(float diffuse);
    float getSpecular();
    void setSpecular(float spec);
    float getSpecularPower();
    void setSpecularPower(float specPow);
    float getTwilight();
    void setTwilight(float twil);
    float getDistance();
    void setDistance(float dist);
    unsigned int getDebug();
    void setDebug(unsigned int  debug);
    const std::string &getTimeFormat();
    void setTimeFormat(std::string tmFormat);
    int getPanedPos();
    void setPanedPos(int panedPos);
    void setWeatherProductId(const std::string& weatherProduct);
    std::string getWeatherProductId();
    void setWeatherTransparency(double transp);
    double getWeatherTransparency();
    void setGeoJsonFile(const Glib::ustring& geoJsonFile);
    Glib::ustring getGeoJsonFile();
protected:
    std::string get_config_name();

    static constexpr const char * const GRP_MAIN = "globe";
    static constexpr const char * const LATITUDE = "lat";
    static constexpr const char * const LONGITUDE = "lon";
    static constexpr const char * const DAYTEX = "dayTex";
    static constexpr const char * const NIGHTTEX = "nightTex";
    static constexpr const char * const AMBIENT = "ambient";
    static constexpr const char * const DIFFUSE = "diffuse";
    static constexpr const char * const SPECULAR = "specular";
    static constexpr const char * const TWILIGHT = "twilight";
    static constexpr const char * const DISTANCE = "distance";
    static constexpr const char * const SPECULAR_POWER = "specularPower";
    static constexpr const char * const TIME_FORMAT = "timeFormat";
    static constexpr const char * const GRP_WIN = "win";
    static constexpr const char * const SPLIT_POS = "splitPos";
    static constexpr const char * const WEATHER_PRODUCT = "weatherProduct";
    static constexpr const char * const WEATHER_TRANSP = "weatherTransparency";
    static constexpr const char * const GEO_JSON_FILE = "geoJsonFile";
private:
    Glib::KeyFile *m_config;
    int m_lat;
    int m_lon;
    float m_ambient;
    float m_diffuse;
    float m_specular;
    float m_twilight;
    unsigned int m_debug;
    float m_distance;
    float m_specular_power;
    std::string m_dayTextureFile;
    std::string m_nightTextureFile;
    std::string m_timeFormat;
    int m_panedPos;
    std::string m_weatherProductId;
    double m_weatherTransparency;
    Glib::ustring m_geoJsonFile;
};
