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

#include <gtkmm.h>
#include <memory>

#include "GlSphereView.hpp"
#include "NaviGlArea.hpp"
#include "Config.hpp"
#include "Timer.hpp"

class GlGlobeWindow : public Gtk::ApplicationWindow {
public:
    GlGlobeWindow();
    virtual ~GlGlobeWindow() = default;

    void on_action_preferences();
    void on_action_about();
    void on_action_weather(const Glib::ustring& id, bool add);
    void on_action_timer();
    void save_config();
    void showMessage(const Glib::ustring& msg, Gtk::MessageType msgType = Gtk::MessageType::MESSAGE_INFO);

private:
    void closeConfigDlg();
    GlSphereView *m_sphereView;
    std::shared_ptr<Config> m_config;
    ConfigDialog *m_cfgdlg{nullptr};
};
