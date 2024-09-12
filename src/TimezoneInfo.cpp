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

#include <glibmm.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <glm/trigonometric.hpp>  //radians
#ifdef __GNUC__
#  if __GNUC__ >= 13
#    ifdef __WIN32__
#  define USE_CHRONO_TZ
#include <chrono>
#include <format>
#    endif
#  endif
#endif
#include <Log.hpp>

#include "TimezoneInfo.hpp"
#include "StringUtils.hpp"

// if your system and glibc++ disagree on tzdata-format this might help
//#ifdef USE_CHRONO_TZ   // as the glibc++ with the tzdata 2024b seems broken keep this windows only see https://gcc.gnu.org/bugzilla/show_bug.cgi?id=116657
//namespace __gnu_cxx {
//    const char*
//    zoneinfo_dir_override()
//    {
//        // may give path and provide the tzdata.zi
//        return nullptr;     // this loads the last know good 2024a (internal?)
//    }
//} /* end __gnu_cxx */
//#endif

Hotspot::Hotspot(GeometryContext *_ctx)
: psc::gl::Geom2(GL_POINTS, _ctx)
{
}

Hotspot::~Hotspot()
{
}

void
Hotspot::setVisible(bool visible)
{
    // adapted Version set just child elements
    for (auto& geo : geometries) {
        if (auto lgeo = geo.lease()) {
            lgeo->setVisible(visible);
        }
    }
    if (auto ltext = m_text.lease()) {
        ltext->setVisible(visible);
    }
}

void
Hotspot::setSelfVisible(bool visible)
{
    m_visible = visible;
    // we dont want to change children here
}

void
Hotspot::setText(const psc::gl::aptrGeom2& text)
{
    m_text = text;
}

Tz::Tz(const std::string &line)
: country()
, lat{0.0f}
, lon{0.0f}
, name()
, info()
, valid{false}
, point{nullptr}
, line{nullptr}
, ctext{nullptr}
{
    std::vector<Glib::ustring> flds;
    StringUtils::split(line, '\t', flds);
    if (flds.size() >= 3) {
        country = flds[0];        // for the 1970 version this might be a list
        //if (country != "DE" && country != "AD") {
        //    return;
        //}
        std::string coord = flds[1];
        size_t n = coord.find('-', 1);
        if (n == std::string::npos)
            n = coord.find('+', 1);
        if (n != std::string::npos) {
            std::string slat = coord.substr(0, n);
            std::string slon = coord.substr(n);
            lat = dms2Decimal(slat,3);  // even if the precision of reading it as decimal seems to be sufficient, convert dms to decimal
            lon = dms2Decimal(slon,4);
            name = flds[2];
            if (flds.size() >= 4) {
                info = flds[3];
            }
            valid = true;
            //std::cout << "Tz " << country<< "\t" << lat << "\t" << lon << "\t" << name << "\t" << info << std::endl;
            return;
        }
    }
    psc::log::Log::logAdd(psc::log::Level::Notice,
                          Glib::ustring::sprintf("Parsing Timezones unusable line %s", line));
}

//Tz::Tz(const Tz& orig)
//: country{orig.country}
//, lat{orig.lat}
//, lon{orig.lon}
//, name{orig.name}
//, info{orig.info}
//, valid{orig.valid}
//, point{orig.point}
//, line{orig.line}       // copying these pointers is o.k. as they are nullptr in this stage, so if the logic get changed a additionl logic is needed for freeing these pointers
//, ctext{orig.ctext}
//{
//}

Tz::~Tz()
{
    // as line is a child of point it will be destructed with it !
    //if (line) {
    //    delete line;
    //}
    line.resetAll();
    ctext.resetAll();
    point.resetAll();
}


// convert DegreeMinuteSecond without separator to decimal
//   ndeg gives the places of degree including sign
float
Tz::dms2Decimal(const std::string &dms, guint ndeg)
{
    float deg = 0.0f;
    if (dms.length() >= ndeg) {
        std::istringstream strdeg(dms.substr(0, ndeg));
        strdeg >> deg;
        if (dms.length() >= ndeg+2) {
            std::istringstream strmm(dms.substr(ndeg, 2));
            float min = 0.0f;
            strmm >> min;
            if (deg < 0)
                min = -min;     // sign extends to all lower fields
            deg += min / 60.0f;
            if (dms.length() >= ndeg+4) {
                std::istringstream strsec(dms.substr(ndeg+2, 2));
                float sec = 0.0f;
                strsec >> sec;
                if (deg < 0)
                    sec = -sec;     // sign extends to all lower fields
                deg += sec / 3600.0f;
            }
        }
    }
    return deg;
}

bool
Tz::isValid()
{
    return valid;
}

void Tz::createGeometry(MarkContext *markContext, TextContext *m_textContext, const psc::gl::ptrFont2& font)
{
    Color red(1.0f, 0.0f, 0.0f);
    float lon = 180.0f + getLongitude();     // sphere aligns with texture which is at 180° so here we wrap again
    float lat = getLatitude();
    float rlon = glm::radians(lon);
    float rlat = glm::radians(lat);

    float const cosLat = std::cos(rlat);
    float const sinLat = std::sin(rlat);
    float const cosLon = std::cos(rlon);
    float const sinLon = std::sin(rlon);

    float const x = cosLat * sinLon;
    float const y = sinLat;
    float const z = cosLat * cosLon;

    Position v(x, y, z);

    Position s(v * 30.01f);
    Position e(v * 33.0f);

    line = psc::mem::make_active<psc::gl::Geom2>(GL_LINES, markContext);
    markContext->addGeometry(line);
    if (auto lline = line.lease()) {
        lline->addLine(s, e, red);
        lline->create_vao();
        lline->setVisible(false);
    }
    ctext = psc::mem::make_active<psc::gl::Text2>(GL_TRIANGLES, m_textContext, font);
    if (auto ltext = ctext.lease()) {
        ltext->setPosition(e);
        ltext->setScale(0.030f);
        ltext->setVisible(false);
        Rotational rot(lon, 0.0f, 0.0f);
        ltext->setRotation(rot);
        updateTime();
        m_textContext->addGeometry(ctext);
    }
    point = psc::mem::make_active<Hotspot>(markContext);
    if (auto lpoint = point.lease()) {
        lpoint->setSensitivity(0.03f);   // uses gl_view_coords
        lpoint->addPoint(&s, &red);
        lpoint->setSelfVisible(false);
        lpoint->addGeometry(line);
        lpoint->create_vao();
        lpoint->setText(ctext);
        markContext->addGeometry(point);
    }

}

void
Tz::updateTime()
{
#ifdef USE_CHRONO_TZ
    // this works for windows if you use Gcc 13.1+
    Glib::ustring tm{"??:??"};
    try {
        using namespace std::chrono;
        auto now = zoned_time{getName(), system_clock::now()};
        tm = std::format("{:%R}", now);
    }
    catch (const std::exception& ex) {
        if (!warned) {      // only report once
            psc::log::Log::logAdd(psc::log::Level::Warn, Glib::ustring::sprintf("Updating time %s failed with %s", getName(), ex.what()));
            warned = true;
        }
    }
#else
    // for windows Glib::TimeZone is fixed on windose zones...
    Glib::TimeZone tz = Glib::TimeZone::create(getName());
    Glib::DateTime dt = Glib::DateTime::create_now(tz);
    Glib::ustring tm = dt.format("%R");
#endif
    auto wcty = Glib::ustring::sprintf("%s %s", getName(), tm);
    if (auto ltext = ctext.lease()) {
        ltext->setText(wcty);
    }
}

void
Tz::setVisible(bool visible)
{
    if (auto ltext = ctext.lease()) {
        ltext->setVisible(visible);
    }
    if (auto lline = line.lease()) {
        lline->setVisible(visible);
    }
}

void
Tz::setDotVisible(bool visible)
{
    if (auto lpoint = point.lease())
        lpoint->setSelfVisible(visible);
}

TimezoneInfo::TimezoneInfo()
{
    char name[128];
    snprintf(name, sizeof(name), "/usr/share/zoneinfo/zone1970.tab");
    struct stat sb;
    int ret = stat(name, &sb);
    //std::cout << name << " ret: " << ret << " mode: " << sb.st_mode << std::endl;
    if (ret == ENOENT
     || ret == -1) {
        snprintf(name, sizeof(name), "/usr/share/zoneinfo/zone.tab");
        ret = stat(name, &sb);
        //std::cout << name << " ret: " << ret << " mode: " << sb.st_mode << std::endl;
        if (ret == ENOENT
         || ret == -1) {
            // alternative: const gchar *reg_key = "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Time Zones\\";
            //  if (RegQueryValueExA (key, "Std", nullptr, nullptr, (LPBYTE)&(tzi.StandardName), &size) != ERROR_SUCCESS)
            //  see https://github.com/GNOME/glib/blob/master/glib/gtimezone.c
            //  but this has no position info?
            const char *msyshome = getenv("MSYS_HOME");
            if (msyshome != nullptr) {
                snprintf(name, sizeof(name), "%s/share/zoneinfo/zone1970.tab", msyshome);
                ret = stat(name, &sb);
            }
            //std::cout << name << " ret: " << ret << " mode: " << sb.st_mode << std::endl;
        }
    }

    std::ifstream  stat;
    std::ios_base::iostate exceptionMask = stat.exceptions() | std::ios::failbit | std::ios::badbit | std::ios::eofbit;
    stat.exceptions(exceptionMask);
    try {
        stat.open(name);        /* Open tz file  */
        if (stat.is_open()) {
            while (!stat.eof()) {
                std::string str;
                std::getline(stat, str);
                std::string::size_type pos = str.find("#");    // these are comments
                if (pos != 0) {
                    Tz tz(str);
                    if (tz.isValid()) {
                        zones.emplace_back(std::move(tz));
                    }
                }
            }
        }
        else {
            psc::log::Log::logAdd(psc::log::Level::Warn,
                                  Glib::ustring::sprintf("Timezone reading %s not opened", name));
            //std::cerr << name << " coud not be read, no timzone info will be available." << std::endl;
        }
    }
    catch (const std::ios_base::failure &e) {
        if (!stat.eof()) {  // as we may hit eof while reading ...
            psc::log::Log::logAdd(psc::log::Level::Warn,
                                  Glib::ustring::sprintf("Timezone reading %s error %s code %s", name, e.what(), e.code().message() ));
        }
    }
    if (stat.is_open()) {
        stat.close();
    }
}


TimezoneInfo::~TimezoneInfo()
{
}

std::vector<Tz> &
TimezoneInfo::getZones()
{
    return zones;
}

void
TimezoneInfo::createGeometry(MarkContext *markContext, TextContext *textContext, const psc::gl::ptrFont2& font)
{
    std::vector<Tz>::iterator t;
    for (t = zones.begin(); t != zones.end(); ++t) {
        Tz &tz = *t;
        tz.createGeometry(markContext, textContext, font);
    }
}

void
TimezoneInfo::updateTime()
{
    std::vector<Tz>::iterator t;
    for (t = zones.begin(); t != zones.end(); ++t) {
        Tz &tz = *t;
        tz.updateTime();
    }
}

void
TimezoneInfo::setAllVisible(bool visible)
{
    std::vector<Tz>::iterator t;
    for (t = zones.begin(); t != zones.end(); ++t) {
        Tz &tz = *t;
        tz.setVisible(visible);
    }
}

void
TimezoneInfo::setDotVisible(bool visible)
{
    std::vector<Tz>::iterator t;
    for (t = zones.begin(); t != zones.end(); ++t) {
        Tz &tz = *t;
        tz.setDotVisible(visible);
    }
}
