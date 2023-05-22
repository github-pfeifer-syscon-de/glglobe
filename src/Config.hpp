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

class WebMapServiceConf {
public:
    WebMapServiceConf(const Glib::ustring& name, const Glib::ustring& address, int delay_sec, const Glib::ustring& type, bool localTime);
    explicit WebMapServiceConf(const WebMapServiceConf& oth) = delete;
    virtual ~WebMapServiceConf() = default;

    Glib::ustring getName() const
    {
        return m_name;
    }
    Glib::ustring getAddress() const
    {
        return m_address;
    }
    // beside the document period see WMS doc (the interval between updates)
    //   there is a delay (the place when this becomes visible is with the time
    //     dimension the latest values always is some minutes behind the actual time).
    //   e.g. the precipitation is announced with a interval P which presumably means ask any time, we will give you the nearest value.
    //     but if you try to ask for now there is a error when requesting the images,
    //     some fiddeling suggested the use of a 30 minutes delay
    //     and the use of a minimum interval of 5 minutes.
    int getDelaySec() const
    {
        return m_delay_sec;
    }
    Glib::ustring getType() const
    {
        return m_type;
    }
    // some server report local time
    bool isLocalTime() const
    {
        return m_localTime;
    }
private:
    Glib::ustring m_name;
    Glib::ustring m_address;
    int m_delay_sec;
    Glib::ustring m_type;
    bool m_localTime;
};

class Config {
public:
    Config() = default;
    virtual ~Config() = default;

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
    uint32_t getWeatherMinPeriodSec();
    void setWeatherMinPeriodSec(uint32_t sec);

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

    static constexpr auto MIN_UPDATE_DELAY_SEC{5 * 60};
    static constexpr auto DEF_UPDATE_DELAY_SEC{30 * 60};
private:
    Glib::KeyFile *m_config{nullptr};
    int m_lat{0};   // default to equator
    int m_lon{0};   // default to greenwich
    float m_ambient{0.67f};
    float m_diffuse{700.0f};
    float m_specular{700.0f};
    float m_twilight{0.08};
    unsigned int m_debug{0};
    float m_distance{100.0f};
    float m_specular_power{5.0f};
    std::string m_dayTextureFile;
    std::string m_nightTextureFile;
    std::string m_timeFormat{"%c\\n%D"};
    std::string m_weatherServiceId;
    std::string m_weatherProductId;
    double m_weatherTransparency{1.0};
    Glib::ustring m_geoJsonFile;
    std::vector<std::shared_ptr<WebMapServiceConf>> m_weatherServices;
    uint32_t m_waether_min_period_sec{5*60};
};
