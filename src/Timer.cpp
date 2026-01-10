/* -*- Mode: c++; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4; coding: utf-8; -*-  */
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

#include <iostream>
#include <psc_format.hpp>
#include <psc_i18n.hpp>
#include <ctime>

#include "Config.hpp"
#include "Timer.hpp"

Timer::Timer(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, const std::shared_ptr<Config>& config)
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
        showMessage(psc::fmt::vformat(
                      _("No \"{}\" object in {}")
                    , psc::fmt::make_format_args("startButton", "timer-dlg.ui"))
                    ,  Gtk::MessageType::MESSAGE_ERROR);
    }
    if (m_config) {
        m_timerValue->set_text(m_config->getTimerValue());
        m_timeValue->set_text(m_config->getTimeValue());
    }
}

Timer::~Timer()
{
    //std::cout << __FILE__ << "::~Timer" << std::endl;
    if (m_timer.connected()) {
        m_timer.disconnect(); // No more updating
    }
}

void
Timer::showMessage(const Glib::ustring& msg, Gtk::MessageType msgType)
{
    Gtk::MessageDialog messagedialog(msg, false, msgType);
    messagedialog.run();
    messagedialog.hide();
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
            return updateTimer(minutes, seconds);
        }
    }
    catch (std::invalid_argument const& ex) {
        showMessage(psc::fmt::vformat(
                _("The value {} was not understood, please use 1:23 for a 1 minute and 23 seconds delay!")
                , psc::fmt::make_format_args(timeValue))
                ,  Gtk::MessageType::MESSAGE_ERROR);
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
            return updateTime(hour, minutes);
        }
    }
    catch (std::invalid_argument const& ex) {
         showMessage(psc::fmt::vformat(
                 _("The value {} was not understood, please use 11:45 for a reminder at 11 o'clock and 45 minutes!")
                 , psc::fmt::make_format_args(timeValue))
                 ,  Gtk::MessageType::MESSAGE_ERROR);
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
Timer::timeout()
{
    if (m_timerRunning) {
        m_timerRunning = timerTimeout();
        if (!m_timerRunning) {
            m_timerValue->set_text(m_config->getTimerValue());
            get_window()->beep();
            showMessage(_("Timer expired!"));
        }
    }
    if (m_timeRunning) {
        m_timeRunning = timeTimeout();
        if (!m_timeRunning)  {
            m_timeValue->set_text(m_config->getTimeValue());
            get_window()->beep();
            showMessage(_("Time reached!"));
        }
    }
    if (m_timerRunning || m_timeRunning) {
        startTimer();
    }
    return false;   // do not repeat was restarted
}

TimerChrono::TimerChrono(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, const std::shared_ptr<Config>& config)
: Timer{cobject, refBuilder, config}
{
}

bool
TimerChrono::updateTime(int hours, int minutes)
{
    using chronoMicroSeconds = std::chrono::duration<uint64_t, std::ratio<1,1000000>>;
    using chronoMinutes = std::chrono::duration<uint64_t, std::ratio<60>>;
    using chronoHours = std::chrono::duration<uint64_t, std::ratio<60*60>>;
    // this seems to be correct, (even if i expected some leap second issues)
    using chronoDays = std::chrono::duration<uint64_t, std::ratio<24*60*60>>;    // 86400
    auto last_midnight = std::chrono::time_point_cast<chronoDays>(std::chrono::system_clock::now());
    //std::cout << __FILE__ << "::parseTimeValue"
    //          << " midnight " << last_midnight << std::endl;
    m_time = std::chrono::time_point<std::chrono::system_clock>(last_midnight);
    m_time += chronoHours{hours};
    m_time += chronoMinutes{minutes};
#if ___GNUC__ >= 13
    auto localZone = std::chrono::current_zone();   // as our input counts as local, need to adjust to utc
    m_time -= localZone->get_info(m_time).offset;
#else
    Glib::DateTime dtnow = Glib::DateTime::create_now_local();
    auto offs = dtnow.get_utc_offset();
    m_time -= chronoMicroSeconds(offs);
#endif
    auto now = std::chrono::system_clock::now();
    if (now > m_time) {
        m_time += chronoHours{24};
    }
    return true;
}

bool
TimerChrono::timeTimeout()
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
    //          << " seconds " << seconds << std::endl;
    auto h = hours.count();
    auto m = minutes.count() - h * 60l;
    auto s = seconds.count() - minutes.count() * 60l;
    if (h > 0 || m > 0 ||s > 0) {
        Glib::ustring remain{psc::fmt::format("{}:{:02}:{:02}", h, m, s)};
        m_timeValue->set_text(remain);
        return true;
    }
    return false;
}

bool
TimerChrono::updateTimer(int minutes, int seconds)
{
    m_delay = std::chrono::steady_clock::now();
    m_delay += std::chrono::minutes(minutes);
    m_delay += std::chrono::seconds(seconds);
    return true;
}

bool
TimerChrono::timerTimeout()
{
    auto now = std::chrono::steady_clock::now();
    auto elapsed = m_delay - now;
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed);
    auto sec{seconds.count()};
    //std::cout << __FILE__ << "::timerTimeout elapsed " << elapsed
    //          << " seconds " << seconds
    //          << " sec " << sec << std::endl;
    auto minutes{sec / 60l};
    sec -= minutes * 60l;
    if (minutes > 0 || sec > 0) {
        Glib::ustring remain{psc::fmt::format("{}:{:02}", minutes, sec)};
        m_timerValue->set_text(remain);
        return true;
    }
    return false;
}


TimerGlib::TimerGlib(BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& refBuilder, const std::shared_ptr<Config>& config)
: Timer{cobject, refBuilder, config}
{
}

bool
TimerGlib::updateTime(int hours, int minutes)
{
    auto now = Glib::DateTime::create_now_local();
    m_time = Glib::DateTime::create_local(now.get_year(), now.get_month(), now.get_day_of_month(), hours, minutes, 0.0);
    return true;
}

bool
TimerGlib::timeTimeout()
{
    auto now = Glib::DateTime::create_now_local();
    auto elapsed = m_time.difference(now);
    auto hours = elapsed / (60l * 60l) / GLIB_USEC;
    auto minutes = (elapsed / (60l * GLIB_USEC)) % 60l;
    auto seconds = (elapsed / (GLIB_USEC)) % 60l;
    //std::cout << __FILE__ << "::timeTimeout"
    //          << " time " << m_time.format_iso8601()
    //          << " now " << now.format_iso8601()
    //          << " elapsed " << elapsed
    //          << " seconds " << seconds << std::endl;
    if (hours > 0||minutes > 0 ||seconds > 0) {
        Glib::ustring remain;
        remain += Glib::ustring::sprintf("%d:%02d:%02d"
            , static_cast<int>(hours), static_cast<int>(minutes), static_cast<int>(seconds));
        m_timeValue->set_text(remain);
        return true;
    }
    return false;
}

bool
TimerGlib::updateTimer(int minutes, int seconds)
{
    m_delay = Glib::DateTime::create_now_local();
    m_delay = m_delay.add_minutes(minutes);
    m_delay = m_delay.add_seconds(seconds);
    return true;
}

bool
TimerGlib::timerTimeout()
{
    // monotonic would be better, but this matters only for daylight switching
    auto now = Glib::DateTime::create_now_local();
    auto elapsed = m_delay.difference(now);
    auto sec{elapsed / GLIB_USEC};
    //std::cout << __FILE__ << "::timerTimeout elapsed " << elapsed
    //          << " delay " << m_delay.format_iso8601()
    //          << " now " << now.format_iso8601()
    //          << " sec " << sec << std::endl;
    Glib::ustring remain;
    auto minutes{sec / 60l};
    sec -= minutes * 60l;
    if (minutes > 0 || sec > 0) {
        remain += Glib::ustring::sprintf("%d:%02d"
            , static_cast<int>(minutes), static_cast<int>(sec));
        m_timerValue->set_text(remain);
        return true;
    }
    return false;
}
