/* -*- Mode: c++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
/*
 * Copyright (C) 2023 RPf 
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

#include <GeoJson.hpp>
#include <Geom2.hpp>

class GeoJsonGeometryHandler
: public GeoJsonHandler
{
public:
    GeoJsonGeometryHandler(const psc::gl::aptrGeom2& geometry, const Color& color, double geomRadius);
    virtual ~GeoJsonGeometryHandler() = default;

    void addFeature(JsonObject* feat) override {};
    void endFeature() override {};
    void addGeometry(JsonObject* geo) override {};
    void endGeometry() override {};
    void addMultiPolygon(JsonArray* multipoly) override {};
    void endMultiPolygon() override {};
    void addPolygon(JsonArray* poly) override {};
    void endPolygon() override {};
    void addShape(JsonArray* shape) override;
    void endShape() override;
    void addCoord(JsonArray* coord, bool last) override;
    int getPointsLimit();
    void setPointsLimit(int points);
private:
    bool m_first{true};
    int m_count{0};
    static constexpr auto DEFAULT_JSON_POINT_LIMIT{65535};  // as we fixed index for geometry to ushort
    int m_pointsLimit{DEFAULT_JSON_POINT_LIMIT};
    psc::gl::aptrGeom2 m_geometry;
    Color m_color;
    double m_geomRadius;
};

