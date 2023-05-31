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

#include <glm/gtc/type_ptr.hpp>

#include "SphereContext.hpp"

SphereContext::SphereContext()
: m_light_location{0}
, m_day_tex_location{0}
, m_night_tex_location{0}
, m_normal_tex_location{0}
, m_modelView3x3MatrixID{0}
, m_model_location{0}
, m_view_location{0}
, m_lightPosWorld{0}
, m_twilight{0}
, m_debug{0}
, m_specular_power{0}
, m_weather_tex_location{0}
, weather_alpha_location{0}
{
}


SphereContext::~SphereContext() {
}

void
SphereContext::updateLocation()
{
    NaviContext::updateLocation();
    //m_light_location = glGetUniformLocation(m_program, "light");
    m_day_tex_location = glGetUniformLocation(m_program, "dayTexture");
    m_night_tex_location= glGetUniformLocation(m_program, "nightTexture");
    m_normal_tex_location = glGetUniformLocation(m_program, "NormalTextureSampler");
    m_specular_tex_location = glGetUniformLocation(m_program, "SpecularTextureSampler");
    m_weather_tex_location = glGetUniformLocation(m_program, "weatherSampler");
    m_modelView3x3MatrixID = glGetUniformLocation(m_program, "MV3x3");
    m_model_location = glGetUniformLocation(m_program, "M");
    m_view_location = glGetUniformLocation(m_program, "V");
    m_lightPosWorld = glGetUniformLocation(m_program, "LightPosition_worldspace");
    m_ambient = glGetUniformLocation(m_program, "ambient");
    m_diffuse = glGetUniformLocation(m_program, "diffuseLightPower");
    m_specular = glGetUniformLocation(m_program, "specLightPower");
    m_twilight = glGetUniformLocation(m_program, "twilight");
    m_debug = glGetUniformLocation(m_program, "debug");
    m_specular_power = glGetUniformLocation(m_program, "specular_power");
    weather_alpha_location = glGetUniformLocation(m_program, "weather_alpha");
}

bool
SphereContext::useNormal()
{
    return true;
}

bool
SphereContext::useColor() {
    return false;
}

bool
SphereContext::useUV()
{
    return true;
}

bool
SphereContext::useNormalMap()
{
    return true;
}

void
SphereContext::setLight(Position &light, float ambient, float diffuse, float specular, float twilight, float specular_power, float weather_alpha)
{
    glUniform1f(m_ambient, ambient);
    glUniform1f(m_diffuse, diffuse);
    glUniform1f(m_specular, specular);
    glUniform1f(m_twilight, twilight);
    glUniform1f(m_specular_power, specular_power);
    glUniform1f(weather_alpha_location, weather_alpha);

    glUniform1i(m_day_tex_location, 0);     // this matches texture unit 0
    checkError("glUniform1i (m_day_tex)");
    glUniform1i(m_night_tex_location, 1);   // this matches texture unit 1
    checkError("glUniform1i (m_night_tex)");
    glUniform1i(m_normal_tex_location, 2);   // this matches texture unit 2
    checkError("glUniform1i (m_normal_tex_location)");
    glUniform1i(m_specular_tex_location, 3);   // this matches texture unit 3
    checkError("glUniform1i (m_specular_tex_location)");
    glUniform1i(m_weather_tex_location, 4);   // this matches texture unit 4
    checkError("glUniform1i (m_weather_tex_location)");

    //std::cout << "light "  << m_lightPosWorld << " " << lightPos.x << " " << lightPos.y << " " << lightPos[2] << std::endl;
    glUniform3fv(m_lightPosWorld, 1, glm::value_ptr(light));
    checkError("glUniform3fv (light)");
}

void
SphereContext::setDebug(unsigned int debug)
{
    glUniform1i(m_debug, debug);
    checkError("glUniform1i (debug)");
}

void
SphereContext::setModelView(glm::mat3x3 &modelView)
{
    //std::cout << "modelview[0] " << modelView[0][0] << " " << modelView[1][0] << " " << modelView[2][0] << std::endl;
    //std::cout << "modelview[1] " << modelView[0][1] << " " << modelView[1][1] << " " << modelView[2][1] << std::endl;
    //std::cout << "modelview[2] " << modelView[0][2] << " " << modelView[1][2] << " " << modelView[2][2] << std::endl;
    //std::cout << m_modelView3x3MatrixID << std::endl;
    glUniformMatrix3fv(m_modelView3x3MatrixID, 1, GL_FALSE, glm::value_ptr(modelView));
    checkError("glUniformMatrix3fv (ModelView)");
}

void
SphereContext::setModel(Matrix &model)
{
    //std::cout << "model[0] " << model[0][0] << " " << model[1][0] << " " << model[2][0] << " " << model[3][0] << std::endl;
    //std::cout << "model[1] " << model[0][1] << " " << model[1][1] << " " << model[2][1] << " " << model[3][1] << std::endl;
    //std::cout << "model[2] " << model[0][2] << " " << model[1][2] << " " << model[2][2] << " " << model[3][2] << std::endl;
    //std::cout << "model[3] " << model[0][3] << " " << model[1][3] << " " << model[2][3] << " " << model[3][3] << std::endl;
    //std::cout << m_model_location << std::endl;
    glUniformMatrix4fv(m_model_location, 1, GL_FALSE, glm::value_ptr(model));
    checkError("glUniformMatrix4fv (Model)");
}

void
SphereContext::setView(Matrix &view)
{
    //std::cout << "view[0] " << view[0][0] << " " << view[1][0] << " " << view[2][0] << " " << view[3][0] << std::endl;
    //std::cout << "view[1] " << view[0][1] << " " << view[1][1] << " " << view[2][1] << " " << view[3][1] << std::endl;
    //std::cout << "view[2] " << view[0][2] << " " << view[1][2] << " " << view[2][2] << " " << view[3][2] << std::endl;
    //std::cout << "view[3] " << view[0][3] << " " << view[1][3] << " " << view[2][3] << " " << view[3][3] << std::endl;
    //std::cout << m_view_location << std::endl;
    glUniformMatrix4fv(m_view_location, 1, GL_FALSE, glm::value_ptr(view));
    checkError("glUniformMatrix4fv (View)");
}

