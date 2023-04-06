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
#include "GenericGlmCompat.hpp"

#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <fstream>
#include <gtkmm.h>
#include <GenericGlmCompat.hpp>
#include <list>
#include <stdlib.h>
#include <cmath>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/trigonometric.hpp>  //radians

#include "GlSphereView.hpp"
#include "SunSet.hpp"
#include "GeoJson.hpp"
#include "ConfigDialog.hpp"
#include "SphereGlArea.hpp"

GlSphereView::GlSphereView(Config *config)
: Scene()
, m_config{config}
, m_earthContext{nullptr}
, m_textContext{nullptr}
, m_fixView{1.0f}
, m_earth{nullptr}
, m_dayTex{nullptr}
, m_nightTex{nullptr}
, m_normalMapTex{nullptr}
, m_speculatMapTex{nullptr}
, m_weatherTex{nullptr}
, m_font{nullptr}
, m_light()
, m_earth_declination_deg{0.0}
, m_text{nullptr}
, debugGeom{nullptr}
, m_timezoneInfo{nullptr}
, m_markContext{nullptr}
, m_sunRise{0.0}
, m_sunSet{0.0}
, geoJsonGeom{nullptr}
, m_moonContext{nullptr}
, m_moon{nullptr}
, m_moonTex{nullptr}
{
}


GlSphereView::~GlSphereView()
{
    if (m_font != nullptr) {
        #ifndef __WIN32__   // unclear why this fails for windows
        delete m_font;
        #endif
    }
}

float
GlSphereView::getZFar()
{
    return 300.0f;
}

Position
GlSphereView::getIntialPosition()
{
    return Position(0.0f, 0.0f, 70.0f);
}

Rotational
GlSphereView::getInitalAngleDegree()
{
    return Rotational(0.0f, 0.0f, 0.0f);    // doesn't count earth has a separate view lat/lon
}

Matrix
GlSphereView::getLookAt(Vector &view, Vector &direction, Vector &up)
{
    //Vector pEarth = m_earth->getPosition();
    // we want a orbiting view
    return glm::lookAt(
        glm::length(view) * direction,           // Camera is here
        Vector(0.0f, 0.0f, 0.0f),       // fix view on earth
        up);
}

Gdk::EventMask
GlSphereView::getAddEventMask()
{
    return Gdk::EventMask::POINTER_MOTION_MASK;
}

void
GlSphereView::updateTimer()
{
    Glib::DateTime date = Glib::DateTime::create_now_utc();
    unsigned int ds = (61 - date.get_second());    // try to hit next minute change, with 60 we may hit 59
    sigc::slot<bool> slot = sigc::mem_fun(*this, &GlSphereView::view_update);
    m_timer = Glib::signal_timeout().connect_seconds(slot, ds);
    //std::cout << "updateTimer " << date.get_hour() << ":" << date.get_minute() << ":" << date.get_second() << " " << ds << std::endl;
}

void
GlSphereView::calcuateLight()
{
    calcuateEarthLight();
    calcuateMoonLight();
}

void
GlSphereView::calcuateEarthLight()
{
    Glib::DateTime dateLocal = Glib::DateTime::create_now_local();
	Glib::TimeSpan timeSpan = dateLocal.get_utc_offset();
	double offsetUtcH = (double)(timeSpan) / (1.0e6 * 3600.0); // us -> h
	//std::cout << "offsetUtcH " << offsetUtcH << std::endl;
    Glib::DateTime dateUtc = Glib::DateTime::create_now_utc();
    float t = (float)(dateUtc.get_hour() * 60 + dateUtc.get_minute()) / (24.0f * 60.0f);
    //float d = (float)dateUtc.get_day_of_year();   // approximate season
    //std::cout <<  "s: " << s << " t: " << t << std::endl;
    //std::cout <<  "dayOfYear: " << d << " utcHour: " << date.get_hour() << std::endl;
    float r =  - t * 2.0f * M_PI;
    // https://en.wikipedia.org/wiki/Position_of_the_Sun
    //m_earth_declination_deg = -23.44f * std::cos((2.0f * M_PI) / 365.0f * (d + t + 9.0f));
	SunSet sunSet(m_config->getLatitude(), m_config->getLongitude(), offsetUtcH);
	sunSet.setCurrentDate(dateLocal.get_year(), dateLocal.get_month(), dateLocal.get_day_of_month());
	m_earth_declination_deg = sunSet.calcSunDeclination();
	m_sunRise = sunSet.calcSunrise();
    m_sunSet = sunSet.calcSunset();
	//std::cout << "m_earth_declination_deg " << m_earth_declination_deg
	//	      << " m_sunRise " << m_sunRise
	//	      << " m_sunSet " << m_sunSet
	//	      << std::endl;
    //std::cout << "d: " << d << " incl: " << incl << std::endl;
    float x = std::sin(r);
    float y = std::tan(Rotational::deg2radians(m_earth_declination_deg));
    float z = std::cos(r);
    m_light = Vector(x, y, z);
    m_light = glm::normalize(m_light);
}

gboolean
GlSphereView::view_update()
{
    calcuateLight();    // do this only on timer updates as a sub minute precision is not needed
                        //   so we keep trigonometric functions out of rendering ...
    if (m_timezoneInfo)
        m_timezoneInfo->updateTime();
    if (m_weather) {
        auto productId = m_config->getWeatherProductId();
        m_weather->check_product(productId);
    }
    updateTimer();

    m_naviGlArea->queue_draw();
    return false;   // as we somewhat between the functions (want to stop timer, and run it once) we deny the repetition here
}

gboolean
GlSphereView::init_shaders(Glib::Error &error)
{
    gboolean ret = init_earth_shaders(error);
    if (ret) {
        ret = init_moon_shaders(error);
    }
    return ret;
}

gboolean
GlSphereView::init_earth_shaders(Glib::Error &error)
{
    gboolean ret = TRUE;
    try {
        m_earthContext = new SphereContext();
        gsize szVertex,szFragm;
        Glib::RefPtr<const Glib::Bytes> refVertex = Gio::Resource::lookup_data_global(
                                        RESOURCE::resource("sphere-vertex.glsl"));
        Glib::RefPtr<const Glib::Bytes> refFragm = Gio::Resource::lookup_data_global(
                                        RESOURCE::resource("sphere-fragment.glsl"));

		std::string vertexVersioned, fragmVersioned;
		#ifndef USE_GLES	// keep shaders compatbile, thats the main benefit of es 3.0
		vertexVersioned = "#version 330 core\n";
		fragmVersioned = "#version 330 core\n";
		#else
		vertexVersioned = "#version 300 es\n"
			"precision mediump float;\n";
		fragmVersioned = "#version 300 es\n"
			"precision mediump float;\n";
		#endif
		const char* sVertex = (const char*)refVertex->get_data(szVertex);
		vertexVersioned += sVertex;
		const char *sFragm = (const char*)refFragm->get_data(szFragm);
		fragmVersioned += sFragm;
        ret = m_earthContext->createProgram(vertexVersioned.c_str(), fragmVersioned.c_str(), error);
        // unclear the docs say something about  Glib::bytes_unref().
        //   but the Bytes doc says never do this manually...
        if (ret) {
            m_textContext = new TextContext(GL_QUADS);
            ret = m_textContext->createProgram(error);
        }
        if (ret) {
            // use this for stdGL+ES
            glEnable(GL_PROGRAM_POINT_SIZE);
            m_markContext = new MarkContext();
            ret = m_markContext->createProgram(error);
        }
    }
    catch (const Glib::Error &err) {
        error = err;
        ret = FALSE;
    }
    return ret;
}

gboolean
GlSphereView::init_moon_shaders(Glib::Error &error)
{
    gboolean ret = TRUE;
    try {
        m_moonContext = new MoonContext();
        gsize szMoonVertex,szMoonFragm;
        Glib::RefPtr<const Glib::Bytes> refMoonVertex = Gio::Resource::lookup_data_global(
                                            RESOURCE::resource("moon-vertex.glsl"));
        Glib::RefPtr<const Glib::Bytes> refMoonFragm = Gio::Resource::lookup_data_global(
                                            RESOURCE::resource("moon-fragment.glsl"));
		std::string vertexVersioned, fragmVersioned;
		#ifndef USE_GLES	// keep shaders compatbile, thats the main benefit of es 3.0
		vertexVersioned = "#version 330 core\n";
		fragmVersioned = "#version 330 core\n";
		#else
		vertexVersioned = "#version 300 es\n"
			"precision mediump float;\n";
		fragmVersioned = "#version 300 es\n"
			"precision mediump float;\n";
		#endif
		const char* sVertex = (const char*)refMoonVertex->get_data(szMoonVertex);
		vertexVersioned += sVertex;
		const char *sFragm = (const char*)refMoonFragm->get_data(szMoonFragm);
		fragmVersioned += sFragm;

        ret = m_moonContext->createProgram(vertexVersioned.c_str(), fragmVersioned.c_str(), error);
  }
    catch (const Glib::Error &err) {
        error = err;
        ret = FALSE;
    }
    return ret;
}

void
GlSphereView::init(Gtk::GLArea *glArea)
{
    m_naviGlArea = (SphereGlArea *)glArea;
    m_earth = m_earthContext->createGeometry(GL_TRIANGLES);
    //enable this to see normals, tangents and bitangents
    //debugGeom = m_markContext->createGeometry(GL_LINES);
    //m_earth->setDebugGeometry(debugGeom);
    m_earth->addSphere(EARTH_RADIUS, 32, 32);  // with simplified calculation can use more details
    checkError("add Sphere earth");
    m_earth->create_vao();
    Position p(0.0f, 0.0f, 0.0f);
    m_earth->setPosition(p);
    checkError("add createVao earth");
    //std::cout << "geo vert: " << m_earth->getNumVertex()
    //          << " idx: " << m_earth->getNumIndex()
    //          << std::endl;
    setDayTextureFile(m_config->getDayTextureFile());
    setNightTextureFile(m_config->getNightTextureFile());
	// take images from file should safe some memory
    m_normalMapTex = Tex::fromFile(PACKAGE_DATA_DIR "/2k_earth_normal_map.tif");
    m_speculatMapTex = Tex::fromFile(PACKAGE_DATA_DIR "/2k_earth_specular_map.tif");
    m_weather_pix = Gdk::Pixbuf::create(Gdk::COLORSPACE_RGB, true, 8, get_weather_image_size(), get_weather_image_size());
    m_weather_pix->fill(0x00);  // transp. black
    m_weatherTex = new Tex();
    m_weatherTex->create(m_weather_pix);
    m_weather = std::make_shared<Weather>(this);
    m_weather->capabilities();

    m_font = new Font("sans-serif");    // Css2 name should give use some sans font
    m_text = new Text(GL_QUADS, m_textContext, m_font);
    Position pt(15.0f, -20.0f, -30.0f);
    m_text->setScale(2.07f);
    m_text->setPosition(pt);
    Rotational rotT(180.0f, 0.0f, 0.0f);
    m_text->setRotation(rotT);

    m_timezoneInfo = new TimezoneInfo();
    m_timezoneInfo->createGeometry(m_markContext, m_textContext, m_font);

    setGeoJsonFile(m_config->getGeoJsonFile());
    m_fixView = m_naviGlArea->getView();
    Rotational rot = get_rotation();
    m_naviGlArea->setRotation(rot);
    //m_sphere->setRotation(rot);       // just change model, but need to account sun as well
    m_naviGlArea->updateView();
    view_update();

    m_moon = m_moonContext->createGeometry(GL_TRIANGLES);
    // with simplified calculation can use more details
    m_moon->addSphere(20.0f, 24, 24);
    checkError("add Sphere moon");
    m_moon->create_vao();
    Position p_moon(0.0f, 0.0f, 0.0f);
    m_moon->setPosition(p_moon);
    m_moon->setRotation(getInitalAngleDegree());
    checkError("add createVao moon");
    m_moonTex = Tex::fromFile(PACKAGE_DATA_DIR "/2k_moon.jpg");
	//m_moonTex = Tex::fromResource(RESOURCE::resource("2k_moon.jpg"));
    // simple fixed view for moon
    glm::vec3 direction(0.0f, 0.0f, -1.0f);
    glm::vec3 right = glm::vec3(-1.0f, 0.0f, 0.0f);
    glm::vec3 up = glm::cross(right, direction);
    Position moonView{0.0f, 0.0f, 70.0f};
    m_moonViewMat = glm::lookAt(
        glm::length(moonView) * direction,
        Vector(0.0f),       // fix view on moon
        up);

}

void
GlSphereView::unrealize()
{
    if (m_timer.connected()) {
        m_timer.disconnect(); // No more updating
    }
    if (m_config) {
        delete m_config;
        m_config = nullptr;
    }
    if (m_text != nullptr) {
        delete m_text;
    }
    if (m_earth != nullptr) {
        delete m_earth;
    }
    if (m_earthContext != nullptr) {
        delete m_earthContext;
    }
    if (m_textContext != nullptr) {
        delete m_textContext;
    }
    if (m_dayTex != nullptr) {
        delete m_dayTex;
    }
    if (m_nightTex != nullptr) {
        delete m_nightTex;
    }
    if (m_normalMapTex != nullptr) {
        delete m_normalMapTex;
    }
    if (m_speculatMapTex != nullptr) {
        delete m_speculatMapTex;
    }
    if (m_weatherTex != nullptr) {
        delete m_weatherTex;
    }
    if (debugGeom != nullptr) {
        delete debugGeom;
    }
    if (m_timezoneInfo != nullptr) {
        delete m_timezoneInfo;
    }
    if (m_markContext) {
        delete m_markContext;
    }
    if (geoJsonGeom) {
        delete geoJsonGeom;
    }
   if (m_moon != nullptr) {
        delete m_moon;
    }
    if (m_moonContext != nullptr) {
        delete m_moonContext;
    }
    if (m_moonTex != nullptr) {
        delete m_moonTex;
    }
}

void
GlSphereView::weather_products_ready()
{
    request_weather_product();
}

void
GlSphereView::request_weather_product()
{

    auto weatherProductId = m_config->getWeatherProductId();
    m_weather_pix->fill(0x0);    // indicate something is going on by setting transp. black
    update_weather_tex();
    if (!weatherProductId.empty()) {
        m_weather->request(weatherProductId);
    }

}

int
GlSphereView::get_weather_image_size()
{
    return 1024;    // used for texture so requires power of two, higher values (e.g. 2048) lead to size limit exceeded...
}

/**
 *  with the original images alpha just on and off
 *   this function would allow some content sensitivity
 */
void
GlSphereView::color_to_alpha(Glib::RefPtr<Gdk::Pixbuf> pix)
{
    if (pix->get_n_channels() == 4) {
        float weatherTransp = (float)m_config->getWeatherTransparency();
        int pixwidth = pix->get_width();    // these will fit into our texture as we created it to fit
        int pixheight = pix->get_height();
        auto pixdata = pix->get_pixels();
        int rowstride = pix->get_rowstride();
        int n_channels = pix->get_n_channels();
        for (int yp = 0; yp < pixheight; ++yp) {
            guchar *p = pixdata + yp * rowstride;   // doing this once per row will be sufficient
            for (int xp = 0; xp < pixwidth; ++xp) {
                // The pixel we wish to modify
                //guchar *p = pixdata + yp * rowstride + xp * n_channels;
                // check byte order ???
                guint alpha = std::max(std::max(p[0], p[1]), p[2]);
                p[3] = (guint8)std::min((int)(alpha * weatherTransp), 0xff);  // alpha mask white = opaque, black = transparent
                p += n_channels;
            }
        }
    }
    else {
        std::cout << "Try to change alpha on pixmap without alpha!!!" << std::endl;
    }
}

void
GlSphereView::update_weather_tex()
{
    m_naviGlArea->make_current();   // context for gl calls
    m_weatherTex->create(m_weather_pix);
    m_naviGlArea->queue_draw();     // refresh
}

void
GlSphereView::weather_image_notify(WeatherImageRequest& request)
{
    if (m_weather_pix) {
        Glib::RefPtr<Gdk::Pixbuf> pix = request.get_pixbuf();
        if (pix) {
            // modify alpha it the provided alpha isn't what you like
            //color_to_alpha(pix);
            int pixwidth = pix->get_width();    // these will fit into our texture as we created it to fit
            int pixheight = pix->get_height();
            //std::cout << Weather::dump(pixdata, 64u) << std::endl;
            // copy to dest
            if (false) {
            std::cout << "GlSphereView::weatherNotify got"
                      << " xi " << request.get_pixX()
                      << " yi " << request.get_pixY()
                    //  << " width " << width
                    //  << " height " << height
                      << " pixwidth  " << pixwidth
                      << " pixheight  " << pixheight
                      << std::endl;
            }
            request.mapping(pix, m_weather_pix);
            update_weather_tex();
        }
    }
    else {
        std::cout << "GlSphereView::weatherNotify no dest pixbuf" << std::endl;
    }
}

static void
showMat(glm::mat4 &proj)
{
    for (int y = 0; y < 4; ++y) {
        std::cout << std::fixed << std::setprecision(2) << std::setw(8)
                  << (proj[y][0]) << "|" << (proj[y][1]) << "|" << (proj[y][2]) << "|" << (proj[y][3])
                  << std::endl;
    }
}

void
GlSphereView::draw(Gtk::GLArea *glArea, Matrix &projin, Matrix &view)
{
    glCullFace(GL_BACK);
    checkError("cull back");
    Position viewPos = m_naviGlArea->get_viewpos();
    float xOffs = -20.0f;
    if (viewPos.z <= 50.0f) {
        xOffs = 0.0f;   // on zoom move to center
    }
    else if (viewPos.z < 70.0f) {
        xOffs = (50.0f - viewPos.z);
    }
    // use a modified transform to display earth with a offset
    Matrix earthProj = glm::translate(projin, glm::vec3{xOffs, 0.0f, 0.0f});
    Matrix earthProjView = earthProj * view;
    // earth
    m_earthContext->use();
    checkError("useSpherectx");
    m_dayTex->use(GL_TEXTURE0);
    m_nightTex->use(GL_TEXTURE1);
    m_normalMapTex->use(GL_TEXTURE2);
    m_speculatMapTex->use(GL_TEXTURE3);
    m_weatherTex->use(GL_TEXTURE4);
    checkError("tex use");

    Position lightPos = m_light * m_config->getDistance();
    m_earthContext->setLight(lightPos, m_config->getAmbient(), m_config->getDiffuse(),
                            m_config->getSpecular(), m_config->getTwilight(),
                            m_config->getSpecularPower(),
                            m_config->getWeatherTransparency());
    m_earthContext->setDebug(m_config->getDebug());
    // used for normal map
    glm::mat4 modelViewMatrix = view * m_earth->getTransform();
    glm::mat3 modelView3x3Matrix = glm::mat3(modelViewMatrix); // Take the upper-left part of ModelViewMatrix
    m_earthContext->setModelView(modelView3x3Matrix);
    m_earthContext->setModel(m_earth->getTransform());
    m_earthContext->setView(view);

    m_earthContext->display(earthProjView);

    m_earthContext->unuse();
    m_dayTex->unuse();
    m_nightTex->unuse();
    m_normalMapTex->unuse();
    m_speculatMapTex->unuse();
    m_weatherTex->unuse();

    //Rotational rinv(rot.getPhi(), rot.getTheta(), rot.getPsi());
    //m_timezoneInfo->updateRot(rinv);
    Matrix projViewSphere = earthProjView * m_earth->getTransform();
    m_markContext->use();
    glLineWidth(1.0f);      // 2.0f has deprecated issues win/intel ogl
    // pointSize set in shader and enabled by glEnable(GL_PROGRAM_POINT_SIZE);
    m_markContext->display(projViewSphere);
    m_markContext->unuse();

    m_textContext->use();
    Color cGreen = Color(0.0f, 1.0f, 0.5f);
    m_textContext->setColor(cGreen);
    m_textContext->setTexture(0);
    m_textContext->display(projViewSphere);

    Color cBlue = Color(0.3f, 0.5f, 1.0f);
    m_textContext->setColor(cBlue);
    Glib::DateTime date = Glib::DateTime::create_now_local();
    std::string prepared = customize_time(m_config->getTimeFormat());
    Glib::ustring buffer = date.format(prepared);
    Matrix projFix = projin * m_fixView;
    m_text->setText(buffer);        // as we use a special transform display separately
    std::list<Geometry *> geos;
    geos.push_back(m_text);
    m_textContext->display(projFix, geos);
    m_textContext->unuse();

    // do the moon at some offset
    Matrix moonProj = glm::translate(projin, glm::vec3{35.0f, 0.0f, 0.0f});
    Matrix moonProjView = moonProj * m_moonViewMat;
    // moon
    m_moonContext->use();
    m_moonTex->use(GL_TEXTURE0);
    m_moonContext->setLight(m_moonLight);

    m_moonContext->display(moonProjView);

    m_moonContext->unuse();
    m_moonTex->unuse();
}

Glib::ustring
GlSphereView::hm(const double& timeM)
{
	int h = (int)timeM / 60;
	int m = (int)timeM % 60;
	Glib::ustring hm(Glib::ustring::sprintf(u8"%02d:%02d", h, m));
	return hm;
}

// prepare time-format with our extension %rise,%set,%D,%weather and \n
std::string
GlSphereView::customize_time(std::string prepared)
{
    int pos = prepared.find("%D", 0);
    if (pos >= 0) { // use std format with %D for declication
        Glib::ustring d(Glib::ustring::sprintf(u8"%.1f°", m_earth_declination_deg));
        prepared.replace(pos, 2, d);
    }
	pos = prepared.find("%rise", 0);
    if (pos >= 0) {
        auto shm = hm(m_sunRise);
        prepared.replace(pos, 5, shm);
    }
	pos = prepared.find("%set", 0);
    if (pos >= 0) {
        auto shm = hm(m_sunSet);
        prepared.replace(pos, 4, shm);
    }
	pos = prepared.find("%weather", 0);
    if (pos >= 0) {
        Glib::ustring info;
        if (m_weather) {
            auto prod = m_weather->find_product(m_config->getWeatherProductId());
            if (prod) {
                Glib::DateTime dateTime;
                if (prod->latest(dateTime)) {
                    info = dateTime.format("%R");
                }
            }
        }
        prepared.replace(pos, 8, info);
    }

    while (true) { // use std format with \n for newline
        pos = prepared.find("\\n", 0);
        if (pos < 0) {
            break;
        }
        prepared.replace(pos, 2, "\n");
    }
    return prepared;
}

Geometry *
GlSphereView::on_click_select(GdkEventButton* event, float mx, float my)
{
    Geometry *selected = m_markContext->hit(mx, my);
    return selected;
}

bool
GlSphereView::on_motion_notify_event(GdkEventMotion* event, float mx, float my)
{
    bool btn = (event->state & (Gdk::ModifierType::BUTTON1_MASK
                              | Gdk::ModifierType::BUTTON2_MASK
                              | Gdk::ModifierType::BUTTON3_MASK)) != 0;
    if (!btn)  {
        if (m_timezoneInfo) {
            m_timezoneInfo->setAllVisible(false);
        }
        Geometry *globe = m_earthContext->hit(mx, my);
        if (globe == nullptr) {
            m_timezoneInfo->setDotVisible(false);
            return false;
        }
        m_timezoneInfo->setDotVisible(true);
        Geometry *selected = m_markContext->hit(mx, my);
        if (selected != nullptr) {
            selected->setVisible(true);
        }
        m_naviGlArea->queue_draw();
    }
    return false;
}

Rotational
GlSphereView::get_rotation()
{
    return Rotational(m_config->getLatitude(), -180.0f+m_config->getLongitude(), 0.0f);   // Greenwich is at 180° as texture is cut this way (date border)
}

void
GlSphereView::pos_update()
{
    Rotational rot = get_rotation();
    m_naviGlArea->setRotation(rot);
    m_naviGlArea->updateView();
    m_naviGlArea->queue_draw();
}

void
GlSphereView::lon_changed(Gtk::SpinButton* lon_spin)
{
    m_config->setLongitude(lon_spin->get_value_as_int());
    pos_update();
}

void
GlSphereView::lat_changed(Gtk::SpinButton* lat_spin)
{
    m_config->setLatitude(lat_spin->get_value_as_int());
    pos_update();
}

void
GlSphereView::setDayTextureFile(std::string &dayTex)
{
    Tex *old = m_dayTex;
    bool def = true;
    if (dayTex.length() > 0) {
        try {
            m_dayTex = Tex::fromFile(dayTex);
            def = false;
            m_config->setDayTextureFile(dayTex);
        }
        catch (const Glib::Error &exc) {
            show_error(exc.what());
        }
    }
    if (def) {
        m_dayTex = Tex::fromFile(PACKAGE_DATA_DIR "/2k_earth_daymap.jpg");
		//m_dayTex = Tex::fromResource(RESOURCE::resource("2k_earth_daymap.jpg"));
        m_config->setDayTextureFile("");
    }
    if (old != nullptr)
        delete old;
    m_naviGlArea->queue_draw();
}

void
GlSphereView::ambient_changed(Gtk::Scale* ambient)
{
    m_config->setAmbient(ambient->get_value());
    m_naviGlArea->queue_draw();
}

void
GlSphereView::diffuse_changed(Gtk::Scale* diffuse)
{
    m_config->setDiffuse(diffuse->get_value());
    m_naviGlArea->queue_draw();
}

void
GlSphereView::specular_changed(Gtk::Scale* specular)
{
    m_config->setSpecular(specular->get_value());
    m_naviGlArea->queue_draw();
}

void
GlSphereView::twilight_changed(Gtk::Scale* twilight)
{
    m_config->setTwilight(twilight->get_value() / 180.0f); // degree to dot -1..1
    m_naviGlArea->queue_draw();
}

void
GlSphereView::debug_changed(Gtk::CheckButton* debug)
{
    m_config->setDebug(debug->get_active() ? 1 : 0);
    m_naviGlArea->queue_draw();
}

void
GlSphereView::specular_power_changed(Gtk::Scale* specular_pow)
{
    m_config->setSpecularPower(specular_pow->get_value());
    m_naviGlArea->queue_draw();
}

void
GlSphereView::distance_changed(Gtk::Scale* distance)
{
    m_config->setDistance(distance->get_value());
    m_naviGlArea->queue_draw();
}

void
GlSphereView::setNightTextureFile(std::string &nightTex)
{
    Tex *old = m_nightTex;
    bool def = true;
    if (nightTex.length() > 0) {
        try {
            m_nightTex = Tex::fromFile(nightTex);
            def = false;
            m_config->setNightTexureFile(nightTex);
        }
        catch (const Glib::Error &exc) {
            show_error(exc.what());
        }
    }
    if (def) {
        m_nightTex = Tex::fromFile(PACKAGE_DATA_DIR "/2k_earth_nightmap.jpg");
		//m_nightTex = Tex::fromResource(RESOURCE::resource("2k_earth_nightmap.jpg"));
        m_config->setNightTexureFile("");
    }
    if (old != nullptr)
        delete old;
    m_naviGlArea->queue_draw();
}

bool
GlSphereView::setGeoJsonFile(const Glib::ustring& file)
{
    if (geoJsonGeom) {
        delete geoJsonGeom;
        geoJsonGeom = nullptr;
    }
    bool ret = false;
    if (!file.empty()) {
        Glib::RefPtr<Gio::File> f = Gio::File::create_for_path(file);
        if (!f->query_exists()) {
            auto msg = Glib::ustring::sprintf("The requested file %s does not exist.", file);
            show_error(msg);
        }
        else {
            Glib::RefPtr<Gio::Cancellable> cancel = Gio::Cancellable::create();
            Glib::RefPtr<Gio::FileInfo> attr = f->query_info(cancel, "standard::*");
            if (attr->get_size() > GeoJson::GEO_FILE_SIZE_LIMIT) {
                auto msg = Glib::ustring::sprintf("The requested file %s exceeds the size limit %d with %d.",
                                         file, GeoJson::GEO_FILE_SIZE_LIMIT, attr->get_size());
                show_error(msg);
            }
            else {
                m_naviGlArea->make_current();
                geoJsonGeom = m_markContext->createGeometry(GL_LINES);
                geoJsonGeom->setMarkable(false);
                GeoJson geoJson;
                geoJson.set_radius(EARTH_RADIUS);
                try {
                    geoJson.read(file, geoJsonGeom);
                    geoJsonGeom->create_vao();
                    ret = true;
                }
                catch (const JsonException& ex) {
                    delete geoJsonGeom; // this would be unusable
                    auto msg = ex.what();
                    show_error(msg);
                }
            }
        }
    }
    m_naviGlArea->queue_draw();
    return ret;
}

void
GlSphereView::show_error(const std::string& msg)
{
    Gtk::MessageDialog* dialog = new Gtk::MessageDialog("Error", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE);
    dialog->set_secondary_text(msg);
    dialog->run();
    delete dialog;
}


void
GlSphereView::time_format_changed(Gtk::Entry *time_format)
{
    m_config->setTimeFormat(time_format->get_text());
    m_naviGlArea->queue_draw();
}

void
GlSphereView::weather_transparency_changed(Gtk::Scale *weather_transparency)
{
    double transparency = weather_transparency->get_value();
    m_config->setWeatherTransparency(transparency);
    //apply_weather_alpha(m_weather_pix); // now apply alpha for all pixels...
    //update_weather_tex(); // not needed as we do not recalculate alpha values
    m_naviGlArea->queue_draw();
}


double
GlSphereView::julianDate()
{
    // https://www.subsystems.us/uploads/9/8/9/4/98948044/moonphase.pdf
    Glib::DateTime date = Glib::DateTime::create_now_utc();
    //Glib::DateTime date = Glib::DateTime::create_utc(2017, 3, 1, 0, 0, 0);
    int Y = date.get_year();
    int M = date.get_month();
    int D = date.get_day_of_month();
    if (M < 3) {
        Y = Y - 1;
        M = M + 12;
    }
    int A = Y / 100;
    int B = A / 4;
    int C = 2 - A + B;
    double E = std::floor(365.25 * (double)(Y+4716));
    double F = std::floor(30.6001 * (double)(M+1));
    double JD = C + D + E + F - 1524.5;
    //double t = (double)(date.get_hour() * 60 + date.get_minute()) / (24.0 * 60.0);
    //JD += t;
    return JD;
}

// 0 new ... 0.5 full ... 1 new
double
GlSphereView::moonPhase()
{
    // https://www.subsystems.us/uploads/9/8/9/4/98948044/moonphase.pdf

    double synMoon = 29.530589;
    double JD = julianDate();
    double daySinceNew = JD - 2451549.5;
    //double newMoons = daySinceNew / synMoon;
    double phaseMoon = std::fmod(daySinceNew, synMoon);
    return phaseMoon / synMoon;
}

void
GlSphereView::calcuateMoonLight()
{
    double moonPh = moonPhase();
    float r = (moonPh * 2.0f * M_PI);
    float x = -std::sin(r);
    float y = 0.0f;
    float z = std::cos(r);
    m_moonLight = Vector(x, y, z);
}

std::string
RESOURCE::resource(const char *file)
{
    return std::string(RESOURCE::PREFIX) + std::string(file);
}

