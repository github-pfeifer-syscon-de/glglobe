/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4;  coding: utf-8; -*-  */
/*
 * Copyright (C) 2025 RPf
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
// see also background
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp> // pi ???
#include <glm/trigonometric.hpp>  //radians

class Moon
{
public:
    Moon();
    explicit Moon(const Moon& orig) = delete;
    virtual ~Moon() = default;

    static double asJulianDate(Glib::DateTime& now);
    // the default semantic is (angular phase (rad)):
    //  0 -> full
    // PI -> new
    // 2PI ->full
    static double moonPhase(double jd);
    static double getIlluminated(double i);

    static constexpr auto SYNOD_MOON{29.530589};
    constexpr static auto SEC_PER_JULIAN_DAY = 86400.0;
    constexpr static auto DAYS_PER_CENTURY = 36525.0;
    constexpr static auto JULIAN_1970_OFFS = 2440587.5;
    constexpr static auto MOON_J2000 = 2451545.0;

private:

};

