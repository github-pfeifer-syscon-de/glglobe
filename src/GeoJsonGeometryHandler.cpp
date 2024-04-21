/*
 * Copyright (C) 2023 RPf <gpl3@pfeifer-syscon.de>
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

#include <glm/trigonometric.hpp>  //radians
#include <GenericGlmCompat.hpp>
#include <cmath>

#include "GeoJsonGeometryHandler.hpp"

GeoJsonGeometryHandler::GeoJsonGeometryHandler(const psc::gl::aptrGeom2& geometry, const Color& color, double geomRadius)
: GeoJsonHandler()
, m_geometry{geometry}
, m_color{color}
, m_geomRadius{geomRadius}
{

}

int GeoJsonGeometryHandler::getPointsLimit()
{
    return m_pointsLimit;
}

void
GeoJsonGeometryHandler::setPointsLimit(int points)
{
    m_pointsLimit = points;
}

void
GeoJsonGeometryHandler::addShape(JsonArray* shape)
{
    m_first = true;
}

void
GeoJsonGeometryHandler::endShape()
{
}

void
GeoJsonGeometryHandler::addCoord(JsonArray* coord, bool last)
{
    int coordLen = json_array_get_length(coord);
    if (coordLen >= 2) {
        double lon = json_array_get_double_element(coord, 0);
        double lat = json_array_get_double_element(coord, 1);
        gdouble rlon = M_PI + glm::radians(lon);       // get radians from degree
        gdouble rlat = glm::radians(lat);
        double const cosLat = std::cos(rlat);
        double const sinLat = std::sin(rlat);
        double const cosLon = std::cos(rlon);
        double const sinLon = std::sin(rlon);
        Position pnt;
        pnt.x = (cosLat * sinLon) * m_geomRadius;    // match definition of texture at -180° from greenwich and wrap (date border)
        pnt.y = sinLat * m_geomRadius;
        pnt.z = (cosLat * cosLon)* m_geomRadius;
        if (auto lgeo = m_geometry.lease()) {
            lgeo->addPoint(&pnt, &m_color, nullptr, nullptr, nullptr, nullptr);
            if (!m_first) {   // start with each poly
                lgeo->addIndex(m_count-1, m_count);    // create connected line
            }
        }
        m_first = false;
        ++m_count;
        if (m_count > m_pointsLimit) {
            throw JsonException(Glib::ustring::sprintf("The file contains more points %d (allowed %d) as we can handle", m_count, m_pointsLimit));
        }
    }
    else {
        // keep as warning ?
        std::cout << "Expected coords 2 got " << coordLen << std::endl;
    }

}

