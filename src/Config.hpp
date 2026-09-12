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

#include <vector>
#include <glibmm.h>
#include <Weather.hpp>
#include <WeatherConfig.hpp>


class Config
: public WeatherConfig
{
public:
    Config() = default;
    virtual ~Config() = default;


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
    const std::string getTimeFormat();
    void setTimeFormat(const std::string& tmFormat);
    void setGeoJsonFile(const Glib::ustring& geoJsonFile);
    Glib::ustring getGeoJsonFile();
    Glib::ustring getTimerValue();
    void setTimerValue(const Glib::ustring& timer);
    Glib::ustring getTimeValue();
    void setTimeValue(const Glib::ustring& time);
    Glib::RefPtr<Gio::File> getTimezoneDir();
    void setTimezoneDir(const Glib::RefPtr<Gio::File>& tzDir);
protected:
    std::string get_config_name() override;
    std::string get_main_config_group() override;
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
    static constexpr auto GEO_JSON_FILE{"geoJsonFile"};
    static constexpr auto GRP_TIME{"time"};
    static constexpr auto TIMER_VALUE{"timerValue"};
    static constexpr auto TIME_VALUE{"timeValue"};
    static constexpr auto TIMEZONE_DIR{"timezoneDir"};


};
