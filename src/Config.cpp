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

Config::Config()
: m_config{nullptr}
, m_lat{0}  // default to equator
, m_lon{0}  // default to greenwich
, m_ambient{0.67f}
, m_diffuse{700.0f}
, m_specular{700.0f}
, m_twilight{0.08}
, m_debug{0}
, m_distance{100.0f}
, m_specular_power{5.0f}
, m_dayTextureFile{""}
, m_nightTextureFile{""}
, m_timeFormat{"%c\\n%D"}
, m_panedPos{0}
, m_weatherProductId{""}
, m_weatherTransparency{1.0}
, m_geoJsonFile{""}
{
}


Config::~Config()
{
}


void
Config::read()
{
    m_config = new Glib::KeyFile();

    std::string cfg = get_config_name();
    try {
        if (!m_config->load_from_file(cfg, Glib::KEY_FILE_NONE)) {
            std::cerr << "Error loading " << cfg << std::endl;
        }
        else {
            if (m_config->has_group(GRP_MAIN)) {
                if (m_config->has_key(GRP_MAIN, LATITUDE))
                    m_lat = m_config->get_integer(GRP_MAIN, LATITUDE);
                if (m_config->has_key(GRP_MAIN, LONGITUDE))
                    m_lon = m_config->get_integer(GRP_MAIN, LONGITUDE);
                if (m_config->has_key(GRP_MAIN, DAYTEX))
                    m_dayTextureFile = m_config->get_string(GRP_MAIN, DAYTEX);
                if (m_config->has_key(GRP_MAIN, NIGHTTEX))
                    m_nightTextureFile = m_config->get_string(GRP_MAIN, NIGHTTEX);
                if (m_config->has_key(GRP_MAIN, AMBIENT))
                    m_ambient = m_config->get_double(GRP_MAIN, AMBIENT);
                if (m_config->has_key(GRP_MAIN, DIFFUSE))
                    m_diffuse = m_config->get_double(GRP_MAIN, DIFFUSE);
                if (m_config->has_key(GRP_MAIN, SPECULAR))
                    m_specular = m_config->get_double(GRP_MAIN, SPECULAR);
                if (m_config->has_key(GRP_MAIN, TWILIGHT))
                    m_twilight = m_config->get_double(GRP_MAIN, TWILIGHT);
                if (m_config->has_key(GRP_MAIN, DISTANCE))
                    m_distance = m_config->get_double(GRP_MAIN, DISTANCE);
                if (m_config->has_key(GRP_MAIN, SPECULAR_POWER))
                    m_specular_power = m_config->get_double(GRP_MAIN, SPECULAR_POWER);
                if (m_config->has_key(GRP_MAIN, TIME_FORMAT))
                    m_timeFormat = m_config->get_string(GRP_MAIN, TIME_FORMAT);
                if (m_config->has_key(GRP_MAIN, WEATHER_PRODUCT))
                    m_weatherProductId = m_config->get_string(GRP_MAIN, WEATHER_PRODUCT);
                if (m_config->has_key(GRP_MAIN, WEATHER_TRANSP))
                    m_weatherTransparency = m_config->get_double(GRP_MAIN, WEATHER_TRANSP);
                if (m_config->has_key(GRP_MAIN, GEO_JSON_FILE))
                    m_geoJsonFile = m_config->get_string(GRP_MAIN, GEO_JSON_FILE);
            }
            if (m_config->has_group(GRP_WIN)) {
                if (m_config->has_key(GRP_WIN, SPLIT_POS))
                    m_panedPos = m_config->get_integer(GRP_WIN, SPLIT_POS);
            }
        }
    }
    catch (const Glib::FileError& exc) {
        // may happen if didn't create a file (yet) but we can go on
        std::cerr << "File Error loading " << cfg << " if its missing, it will be created?" << std::endl;
    }
}

void
Config::save()
{
    if (m_config) {
        m_config->set_integer(GRP_MAIN, LATITUDE, m_lat);
        m_config->set_integer(GRP_MAIN, LONGITUDE, m_lon);
        m_config->set_string(GRP_MAIN, DAYTEX, m_dayTextureFile);
        m_config->set_string(GRP_MAIN, NIGHTTEX, m_nightTextureFile);
        m_config->set_double(GRP_MAIN, AMBIENT, m_ambient);
        m_config->set_double(GRP_MAIN, DIFFUSE, m_diffuse);
        m_config->set_double(GRP_MAIN, SPECULAR, m_specular);
        m_config->set_double(GRP_MAIN, TWILIGHT, m_twilight);
        m_config->set_double(GRP_MAIN, DISTANCE, m_distance);
        m_config->set_double(GRP_MAIN, SPECULAR_POWER, m_specular_power);
        m_config->set_string(GRP_MAIN, TIME_FORMAT, m_timeFormat);
        m_config->set_integer(GRP_WIN, SPLIT_POS, m_panedPos);
        m_config->set_string(GRP_MAIN, WEATHER_PRODUCT, m_weatherProductId);
        m_config->set_double(GRP_MAIN, WEATHER_TRANSP, m_weatherTransparency);
        m_config->set_string(GRP_MAIN, GEO_JSON_FILE, m_geoJsonFile);

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
    return m_lat;
}

void
Config::setLatitude(int lat)
{
    m_lat = lat;
}

int
Config::getLongitude()
{
    return m_lon;
}

void
Config::setLongitude(int lon)
{
    m_lon = lon;
}

std::string &
Config::getDayTextureFile()
{
    return m_dayTextureFile;
}

void
Config::setDayTextureFile(std::string dayTex)
{
    m_dayTextureFile = dayTex;
}

std::string &
Config::getNightTextureFile()
{
    return m_nightTextureFile;
}

void
Config::setNightTexureFile(std::string nightTex)
{
    m_nightTextureFile = nightTex;
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
    return m_ambient;
}

void
Config::setAmbient(float ambient)
{
    m_ambient = ambient;
}

float
Config::getDiffuse()
{
    return m_diffuse;
}

void
Config::setDiffuse(float diffuse)
{
    m_diffuse = diffuse;
}

float
Config::getSpecular()
{
    return m_specular;
}

void
Config::setSpecular(float specular)
{
    m_specular = specular;
}

float
Config::getTwilight()
{
    return m_twilight;
}

void
Config::setTwilight(float twilight)
{
    m_twilight = twilight;
}

float
Config::getSpecularPower()
{
    return m_specular_power;
}

void
Config::setSpecularPower(float specular_power)
{
    m_specular_power = specular_power;
}

float
Config::getDistance()
{
     return m_distance;
}

void
Config::setDistance(float dist)
{
    m_distance = dist;
}

const std::string &
Config::getTimeFormat()
{
    return m_timeFormat;
}

void
Config::setTimeFormat(std::string tmFormat)
{
    m_timeFormat = tmFormat;
}

int
Config::getPanedPos()
{
    return m_panedPos;
}

void
Config::setPanedPos(int panedPos)
{
    m_panedPos = panedPos;
}

void Config::setWeatherProductId(const std::string& weatherProductId)
{
    m_weatherProductId = weatherProductId;
}

std::string Config::getWeatherProductId()
{
    return m_weatherProductId;
}

void
Config::setWeatherTransparency(double transp)
{
    m_weatherTransparency = transp;
}

double
Config::getWeatherTransparency()
{
    return m_weatherTransparency;
}

void
Config::setGeoJsonFile(const Glib::ustring& geoJsonFile)
{
    m_geoJsonFile = geoJsonFile;
}


Glib::ustring
Config::getGeoJsonFile()
{
    return m_geoJsonFile;
}
