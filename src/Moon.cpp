/* -*- Mode: c++; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
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


#include "Moon.hpp"

Moon::Moon()
{

}


static double
constrain(double d)
{
	double t = std::fmod(d, 360.0);
	if (t < 0.0) {
	    t += 360.0;
	}
    return t;
}


double
Moon::asJulianDate(Glib::DateTime& now)
{
    double jd = ((static_cast<double>(now.to_unix()) / SEC_PER_JULIAN_DAY) + JULIAN_1970_OFFS);
    return jd;
}


double
Moon::moonPhase(double jd)
{
    // complex algorithm as suggested by Greg Miller see https://celestialprogramming.com/
    const double T = (jd - MOON_J2000) / DAYS_PER_CENTURY;  // epoch centuries
	const double T2 = T * T;
	const double T3 = T2 * T;
	const double T4 = T3 * T;
	double D = glm::radians(constrain(297.8501921 + 445267.1114034*T - 0.0018819*T2 + 1.0/545868.0*T3 - 1.0/113065000.0*T4)); //47.2
	double M = glm::radians(constrain(357.5291092 + 35999.0502909*T - 0.0001536*T2 + 1.0/24490000.0*T3)); //47.3
	double Mp = glm::radians(constrain(134.9633964 + 477198.8675055*T + 0.0087414*T2 + 1.0/69699.0*T3 - 1.0/14712000.0*T4)); //47.4
	//48.4
	double i = glm::radians(constrain(180.0 - glm::degrees(D) - 6.289 * std::sin(Mp) + 2.1 * std::sin(M) -1.274 * std::sin(2.0*D - Mp) -0.658 * std::sin(2.0*D) -0.214 * std::sin(2.0*Mp) -0.11 * std::sin(D)));
    return i;
}

double
Moon::getIlluminated(double i)
{
    return (1.0 + std::cos(i)) / 2.0;
}
