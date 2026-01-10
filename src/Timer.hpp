/* -*- Mode: c++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
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

#include <gtkmm.h>
#include <chrono>
#include <memory>

class Config;

class Timer
 : public Gtk::Dialog
{
public:
    Timer(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, const std::shared_ptr<Config>& config);
    explicit Timer(const Timer& orig) = delete;
    virtual ~Timer();


protected:
    bool parseTimerValue();
    bool parseTimeValue();
    void startTimer();
    bool timeout();
    virtual bool timerTimeout() = 0;
    virtual bool timeTimeout() = 0;
    virtual bool updateTime(int hour, int minutes) = 0;
    virtual bool updateTimer(int minute, int seconds) = 0;
    Gtk::Entry* m_timerValue{nullptr};
    Gtk::Entry* m_timeValue{nullptr};
    void showMessage(const Glib::ustring& msg, Gtk::MessageType msgType = Gtk::MessageType::MESSAGE_INFO);

private:
    bool m_timerRunning{false};
    bool m_timeRunning{false};
    Gtk::Button* m_startButton{nullptr};
    Gtk::Notebook* m_notebook{nullptr};
    sigc::connection m_timer;
    std::shared_ptr<Config> m_config;
};

// to compare the chrono vs. glib approach
class TimerChrono
: public Timer
{
public:
    TimerChrono(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, const std::shared_ptr<Config>& config);
    virtual ~TimerChrono() = default;
protected:
    bool timeTimeout() override;
    bool timerTimeout() override;
    bool updateTime(int hours, int minutes) override;
    bool updateTimer(int minute, int seconds) override;

private:
    std::chrono::time_point<std::chrono::steady_clock> m_delay;
    std::chrono::time_point<std::chrono::system_clock> m_time;

};

class TimerGlib
: public Timer
{
public:
    TimerGlib(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, const std::shared_ptr<Config>& config);
    virtual ~TimerGlib() = default;
protected:
    bool updateTime(int hours, int minutes) override;
    bool timeTimeout() override;
    bool updateTimer(int minutes, int seconds) override;
    bool timerTimeout() override;

private:
    Glib::DateTime m_delay;
    Glib::DateTime m_time;
    static constexpr auto GLIB_USEC = 1000000l;
};
