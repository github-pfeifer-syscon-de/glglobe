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

#include <iostream>
#include <psc_format.hpp>
#include <Log.hpp>
#include <StringUtils.hpp>
#include <Weather.hpp>
#include <RealEarth.hpp>
#include <WebMapService.hpp>
#include <cmath>

#include "Config.hpp"

void
Config::read()
{
    m_config = new Glib::KeyFile();
    std::string cfg = get_config_name();
    auto file = Gio::File::create_for_path(cfg);
    if (file->query_exists()) {  // it is not a error, if the file doesn't exists
        try {
            if (!m_config->load_from_file(cfg, Glib::KEY_FILE_NONE)) {
                std::cerr << "Error loading " << cfg << std::endl;
            }
        }
        catch (const Glib::FileError& exc) {
            Glib::ustring msg{psc::fmt::format("Error {} loading config {}", exc.what(), cfg)};
            psc::log::Log::logAdd(psc::log::Level::Error, msg);
        }
    }
    if (!m_config->has_group(GRP_MAIN)) {   // create group
        m_config->set_string(GRP_MAIN, LOG_LEVEL, DEFAULT_LOG_LEVEL);
    }
    for (uint32_t i = 0; i < MAX_WEATHER_SERVICES; ++i) {
        migrateWeatherServices(i);
        std::shared_ptr<WebMapServiceConf> weatherService;
        auto weatherGrp = Glib::ustring::sprintf("%s%d", GRP_WEATHER, i);
        if (m_config->has_group(weatherGrp)
         && m_config->has_key(weatherGrp, WEATHER_SERVICE_ADDRESS)
         && m_config->has_key(weatherGrp, WEATHER_SERVICE_NAME)) {
            auto weatherAddress = m_config->get_string(weatherGrp, WEATHER_SERVICE_ADDRESS);
            auto weatherName = m_config->get_string(weatherGrp, WEATHER_SERVICE_NAME);
            int delay_sec = DEF_UPDATE_DELAY_SEC;
            if (m_config->has_key(weatherGrp, WEATHER_SERVICE_DELAY)) {
                delay_sec = m_config->get_integer(weatherGrp, WEATHER_SERVICE_DELAY);
                if (delay_sec < MIN_UPDATE_DELAY_SEC) {
                    delay_sec = MIN_UPDATE_DELAY_SEC;
                }
            }
            Glib::ustring weatherType{WEATHER_WMS_CONF};
            if (m_config->has_key(weatherGrp, WEATHER_SERVICE_TYPE)) {
                weatherType = m_config->get_string(weatherGrp, WEATHER_SERVICE_TYPE);
            }
            bool viewCurrentTime{false};
            if (m_config->has_key(weatherGrp, WEATHER_SERVICE_LOCAL_TIME)) {
                viewCurrentTime = m_config->get_boolean(weatherGrp, WEATHER_SERVICE_LOCAL_TIME);
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

// the decision to put all config into the main-grp was questionable,
//   so now move them into separate groups
void
Config::migrateWeatherServices(uint32_t i)
{
    auto addressKey = Glib::ustring::sprintf("%s%d", WEATHER_SERVICE_ADDRESS, i);
    auto nameKey = Glib::ustring::sprintf("%s%d", WEATHER_SERVICE_NAME, i);
    if (m_config->has_key(GRP_MAIN, addressKey)
     && m_config->has_key(GRP_MAIN, nameKey)) {
        auto weatherGrp = Glib::ustring::sprintf("%s%d", GRP_WEATHER, i);
        auto weatherAddress = m_config->get_string(GRP_MAIN, addressKey);
        m_config->remove_key(GRP_MAIN, addressKey);
        m_config->set_string(weatherGrp, WEATHER_SERVICE_ADDRESS, weatherAddress);
        auto weatherName = m_config->get_string(GRP_MAIN, nameKey);
        m_config->remove_key(GRP_MAIN, nameKey);
        m_config->set_string(weatherGrp, WEATHER_SERVICE_NAME, weatherName);
        auto delayKey = Glib::ustring::sprintf("%s%d", WEATHER_SERVICE_DELAY, i);
        if (m_config->has_key(GRP_MAIN, delayKey)) {
            int delay_sec = m_config->get_integer(GRP_MAIN, delayKey);
            m_config->remove_key(GRP_MAIN, delayKey);
            m_config->set_integer(weatherGrp, WEATHER_SERVICE_DELAY, delay_sec);
        }
        auto typeKey = Glib::ustring::sprintf("%s%d", WEATHER_SERVICE_TYPE, i);
        if (m_config->has_key(GRP_MAIN, typeKey)) {
            Glib::ustring  weatherType = m_config->get_string(GRP_MAIN, typeKey);
            m_config->remove_key(GRP_MAIN, typeKey);
            m_config->set_string(weatherGrp, WEATHER_SERVICE_TYPE, weatherType);
        }
        auto localTimeKey = Glib::ustring::sprintf("%s%d", WEATHER_SERVICE_LOCAL_TIME, i);
        if (m_config->has_key(GRP_MAIN, localTimeKey)) {
            bool viewCurrentTime = m_config->get_boolean(GRP_MAIN, localTimeKey);
            m_config->remove_key(GRP_MAIN, localTimeKey);
            m_config->set_boolean(weatherGrp, WEATHER_SERVICE_LOCAL_TIME, viewCurrentTime);
        }
    }
}

bool
Config::save()
{
    bool ret{false};
    if (m_config) {
        if (!m_config->has_group(GRP_MAIN)) {   // create group
            m_config->set_string(GRP_MAIN, LOG_LEVEL, DEFAULT_LOG_LEVEL);
        }
        if (m_weatherImageSize > 0) {
            m_config->set_integer(GRP_MAIN, WEATHER_IMAGE_SIZE, m_weatherImageSize);
        }
        for (uint32_t i = 0; i < m_weatherServices.size(); ++i) {
            auto weatherService = m_weatherServices[i];
            if (weatherService) {
                auto weatherGrp = Glib::ustring::sprintf("%s%d", GRP_WEATHER, i);
                m_config->set_string(weatherGrp, WEATHER_SERVICE_ADDRESS, weatherService->getAddress());
                m_config->set_string(weatherGrp, WEATHER_SERVICE_NAME, weatherService->getName());
                m_config->set_integer(weatherGrp, WEATHER_SERVICE_DELAY, weatherService->getDelaySec());
                m_config->set_string(weatherGrp, WEATHER_SERVICE_TYPE, weatherService->getType());
                m_config->set_boolean(weatherGrp, WEATHER_SERVICE_LOCAL_TIME, weatherService->isViewCurrentTime());
            }
        }
        auto cfg = get_config_name();
        try {
            ret = m_config->save_to_file(cfg);
        }
        catch (const Glib::FileError& exc) {
            Glib::ustring msg{psc::fmt::format("Error {} saving config {}", exc.what(), cfg)};
            psc::log::Log::logAdd(psc::log::Level::Error, msg);
        }
    }
    return ret;
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

Glib::RefPtr<Gio::File>
Config::getTimezoneDir()
{
    Glib::RefPtr<Gio::File> tzDirFile;
    if (m_config->has_key(GRP_MAIN, TIMEZONE_DIR)) {
        auto tzDir = m_config->get_string(GRP_MAIN, TIMEZONE_DIR);
        tzDirFile = Gio::File::create_for_path(tzDir);
    }
    return tzDirFile;
}

void
Config::setTimezoneDir(const Glib::RefPtr<Gio::File>& tzDir)
{
    m_config->set_string(GRP_MAIN, TIMEZONE_DIR, tzDir->get_path());
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

// as the vector is copied on return, need this to add entries
std::shared_ptr<WebMapServiceConf>
Config::addWebMapService(const Glib::ustring& newName)
{
    auto service = std::make_shared<WebMapServiceConf>(newName, "", 0, "", false);
    m_weatherServices.push_back(service);
    return service;
}


int
Config::getWeatherMinPeriodSec()
{
    int waether_min_period_sec{5*SECS_PER_MINUTE};
    if (m_config->has_key(GRP_MAIN, WEATHER_MIN_PERIOD_SECONDS))
        waether_min_period_sec = m_config->get_integer(GRP_MAIN, WEATHER_MIN_PERIOD_SECONDS);
    if (waether_min_period_sec < SECS_PER_MINUTE) {
        waether_min_period_sec = SECS_PER_MINUTE;
    }
    if (waether_min_period_sec > SECS_PER_DAY) {
        waether_min_period_sec = SECS_PER_DAY;
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

int
Config::getWeatherImageSize()
{
    if (m_weatherImageSize > 0) {   // only do computation once
        return m_weatherImageSize;
    }
    if (m_config->has_key(GRP_MAIN, WEATHER_IMAGE_SIZE)) {
        m_weatherImageSize = m_config->get_integer(GRP_MAIN, WEATHER_IMAGE_SIZE);
        // check if it is power of two, as it will be used as OpenGL texture
        double pow2 = std::log2(m_weatherImageSize);
        double integral;
        double rem = std::modf(pow2, &integral);
        if (rem > 0.0001) {
            uint32_t shift = static_cast<uint32_t>(integral);
            m_weatherImageSize = 1u << shift;
        }
        // limit to useful range
        m_weatherImageSize = std::max(std::min(m_weatherImageSize, MAX_WEATHER_IMAGE_SIZE), MIN_WEATHER_IMAGE_SIZE);
    }
    else {
        m_weatherImageSize = DEFAULT_WEATHER_IMAGE_SIZE;
    }
    return m_weatherImageSize;
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

std::shared_ptr<Weather>
Config::getService(WeatherConsumer* consumer,const std::shared_ptr<WebMapServiceConf>& serviceConf)
{
    Glib::ustring typeStr = serviceConf->getType();
    if (typeStr == Config::WEATHER_REAL_EARTH_CONF) {
        return std::make_shared<RealEarth>(consumer, serviceConf->getAddress());
    }
    else if (typeStr == Config::WEATHER_WMS_CONF) {
        return std::make_shared<WebMapService>(consumer, serviceConf, getWeatherMinPeriodSec());
    }
    else {
        psc::log::Log::logAdd(psc::log::Level::Warn, [&] {
            return psc::fmt::format("refresh serviceId typeStr {}", typeStr);
        });
    }
    return std::shared_ptr<Weather>();
}