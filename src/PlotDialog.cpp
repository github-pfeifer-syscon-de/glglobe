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

#include <glm/trigonometric.hpp>
#include <iostream>
#include <StringUtils.hpp>
#include <psc_i18n.hpp>
#include <psc_format.hpp>


#include "PlotDialog.hpp"
#include "Moon.hpp"
#include "SunSet.hpp"

PlotDialog::PlotDialog(BaseObjectType* cobject
                    , const Glib::RefPtr<Gtk::Builder>& builder)
: Gtk::Dialog(cobject)
{
    builder->get_widget("grid", m_grid);
    builder->get_widget("col1", m_col1);
    builder->get_widget("col2", m_col2);
    builder->get_widget("col3", m_col3);
    builder->get_widget("apply", m_apply);
    builder->get_widget("max", m_max);
    builder->get_widget("scroll", m_scroll);
    builder->get_widget_derived<psc::ui::PlotDrawing>("drawing", m_drawing);

    m_apply->signal_clicked().connect(sigc::mem_fun(*this, &PlotDialog::apply));
    m_max->signal_value_changed().connect(sigc::mem_fun(*this, &PlotDialog::apply));
    apply();
}

void
PlotDialog::apply()
{
    auto& xAxis = m_drawing->getXAxis();
    auto min = 0.0;
    auto max = m_max->get_value();
    xAxis.setMinMax(min, max);
    std::vector<std::shared_ptr<psc::ui::PlotView>> views;
    try {
        auto exprPhase = createExpression("Phase", min, max, [](double jd) -> double
        {
            auto p = glm::degrees(Moon::moonPhase(jd) - glm::pi<double>());
            return p;
        });
        exprPhase->setPlotColor(m_col1->get_rgba());
        views.push_back(exprPhase);
        m_drawing->getXAxis().setGrid(exprPhase);

        auto exprIllum = createExpression("Illum", min, max, [](double jd) -> double
        {
            auto i = Moon::moonPhase(jd);
            return (Moon::getIlluminated(i) - 0.5) * 360.0;   // scale to angular values
        });
        exprIllum->setPlotColor(m_col2->get_rgba());
        views.push_back(exprIllum);

        auto exprDec = createExpression("Declination", min, max, [](double jd) -> double
        {
            //Glib::DateTime dateLocal = Glib::DateTime::create_now_local();
            //Glib::TimeSpan timeSpan = dateLocal.get_utc_offset();
            //double offsetUtcH = (double)(timeSpan) / (1.0e6 * 3600.0); // us -> h
            //std::cout << "offsetUtcH " << offsetUtcH << std::endl;
            //Glib::DateTime dateUtc = Glib::DateTime::create_now_utc();
            //float t = (float)(dateUtc.get_hour() * 60 + dateUtc.get_minute()) / (24.0f * 60.0f);
            //float d = (float)dateUtc.get_day_of_year();   // approximate season
            //std::cout <<  "s: " << s << " t: " << t << std::endl;
            //std::cout <<  "dayOfYear: " << d << " utcHour: " << date.get_hour() << std::endl;
            //float r =  - t * 2.0f * (float)M_PI;
            SunSet sunSet; // (config->getLatitude(), config->getLongitude(), offsetUtcH);
            //sunSet.setCurrentDate(dateLocal.get_year(), dateLocal.get_month(), dateLocal.get_day_of_month());
            auto t = sunSet.calcTimeJulianCent(jd);
            auto declination = sunSet.calcSunDeclination(t);    // keep at actual value
            return declination;
        });
        exprDec->setPlotColor(m_col3->get_rgba());
        views.push_back(exprDec);

        m_drawing->setPlot(views);
        m_drawing->refresh();
    }
    catch (const std::exception& exc) {
            auto msg = exc.what();
            show_error(psc::fmt::vformat(
                        _("Error evaluating {}" ),
                          psc::fmt::make_format_args(msg)));
    }
}


void
PlotDialog::show_error(const Glib::ustring& msg, Gtk::MessageType type)
{
    Gtk::MessageDialog messagedialog(*this, msg, false, type);
    messagedialog.run();
    messagedialog.hide();
}



std::shared_ptr<PlotExpression>
PlotDialog::createExpression(const Glib::ustring& lbl, double min, double max, const std::function<double(double)>& func)
{
    std::vector<double> values;
    values.reserve(static_cast<size_t>(max));
    for (int i = min; i <= max; ++i) {
        Glib::DateTime now = Glib::DateTime::create_now_utc();
        now = now.add_days(i);
        double jd = Moon::asJulianDate(now);
        auto r = func(jd);
        values.push_back(r);
    }
    return std::make_shared<PlotExpression>(lbl, values);
}



PlotExpression::PlotExpression(const Glib::ustring& lbl, std::vector<double> values)
: psc::ui::PlotDiscrete(values)
, m_lbl{lbl}
{
}

void
PlotExpression::showGrid(const Cairo::RefPtr<Cairo::Context>& ctx
                       , psc::ui::PlotDrawing* plotDrawing
                       , psc::ui::PlotAxis& majorAxis
                       , psc::ui::PlotAxis& minorAxis)
{
    for (size_t n = 0; n < m_values.size(); n += 10) {
        auto xPix = majorAxis.toPixel(static_cast<double>(n));
        ctx->set_source_rgb(plotDrawing->gridColor.get_red(), plotDrawing->gridColor.get_green(), plotDrawing->gridColor.get_blue());
        ctx->move_to(xPix, 0.0);
        ctx->line_to(xPix, minorAxis.getPixel());
        ctx->stroke();
        ctx->set_source_rgb(plotDrawing->textColor.get_red(), plotDrawing->textColor.get_green(), plotDrawing->textColor.get_blue());
        ctx->move_to(xPix, minorAxis.getPixel());
        Glib::DateTime now = Glib::DateTime::create_now_local();
        now = now.add_days(n);
        auto lbl = now.format("%x"); // Glib::ustring::sprintf("%d.%d", now.get_day_of_month(), now.get_month());
        ctx->show_text(lbl);
    }
}

