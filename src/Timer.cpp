/* -*- Mode: c++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
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

#include <iostream>
#include <format>
#include <ctime>

#include "Config.hpp"
#include "Timer.hpp"

Timer::Timer(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, Config* config)
: Gtk::Dialog(cobject)
, m_config{config}
{
    refBuilder->get_widget("timerValue", m_timerValue);
    refBuilder->get_widget("timeValue", m_timeValue);
    refBuilder->get_widget("startButton", m_startButton);
    refBuilder->get_widget("notebook", m_notebook);
    if (m_startButton) {
        m_startButton->signal_clicked().connect([&] {
            if (m_notebook->get_current_page() == 0) {
                m_timerRunning = parseTimerValue();
            }
            else if (m_notebook->get_current_page() == 1) {
                m_timeRunning = parseTimeValue();
            }
            if (m_timerRunning || m_timeRunning) {
                startTimer();
            }
        });
    }
    else {
        std::cout << "The expected startButton was not found!" << std::endl;
    }
    if (m_config) {
        m_timerValue->set_text(m_config->getTimerValue());
        m_timeValue->set_text(m_config->getTimeValue());
    }
}

Timer::~Timer()
{
    std::cout << __FILE__ << "::~Timer" << std::endl;
    if (m_timer.connected()) {
        m_timer.disconnect(); // No more updating
    }
}

bool
Timer::parseTimerValue()
{
    auto timeValue = m_timerValue->get_text();
    try {
        int minutes = 0;
        auto pos = timeValue.find(':');
        if (pos != timeValue.npos) {
            minutes = std::stoi(timeValue.substr(0, pos));
            ++pos;
            timeValue = timeValue.substr(pos);
        }
        int seconds = 0;
        if (!timeValue.empty()) {
            seconds = std::stoi(timeValue);
        }
        if (minutes > 0
         || seconds > 0) {
            if (m_config) {
                m_config->setTimerValue(m_timerValue->get_text());
                m_config->save();
            }
            m_delay = std::chrono::steady_clock::now();
            m_delay += std::chrono::minutes(minutes);
            m_delay += std::chrono::seconds(seconds);
            return true;
        }
    }
    catch (std::invalid_argument const& ex) {
        Gtk::MessageDialog dialog = Gtk::MessageDialog("Invalid value", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE);
        dialog.set_secondary_text(std::format("The value {} was not understood, please use 01:23 for a 1 minute and 23 seconds delay!", static_cast<std::string>(timeValue)));
        dialog.run();
    }
    return false;
}

bool
Timer::parseTimeValue()
{
    auto timeValue = m_timeValue->get_text();
    try {
        int hour = 0;
        auto pos = timeValue.find(':');
        if (pos != timeValue.npos) {
            hour = std::stoi(timeValue.substr(0, pos));
            ++pos;
            timeValue = timeValue.substr(pos);
        }
        int minutes = 0;
        if (!timeValue.empty()) {
            minutes = std::stoi(timeValue);
        }
        if (hour > 0
         || minutes > 0) {
            if (m_config) {
                m_config->setTimeValue(m_timeValue->get_text());
                m_config->save();
            }
            using chronoMinutes = std::chrono::duration<uint64_t, std::ratio<60>>;
            using chronoHours = std::chrono::duration<uint64_t, std::ratio<60*60>>;
            using chronoDays = std::chrono::duration<uint64_t, std::ratio<24*60*60>>;    // 86400
            auto last_midnight = std::chrono::time_point_cast<chronoDays>(std::chrono::system_clock::now());
            //std::cout << __FILE__ << "::parseTimeValue"
            //          << " midnight " << last_midnight
            //          << " offs " << localZone->get_info(std::chrono::system_clock::now()).offset
            //          << std::endl;
            m_time = std::chrono::time_point<std::chrono::system_clock>(last_midnight);
            m_time += chronoHours{hour};
            m_time += chronoMinutes{minutes};
            auto localZone = std::chrono::current_zone();   // as our input counts as local, need to adjust to utc
            m_time -= localZone->get_info(std::chrono::system_clock::now()).offset;
            return true;
        }
    }
    catch (std::invalid_argument const& ex) {
        Gtk::MessageDialog dialog = Gtk::MessageDialog("Invalid value", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_CLOSE);
        dialog.set_secondary_text(std::format("The value {} was not understood, please use 11:45 for a reminder at 11 o'clock and 45 minutes!", static_cast<std::string>(timeValue)));
        dialog.run();
    }
    return false;
}


void
Timer::startTimer()
{
    Glib::DateTime date = Glib::DateTime::create_now_utc();
    auto ms{1000u - static_cast<uint32_t>(std::fmod(date.get_seconds(), 1.0) * 1000.0)};    // try to hit next second change
    m_timer = Glib::signal_timeout().connect(sigc::mem_fun(*this, &Timer::timeout), ms);
}

bool
Timer::timerTimeout()
{
    auto now = std::chrono::steady_clock::now();
    auto elapsed = m_delay - now;
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed);
    auto sec{seconds.count()};
    //std::cout << __FILE__ << "::timerTimeout elapsed " << elapsed
    //          << " seconds " << seconds
    //          << " sec " << sec
    //          << std::endl;
    Glib::ustring remain;
    auto minutes{sec / 60};
    sec -= minutes * 60;
    if (minutes > 0 || sec > 0) {
        std::format_to(std::back_inserter(remain), "{:02}:{:02}", minutes, sec);
        m_timerValue->set_text(remain);
        return true;
    }
    else {
        m_timerValue->set_text(m_config->getTimerValue());
        get_window()->beep();
        Gtk::MessageDialog dialog = Gtk::MessageDialog("Timeout", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_CLOSE);
        dialog.set_secondary_text("Time expired!");
        dialog.run();
    }
    return false;
}

bool
Timer::timeTimeout()
{
    auto now = std::chrono::system_clock::now();
    auto elapsed = m_time - now;
    auto hours = std::chrono::duration_cast<std::chrono::hours>(elapsed);
    auto minutes = std::chrono::duration_cast<std::chrono::minutes>(elapsed);
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed);
    //std::cout << __FILE__ << "::timeTimeout"
    //          << " time " << m_time
    //          << " now " << now
    //          << " elapsed " << elapsed
    //          << " seconds " << seconds
    //          << std::endl;
    auto h = hours.count();
    auto m = minutes.count() - h * 60;
    auto s = seconds.count() - minutes.count() * 60;
    if (h > 0 || m > 0 ||s > 0) {
        Glib::ustring remain;
        std::format_to(std::back_inserter(remain), "{:02}:{:02}:{:02}", h, m, s);
        m_timeValue->set_text(remain);
        return true;
    }
    else {
        m_timeValue->set_text(m_config->getTimeValue());
        get_window()->beep();
        Gtk::MessageDialog dialog = Gtk::MessageDialog("Reminder", false, Gtk::MESSAGE_INFO, Gtk::BUTTONS_CLOSE);
        dialog.set_secondary_text("Time reached!");
        dialog.run();
    }
    return false;
}

bool
Timer::timeout()
{
    if (m_timerRunning) {
        m_timerRunning = timerTimeout();
    }
    if (m_timeRunning) {
        m_timeRunning = timeTimeout();
    }
    if (m_timerRunning || m_timeRunning) {
        startTimer();
    }
    return false;   // do not repeat was restarted
}

