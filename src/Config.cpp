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

#include <iostream>

#include "Config.hpp"

void
Config::read()
{
    m_config = new Glib::KeyFile();
    std::string cfg = get_config_name();
    try {
        if (!m_config->load_from_file(cfg, Glib::KEY_FILE_NONE)) {
            std::cerr << "Error loading " << cfg << std::endl;
        }
    }
    catch (const Glib::FileError& exc) {
        // may happen if didn't create a file (yet) but we can go on
        std::cerr << "File Error loading " << cfg << " if its missing, it will be created?" << std::endl;
    }
    if (!m_config->has_group(GRP_MAIN)) {   // create group
        m_config->set_string(GRP_MAIN, LOG_LEVEL, DEFAULT_LOG_LEVEL);
    }
    for (uint32_t i = 0; i < 10; ++i) {
        std::shared_ptr<WebMapServiceConf> weatherService;
        auto addressKey = Glib::ustring::sprintf("%s%d", WEATHER_SERVICE_ADDRESS, i);
        auto nameKey = Glib::ustring::sprintf("%s%d", WEATHER_SERVICE_NAME, i);
        if (m_config->has_key(GRP_MAIN, addressKey)
         && m_config->has_key(GRP_MAIN, nameKey)) {
            auto weatherAddress = m_config->get_string(GRP_MAIN, addressKey);
            auto weatherName = m_config->get_string(GRP_MAIN, nameKey);
            auto delayKey = Glib::ustring::sprintf("%s%d", WEATHER_SERVICE_DELAY, i);
            int delay_sec = DEF_UPDATE_DELAY_SEC;
            if (m_config->has_key(GRP_MAIN, delayKey)) {
                delay_sec = m_config->get_integer(GRP_MAIN, delayKey);
                if (delay_sec < MIN_UPDATE_DELAY_SEC) {
                    delay_sec = MIN_UPDATE_DELAY_SEC;
                }
            }
            Glib::ustring weatherType{WEATHER_WMS_CONF};
            auto typeKey = Glib::ustring::sprintf("%s%d", WEATHER_SERVICE_TYPE, i);
            if (m_config->has_key(GRP_MAIN, delayKey)) {
                weatherType = m_config->get_string(GRP_MAIN, typeKey);
            }
            auto localTimeKey = Glib::ustring::sprintf("%s%d", WEATHER_SERVICE_LOCAL_TIME, i);
            bool viewCurrentTime{false};
            if (m_config->has_key(GRP_MAIN, localTimeKey)) {
                viewCurrentTime = m_config->get_boolean(GRP_MAIN, localTimeKey);
            }
            weatherService = std::make_shared<WebMapServiceConf>(weatherName, weatherAddress, delay_sec, weatherType, viewCurrentTime);
        }
        else {
            switch (i) {    // prepare some defaults
            case 0:
                weatherService = std::make_shared<WebMapServiceConf>("RealEarth", "https://realearth.ssec.wisc.edu/", MIN_UPDATE_DELAY_SEC, WEATHER_REAL_EARTH_CONF, false);
                break;
            case 1:
                weatherService = std::make_shared<WebMapServiceConf>("EumetSat", "https://view.eumetsat.int/geoserver/wms", DEF_UPDATE_DELAY_SEC, WEATHER_WMS_CONF, false);
                break;
            case 2:
                weatherService = std::make_shared<WebMapServiceConf>("DeutscherWetterDienst", "https://maps.dwd.de/geoserver/ows", MIN_UPDATE_DELAY_SEC, WEATHER_WMS_CONF, true);
                break;
            }
        }
        if (weatherService) {
            m_weatherServices.push_back(weatherService);
        }
    }

}

void
Config::save()
{
    if (m_config) {
        if (!m_config->has_group(GRP_MAIN)) {   // create group
            m_config->set_string(GRP_MAIN, LOG_LEVEL, DEFAULT_LOG_LEVEL);
        }
        for (uint32_t i = 0; i < m_weatherServices.size(); ++i) {
            auto weatherService = m_weatherServices[i];
            if (weatherService) {
                auto addressKey = Glib::ustring::sprintf("%s%d", WEATHER_SERVICE_ADDRESS, i);
                m_config->set_string(GRP_MAIN, addressKey, weatherService->getAddress());
                auto nameKey = Glib::ustring::sprintf("%s%d", WEATHER_SERVICE_NAME, i);
                m_config->set_string(GRP_MAIN, nameKey, weatherService->getName());
                auto delayKey = Glib::ustring::sprintf("%s%d", WEATHER_SERVICE_DELAY, i);
                m_config->set_integer(GRP_MAIN, delayKey, weatherService->getDelaySec());
                auto typeKey = Glib::ustring::sprintf("%s%d", WEATHER_SERVICE_TYPE, i);
                m_config->set_string(GRP_MAIN, typeKey, weatherService->getType());
                auto localTimeKey = Glib::ustring::sprintf("%s%d", WEATHER_SERVICE_LOCAL_TIME, i);
                m_config->set_boolean(GRP_MAIN, localTimeKey, weatherService->isViewCurrentTime());
            }
        }
        std::string cfg = get_config_name();
        if (!m_config->save_to_file(cfg)) {
             std::cerr << "Error saving " << cfg << std::endl;
        }
    }
}

std::string
Config::get_config_name()
{
    std::string fullPath = g_canonicalize_filename("glglobe.conf", Glib::get_user_config_dir().c_str());
    //std::cout << "using config " << fullPath << std::endl;
    return fullPath;
}

int
Config::getLatitude()
{
    int lat{0};
    if (m_config->has_key(GRP_MAIN, LATITUDE))
        lat = m_config->get_integer(GRP_MAIN, LATITUDE);
    return lat;
}

void
Config::setLatitude(int lat)
{
    m_config->set_integer(GRP_MAIN, LATITUDE, lat);
}

int
Config::getLongitude()
{
    int lon{0};
    if (m_config->has_key(GRP_MAIN, LONGITUDE))
        lon = m_config->get_integer(GRP_MAIN, LONGITUDE);
    return lon;
}

void
Config::setLongitude(int lon)
{
    m_config->set_integer(GRP_MAIN, LONGITUDE, lon);
}

std::string
Config::getDayTextureFile()
{
    std::string dayTextureFile;
    if (m_config->has_key(GRP_MAIN, DAYTEX))
        dayTextureFile = m_config->get_string(GRP_MAIN, DAYTEX);
    return dayTextureFile;
}

void
Config::setDayTextureFile(std::string dayTextureFile)
{
    m_config->set_string(GRP_MAIN, DAYTEX, dayTextureFile);
}

std::string
Config::getNightTextureFile()
{
    std::string nightTextureFile;
    if (m_config->has_key(GRP_MAIN, NIGHTTEX))
        nightTextureFile = m_config->get_string(GRP_MAIN, NIGHTTEX);
    return nightTextureFile;
}

void
Config::setNightTexureFile(std::string nightTextureFile)
{
    m_config->set_string(GRP_MAIN, NIGHTTEX, nightTextureFile);
}


unsigned int
Config::getDebug()
{
    return m_debug;
}

void
Config::setDebug(unsigned int debug)
{
    m_debug = debug;
}

float
Config::getAmbient()
{
    float ambient{0.67f};
    if (m_config->has_key(GRP_MAIN, AMBIENT))
        ambient = static_cast<float>(m_config->get_double(GRP_MAIN, AMBIENT));
    return ambient;
}

void
Config::setAmbient(float ambient)
{
    m_config->set_double(GRP_MAIN, AMBIENT, ambient);
}

float
Config::getDiffuse()
{
    float diffuse{700.0f};
    if (m_config->has_key(GRP_MAIN, DIFFUSE))
        diffuse = static_cast<float>(m_config->get_double(GRP_MAIN, DIFFUSE));
    return diffuse;
}

void
Config::setDiffuse(float diffuse)
{
    m_config->set_double(GRP_MAIN, DIFFUSE, diffuse);
}

float
Config::getSpecular()
{
    float specular{700.0f};
    if (m_config->has_key(GRP_MAIN, SPECULAR))
        specular = static_cast<float>(m_config->get_double(GRP_MAIN, SPECULAR));

    return specular;
}

void
Config::setSpecular(float specular)
{
    m_config->set_double(GRP_MAIN, SPECULAR, specular);
}

float
Config::getTwilight()
{
    float twilight{0.08f};
    if (m_config->has_key(GRP_MAIN, TWILIGHT))
        twilight = static_cast<float>(m_config->get_double(GRP_MAIN, TWILIGHT));
    return twilight;
}

void
Config::setTwilight(float twilight)
{
    m_config->set_double(GRP_MAIN, TWILIGHT, twilight);
}

float
Config::getSpecularPower()
{
    float specular_power{5.0f};
    if (m_config->has_key(GRP_MAIN, SPECULAR_POWER))
        specular_power = static_cast<float>(m_config->get_double(GRP_MAIN, SPECULAR_POWER));
    return specular_power;
}

void
Config::setSpecularPower(float specular_power)
{
    m_config->set_double(GRP_MAIN, SPECULAR_POWER, specular_power);
}

float
Config::getDistance()
{
    float distance{100.0f};
    if (m_config->has_key(GRP_MAIN, DISTANCE))
        distance = static_cast<float>(m_config->get_double(GRP_MAIN, DISTANCE));
     return distance;
}

void
Config::setDistance(float distance)
{
    m_config->set_double(GRP_MAIN, DISTANCE, distance);
}

const std::string
Config::getTimeFormat()
{
    std::string timeFormat{"%c\\n%D"};
    if (m_config->has_key(GRP_MAIN, TIME_FORMAT))
        timeFormat = m_config->get_string(GRP_MAIN, TIME_FORMAT);
    return timeFormat;
}

void
Config::setTimeFormat(const std::string& timeFormat)
{
    m_config->set_string(GRP_MAIN, TIME_FORMAT, timeFormat);
}

void
Config::setWeatherProductId(const std::string& weatherProductId)
{
    m_config->set_string(GRP_MAIN, WEATHER_PRODUCT, weatherProductId);
}

std::string
Config::getWeatherProductId()
{
    std::string weatherProductId;
    if (m_config->has_key(GRP_MAIN, WEATHER_PRODUCT))
        weatherProductId = m_config->get_string(GRP_MAIN, WEATHER_PRODUCT);
    return weatherProductId;
}

void
Config::setWeatherServiceId(const std::string& weatherServiceId)
{
    m_config->set_string(GRP_MAIN, WEATHER_SERVICE, weatherServiceId);
}

std::string
Config::getWeatherServiceId()
{
    std::string weatherServiceId;
    if (m_config->has_key(GRP_MAIN, WEATHER_SERVICE))
        weatherServiceId = m_config->get_string(GRP_MAIN, WEATHER_SERVICE);
    return weatherServiceId;
}

void
Config::setWeatherTransparency(double transp)
{
    m_config->set_double(GRP_MAIN, WEATHER_TRANSP, transp);
}

double
Config::getWeatherTransparency()
{
    double weatherTransparency{1.0};
    if (m_config->has_key(GRP_MAIN, WEATHER_TRANSP))
        weatherTransparency = m_config->get_double(GRP_MAIN, WEATHER_TRANSP);
    return weatherTransparency;
}

void
Config::setGeoJsonFile(const Glib::ustring& geoJsonFile)
{
    m_config->set_string(GRP_MAIN, GEO_JSON_FILE, geoJsonFile);
}


Glib::ustring
Config::getGeoJsonFile()
{
    Glib::ustring geoJsonFile;
    if (m_config->has_key(GRP_MAIN, GEO_JSON_FILE))
        geoJsonFile = m_config->get_string(GRP_MAIN, GEO_JSON_FILE);
    return geoJsonFile;
}

std::vector<std::shared_ptr<WebMapServiceConf>>
Config::getWebMapServices()
{
    return m_weatherServices;
}

int
Config::getWeatherMinPeriodSec()
{
    int waether_min_period_sec{5*60};
    if (m_config->has_key(GRP_MAIN, WEATHER_MIN_PERIOD_SECONDS))
        waether_min_period_sec = m_config->get_integer(GRP_MAIN, WEATHER_MIN_PERIOD_SECONDS);
    if (waether_min_period_sec < 60) {
        waether_min_period_sec = 60;
    }
    if (waether_min_period_sec > 24 * 60 * 60) {
        waether_min_period_sec = 24 * 60 * 60;
    }
    return waether_min_period_sec;
}

void
Config::setWeatherMinPeriodSec(uint32_t sec)
{
    m_config->set_integer(GRP_MAIN, WEATHER_MIN_PERIOD_SECONDS, sec);
}

Glib::ustring
Config::getTimerValue()
{
    Glib::ustring timerValue;
    if (m_config->has_group(GRP_TIME)
     && m_config->has_key(GRP_TIME, TIMER_VALUE))
        timerValue = m_config->get_string(GRP_TIME, TIMER_VALUE);
    return timerValue;
}

void
Config::setTimerValue(const Glib::ustring& timer)
{
    m_config->set_string(GRP_TIME, TIMER_VALUE, timer);
}

Glib::ustring
Config::getTimeValue()
{
    Glib::ustring timeValue;
    if (m_config->has_group(GRP_TIME)
     && m_config->has_key(GRP_TIME, TIME_VALUE))
        timeValue = m_config->get_string(GRP_TIME, TIME_VALUE);
    return timeValue;
}

void
Config::setTimeValue(const Glib::ustring& time)
{
    m_config->set_string(GRP_TIME, TIME_VALUE, time);
}

Glib::ustring
Config::getLogLevel()
{
    Glib::ustring logLevel{DEFAULT_LOG_LEVEL};
    if (m_config->has_key(GRP_MAIN, LOG_LEVEL))
        logLevel = m_config->get_string(GRP_MAIN, LOG_LEVEL);
    return logLevel;
}

std::shared_ptr<WebMapServiceConf>
Config::getActiveWebMapServiceConf()
{
    std::shared_ptr<WebMapServiceConf> conf;
    auto name = getWeatherServiceId();
    if (!name.empty()) {
        for (auto cnf : getWebMapServices()) {
            if (cnf->getName() == name) {
                conf = cnf;
                break;
            }
        }
    }
    return conf;
}