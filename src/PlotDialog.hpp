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

#include <gtkmm.h>
#include <Plot.hpp>
#include <map>
#include <functional>

class PlotExpression
: public psc::ui::PlotDiscrete
{
public:
    PlotExpression(const Glib::ustring& lbl, std::vector<double> values);
    ~PlotExpression() = default;

    Glib::ustring getLabel(size_t idx) override;

protected:
    Glib::ustring m_lbl;
};

class PlotDialog
: public Gtk::Dialog
{
public:
    PlotDialog(BaseObjectType* cobject
            , const Glib::RefPtr<Gtk::Builder>& builder);
    explicit PlotDialog(const PlotDialog& orig) = delete;
    virtual ~PlotDialog() = default;

    void apply();

protected:
    void show_error(const Glib::ustring& msg
            , Gtk::MessageType type = Gtk::MessageType::MESSAGE_ERROR);
    std::shared_ptr<PlotExpression> createExpression(
        const Glib::ustring& lbl, double min, double max, const std::function<double(double)>& func);

private:
    Gtk::ScrolledWindow* m_scroll;
    Gtk::Grid* m_grid;
    Gtk::ColorButton* m_col1;
    Gtk::ColorButton* m_col2;
    Gtk::ColorButton* m_col3;
    Gtk::Button* m_apply;
    Gtk::SpinButton* m_max;
    psc::ui::PlotDrawing* m_drawing;
};

