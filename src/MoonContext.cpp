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


#include "MoonContext.hpp"

MoonContext::MoonContext()
: m_moon_light_location{0}
, m_moon_tex_location{0}
{
}


MoonContext::~MoonContext() {
}

void
MoonContext::updateLocation()
{
    NaviContext::updateLocation();
    m_moon_light_location = glGetUniformLocation(m_program, "light");
    m_moon_tex_location = glGetUniformLocation(m_program, "tex");
}

bool
MoonContext::useNormal()
{
    return true;
}

bool
MoonContext::useColor()
{
    return false;
}

bool
MoonContext::useUV()
{
    return true;
}


void
MoonContext::setLight(Position &light)
{

    glUniform1i(m_moon_tex_location, 0);     // this matches texture unit 0
    checkError("glUniform1i (m_texture)");

    //std::cout << "light "  << m_lightPosWorld << " " << lightPos.x << " " << lightPos.y << " " << lightPos[2] << std::endl;
    glUniform3fv(m_moon_light_location, 1, &light[0]);
    checkError("glUniform3fv (light)");
}


