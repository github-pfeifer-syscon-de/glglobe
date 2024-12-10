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

#include "NaviContext.hpp"


class SphereContext : public NaviContext {
public:
    SphereContext();
    virtual ~SphereContext();
    void setLight(Position &light, float ambient, float diffuse, float specular, float twilight, float specular_power, float weather_alpha);
    void setModelView(glm::mat3x3 &modelView);
    void setModel(Matrix &model);
    void setView(Matrix &view);
    bool useNormal() override;
    bool useColor() override;
    bool useUV() override;
    bool useNormalMap() override;
    void setDebug(unsigned int debug);
protected:
    void updateLocation() override;

private:
    GLint m_light_location;
    GLint m_day_tex_location;
    GLint m_night_tex_location;
    GLint m_normal_tex_location;
    GLint m_specular_tex_location;
    GLint m_modelView3x3MatrixID;
    GLint m_model_location;
    GLint m_view_location;
    GLint m_lightPosWorld;
    GLint m_ambient;
    GLint m_diffuse;
    GLint m_specular;
    GLint m_twilight;
    GLint m_debug;
    GLint m_specular_power;
    GLint m_weather_tex_location;
    GLint weather_alpha_location;

};

