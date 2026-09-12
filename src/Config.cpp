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

std::string
Config::get_config_name()
{
    std::string fullPath = g_canonicalize_filename("glglobe.conf", Glib::get_user_config_dir().c_str());
    //std::cout << "using config " << fullPath << std::endl;
    return fullPath;
}

std::string
Config::get_main_config_group()
{
    return GRP_MAIN;
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
