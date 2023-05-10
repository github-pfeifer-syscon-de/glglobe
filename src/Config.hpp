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
    void setWeatherServiceId(const std::string& weatherServiceId);
    std::string getWeatherServiceId();
    void setWeatherTransparency(double transp);
    double getWeatherTransparency();
    void setGeoJsonFile(const Glib::ustring& geoJsonFile);
    Glib::ustring getGeoJsonFile();
protected:
    std::string get_config_name();

    static constexpr auto GRP_MAIN{"globe"};
    static constexpr auto LATITUDE{"lat"};
    static constexpr auto LONGITUDE{"lon"};
    static constexpr auto DAYTEX{"dayTex"};
    static constexpr auto NIGHTTEX{"nightTex"};
    static constexpr auto AMBIENT{"ambient"};
    static constexpr auto DIFFUSE{"diffuse"};
    static constexpr auto SPECULAR{"specular"};
    static constexpr auto TWILIGHT{"twilight"};
    static constexpr auto DISTANCE{"distance"};
    static constexpr auto SPECULAR_POWER{"specularPower"};
    static constexpr auto TIME_FORMAT{"timeFormat"};
    static constexpr auto GRP_WIN{"win"};
    static constexpr auto SPLIT_POS{"splitPos"};
    static constexpr auto WEATHER_SERVICE{"weatherService"};
    static constexpr auto WEATHER_PRODUCT{"weatherProduct"};
    static constexpr auto WEATHER_TRANSP{"weatherTransparency"};
    static constexpr auto GEO_JSON_FILE{"geoJsonFile"};
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
    std::string m_weatherServiceId;
    std::string m_weatherProductId;
    double m_weatherTransparency;
    Glib::ustring m_geoJsonFile;
};
