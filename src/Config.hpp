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

#include <vector>
#include <glibmm.h>
#include <Weather.hpp>


class Config {
public:
    Config() = default;
    virtual ~Config() = default;

    void read();
    void save();

    std::string getDayTextureFile();
    void setDayTextureFile(std::string dayTex);
    std::string getNightTextureFile();
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
    const std::string getTimeFormat();
    void setTimeFormat(const std::string& tmFormat);
    void setWeatherProductId(const std::string& weatherProduct);
    std::string getWeatherProductId();
    void setWeatherServiceId(const std::string& weatherServiceId);
    std::string getWeatherServiceId();
    std::shared_ptr<WebMapServiceConf> getActiveWebMapServiceConf();
    void setWeatherTransparency(double transp);
    double getWeatherTransparency();
    void setGeoJsonFile(const Glib::ustring& geoJsonFile);
    Glib::ustring getGeoJsonFile();
    std::vector<std::shared_ptr<WebMapServiceConf>> getWebMapServices();
    int getWeatherMinPeriodSec();
    void setWeatherMinPeriodSec(uint32_t sec);
    Glib::ustring getTimerValue();
    void setTimerValue(const Glib::ustring& timer);
    Glib::ustring getTimeValue();
    void setTimeValue(const Glib::ustring& time);
    Glib::ustring getLogLevel();

    static constexpr auto WEATHER_REAL_EARTH_CONF{"RE"};
    static constexpr auto WEATHER_WMS_CONF{"WMS"};

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
    static constexpr auto WEATHER_SERVICE{"weatherService"};
    static constexpr auto WEATHER_PRODUCT{"weatherProduct"};
    static constexpr auto WEATHER_TRANSP{"weatherTransparency"};
    static constexpr auto WEATHER_MIN_PERIOD_SECONDS{"minWeatherPeriodSeconds"};
    static constexpr auto WEATHER_SERVICE_NAME{"weatherName"};
    static constexpr auto WEATHER_SERVICE_ADDRESS{"weatherAddress"};
    static constexpr auto WEATHER_SERVICE_DELAY{"weatherDelay"};
    static constexpr auto WEATHER_SERVICE_TYPE{"weatherType"};
    static constexpr auto WEATHER_SERVICE_LOCAL_TIME{"weatherLocalTime"};
    static constexpr auto GEO_JSON_FILE{"geoJsonFile"};
    static constexpr auto GRP_TIME{"time"};
    static constexpr auto TIMER_VALUE{"timerValue"};
    static constexpr auto TIME_VALUE{"timeValue"};
    static constexpr auto LOG_LEVEL{"logLevel"};
    static constexpr auto DEFAULT_LOG_LEVEL{"Info"};

    static constexpr auto MIN_UPDATE_DELAY_SEC{5 * 60};
    static constexpr auto DEF_UPDATE_DELAY_SEC{30 * 60};
private:
    Glib::KeyFile *m_config{nullptr};
    unsigned int m_debug{0};
    std::vector<std::shared_ptr<WebMapServiceConf>> m_weatherServices;

};
