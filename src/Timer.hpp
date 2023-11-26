/* -*- Mode: c++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
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

#pragma once

#include <gtkmm.h>
#include <chrono>

class Config;

class Timer
 : public Gtk::Dialog
{
public:
    Timer(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, Config* config);
    explicit Timer(const Timer& orig) = delete;
    virtual ~Timer();


protected:
    bool parseTimerValue();
    bool parseTimeValue();
    void startTimer();
    bool timeout();
    bool timerTimeout();
    bool timeTimeout();

private:
    bool m_timerRunning{false};
    bool m_timeRunning{false};
    Gtk::Entry* m_timerValue{nullptr};
    Gtk::Entry* m_timeValue{nullptr};
    Gtk::Button* m_startButton{nullptr};
    Gtk::Notebook* m_notebook{nullptr};
    sigc::connection m_timer;
    std::chrono::time_point<std::chrono::steady_clock> m_delay;
    std::chrono::time_point<std::chrono::system_clock> m_time;
    Config* m_config{nullptr};
};

