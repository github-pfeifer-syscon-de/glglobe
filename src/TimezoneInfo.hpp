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
#include <wchar.h>
#include <locale>
#include <string>

#include "MarkContext.hpp"
#include "TextContext.hpp"
#include "Text2.hpp"

class Hotspot
: public psc::gl::Geom2
{
public:
    Hotspot(GeometryContext *_ctx);
    virtual ~Hotspot();

    void setVisible(bool visible) override;
    void setSelfVisible(bool visible);
    void setText(const psc::gl::aptrGeom2& text);
private:
    psc::gl::aptrGeom2 m_text;
};

class Tz {
public:
    Tz(const std::string &line);
    Tz(const Tz& orig) = default;
    virtual ~Tz();

    bool isValid();
    const std::string &getCountry() {
        return country;
    }
    const std::string &getName() {
        return name;
    }
    const std::string &getInfo() {
        return info;
    }
    float getLatitude() {
        return lat;
    }
    float getLongitude() {
        return lon;
    }
    void createGeometry(MarkContext *markContext, TextContext *textContext, const psc::gl::ptrFont2& font);
    void updateTime();
    void setVisible(bool visible);
    void setDotVisible(bool visible);
    float dms2Decimal(const std::string &dms, guint ndeg);
private:
    std::string country;
    float lat;
    float lon;
    std::string name;
    std::string info;
    bool valid;
    psc::mem::active_ptr<Hotspot> point;
    psc::gl::aptrGeom2 line;
    psc::gl::aptrText2 ctext;
    bool warned{false};
};

class TimezoneInfo {
public:
    TimezoneInfo();
    virtual ~TimezoneInfo();

    std::vector<Tz> &getZones();
    void createGeometry(MarkContext *markContext, TextContext *textContext, const psc::gl::ptrFont2& font);
    void updateTime();
    void setAllVisible(bool visible);
    void setDotVisible(bool visible);
private:
    std::vector<Tz> zones;
};

