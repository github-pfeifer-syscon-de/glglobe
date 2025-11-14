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

#include <iostream>
#include <charconv>
#include <optional>
#include <system_error>
#include <StringUtils.hpp>
#include <psc_i18n.hpp>
#include <psc_format.hpp>


#include "PlotDialog.hpp"
#include "Moon.hpp"

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
            auto p = Moon::moonPhase(jd);
            return p;
        });
        exprPhase->setPlotColor(m_col1->get_rgba());
        views.push_back(exprPhase);
        m_drawing->getXAxis().setGrid(exprPhase);

        auto exprIllum = createExpression("Illum", min, max, [](double jd) -> double
        {
            auto i = Moon::moonPhase(jd);
            return Moon::getIlluminated(i) * 2.0 * glm::pi<double>();   // scale to angular values
        });
        exprIllum->setPlotColor(m_col2->get_rgba());
        views.push_back(exprIllum);

        //auto expr = createExpression("Legacy", min, max, [](double jd) -> double
        //{
        //    auto i = Moon::moonPhase(jd);
        //    auto p = std::fmod(i + glm::pi<double>(), 2.0 * glm::pi<double>());
        //    return (p);
        //});
        //expr->setPlotColor(m_col3->get_rgba());
        //views.push_back(expr);

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
    Glib::DateTime now = Glib::DateTime::create_now_local();
    for (size_t n = 0; n < m_values.size(); ++n) {
        if ((n % 10) == 0) {
            auto xPix = majorAxis.toPixel(static_cast<double>(n));
            ctx->set_source_rgb(plotDrawing->gridColor.get_red(), plotDrawing->gridColor.get_green(), plotDrawing->gridColor.get_blue());
            ctx->move_to(xPix, 0.0);
            ctx->line_to(xPix, minorAxis.getPixel());
            ctx->stroke();
            ctx->set_source_rgb(plotDrawing->textColor.get_red(), plotDrawing->textColor.get_green(), plotDrawing->textColor.get_blue());
            ctx->move_to(xPix, minorAxis.getPixel());
            auto lbl = now.format("%x"); // Glib::ustring::sprintf("%d.%d", now.get_day_of_month(), now.get_month());
            ctx->show_text(lbl);
        }
        now = now.add_days(1);
    }
}

