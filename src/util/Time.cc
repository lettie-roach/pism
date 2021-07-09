// Copyright (C) 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2019, 2020, 2021 Constantine Khroulev
//
// This file is part of PISM.
//
// PISM is free software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the Free Software
// Foundation; either version 3 of the License, or (at your option) any later
// version.
//
// PISM is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
// details.
//
// You should have received a copy of the GNU General Public License
// along with PISM; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#include <cmath>

#include "Time.hh"

#include "ConfigInterface.hh"
#include "Time_Calendar.hh"
#include "pism_options.hh"
#include "pism_utilities.hh"
#include "error_handling.hh"
#include "pism/util/io/File.hh"
#include "pism/util/Logger.hh"

namespace pism {

//! Convert model years into seconds using the year length
//! corresponding to the current calendar.
/*! Do not use this to convert quantities other than time intervals!
 */
double Time::years_to_seconds(double input) const {
  return input * m_year_length;
}

//! Convert seconds into model years using the year length
//! corresponding to the current calendar.
/*! Do not use this to convert quantities other than time intervals!
 */
double Time::seconds_to_years(double input) const {
  return input / m_year_length;
}


Time::Time(Config::ConstPtr conf,
           units::System::Ptr unit_system)
  : m_config(conf),
    m_unit_system(unit_system),
    m_time_units(m_unit_system, "seconds") {
}

void Time::init_calendar(const std::string &calendar_string) {

  if (not pism_is_valid_calendar_name(calendar_string)) {
    throw RuntimeError::formatted(PISM_ERROR_LOCATION,
                                  "unsupported calendar: %s", calendar_string.c_str());
  }

  m_calendar_string = calendar_string;

  double seconds_per_day = convert(m_unit_system, 1.0, "day", "seconds");
  if (calendar_string == "360_day") {
    m_year_length = 360 * seconds_per_day;
  } else if (member(calendar_string, {"365_day", "noleap"})) {
    m_year_length = 365 * seconds_per_day;
  } else {
    // use the ~365.2524-day year
    m_year_length = convert(m_unit_system, 1.0, "year", "seconds");
  }
}

//! \brief Sets the current time (in seconds since the reference time).
void Time::set(double new_time) {
  m_time_in_seconds = new_time;
}

void Time::set_start(double new_start) {
  m_run_start = new_start;
}

void Time::set_end(double new_end) {
  m_run_end = new_end;
}

//! \brief Current time, in seconds.
double Time::current() const {
  return m_time_in_seconds;
}

double Time::start() const {
  return m_run_start;
}

double Time::end() const {
  return m_run_end;
}

std::string Time::units_string() const {
  return m_time_units.format();
}

//! \brief Returns the calendar string.
std::string Time::calendar() const {
  return m_calendar_string;
}

void Time::step(double delta_t) {
  m_time_in_seconds += delta_t;

  // If we are less than 0.001 second from the end of the run, reset
  // m_time_in_seconds to avoid taking a very small (and useless) time step.
  if (m_run_end > m_time_in_seconds &&
      m_run_end - m_time_in_seconds < 1e-3) {
    m_time_in_seconds = m_run_end;
  }
}

bool Time::process_y(double &result) {
  options::Real y(m_unit_system,
                  "-y", "Run length, in years",
                  m_config->units("time.run_length"),
                  m_config->get_number("time.run_length"));
  result = years_to_seconds(y);
  return y.is_set();
}

bool Time::process_ye(double &result) {
  options::Real ye(m_unit_system,
                   "-ye", "End year",
                   "365days",
                   m_config->get_number("time.start_year", "365days") +
                   m_config->get_number("time.run_length", "365days"));
  result = years_to_seconds(ye);
  return ye.is_set();
}

std::string Time::date(double T) const {
  return pism::printf("%.3f", seconds_to_years(T));
}

std::string Time::date() const {
  return date(current());
}

std::string Time::start_date() const {
  return date(m_run_start);
}

std::string Time::end_date() const {
  return date(m_run_end);
}

std::string Time::run_length() const {
  return pism::printf("%3.3f", seconds_to_years(m_run_end - m_run_start));
}

double Time::mod(double time, unsigned int period_years) const {
  if (period_years == 0) {
    return time;
  }

  double period_seconds = years_to_seconds(period_years);

  double tmp = time - floor(time / period_seconds) * period_seconds;

  if (fabs(tmp - period_seconds) < 1) {
    tmp = 0;
  }

  return tmp;
}

double Time::year_fraction(double T) const {
  double Y = seconds_to_years(T);
  return Y - floor(Y);
}

double Time::day_of_the_year_to_day_fraction(unsigned int day) const {
  const double sperd = 86400.0;
  return (sperd / m_year_length) * (double) day;
}

double Time::calendar_year_start(double T) const {
  return T - this->mod(T, 1);
}

double Time::increment_date(double T, double years) const {
  return T + years_to_seconds(years);
}

std::vector<double> Time::parse_times(const std::string &spec) const {
  if (spec.find(',') != std::string::npos) {
    // a list will always contain a comma because at least two numbers are
    // needed to specify reporting intervals
    return parse_list(spec);
  } else {
    // it must be a range specification
    return parse_range(spec);
  }
}

std::vector<double> Time::parse_list(const std::string &spec) const {
  std::vector<double> result;

  try {
    for (const auto &s : split(spec, ',')) {
      result.emplace_back(parse_date(s));
    }
  } catch (RuntimeError &e) {
      e.add_context("parsing a list of dates %s", spec.c_str());
      throw;
  }

  return result;
}

/**
 * Parses an interval specification string.
 *
 * @param[in] spec specification string
 *
 */
auto Time::parse_interval_length(const std::string &spec) const -> Interval {

  // check if it is a keyword
  if (spec == "hourly") {
    return {3600.0, SIMPLE};
  }

  if (spec == "daily") {
    return {86400.0, SIMPLE};
  }

  if (spec == "monthly") {
    return {1.0, MONTHLY};
  }

  if (spec == "yearly") {
    return {1.0, YEARLY};
  }

  try {
    units::Unit seconds(m_unit_system, "seconds"),
      one(m_unit_system, "1"),
      tmp(m_unit_system, spec);

    // Check if these units are compatible with "seconds" or "1". The
    // latter allows intervals of the form "0.5", which stands for "half
    // of a model year". This also discards interval specs such as "days
    // since 1-1-1", even though "days" is compatible with "seconds".
    if (units::are_convertible(tmp, seconds)) {
      units::Converter c(tmp, seconds);

      return {c(1.0), SIMPLE};
    } else if (units::are_convertible(tmp, one)) {
      units::Converter c(tmp, one);

      // convert from years to seconds without using UDUNITS-2 (this
      // way we handle 360-day and 365-day years correctly)
      return {years_to_seconds(c(1.0)), SIMPLE};
    }
  } catch (RuntimeError &e) {
    e.add_context("processing interval length " + spec);
    throw;
  }

  throw RuntimeError::formatted(PISM_ERROR_LOCATION,
                                "interval length '%s' is invalid", spec.c_str());
}


std::vector<double> Time::parse_range(const std::string &spec) const {
  double
    time_start   = m_run_start,
    time_end     = m_run_end;

  Interval I{0.0, SIMPLE};

  if (spec == "hourly") {
    I = {3600.0, SIMPLE};
  } else if (spec == "daily") {
    I = {86400.0, SIMPLE};
  } else if (spec == "monthly") {
    I = {1.0, MONTHLY};
  } else if (spec == "yearly") {
    I = {1.0, YEARLY};
  } else {

    auto parts = pism::split(spec, ':');

    if (parts.size() == 1) {
      I = parse_interval_length(parts[0]);

    } else if (parts.size() == 3) {
      time_start = parse_date(parts[0]);
      I          = parse_interval_length(parts[1]);
      time_end   = parse_date(parts[2]);
    } else {
      throw RuntimeError::formatted(PISM_ERROR_LOCATION,
                                    "a time range must consist of exactly 3 parts separated by colons (got '%s').",
                                    spec.c_str());
    }
  }

  std::vector<double> result;
  compute_times(time_start, time_end, I, result);
  return result;
}


double Time::parse_date(const std::string &spec) const {

  if (spec.empty()) {
    throw RuntimeError(PISM_ERROR_LOCATION, "got an empty date specification");
  }

  char *endptr = NULL;
  double d = strtod(spec.c_str(), &endptr);
  if (*endptr != '\0') {
    throw RuntimeError::formatted(PISM_ERROR_LOCATION, "date specification '%s' is invalid ('%s' is not an number)",
                                  spec.c_str(), spec.c_str());
  }

  return years_to_seconds(d);
}


/**
 * Compute times corresponding to a "simple" time range.
 *
 * This is a time range with a fixed step ("hourly" is an example).
 *
 * @param[in] time_start beginning of the time interval, in seconds
 * @param[in] delta step, in seconds
 * @param[in] time_end end of the interval, in seconds
 * @param[out] result list of model times
 *
 */
void Time::compute_times_simple(double time_start, double delta, double time_end,
                                std::vector<double> &result) const {
  if (time_start >= time_end) {
    throw RuntimeError::formatted(PISM_ERROR_LOCATION, "a >= b in time range a:dt:b (got a = %s, b = %s)",
                                  this->date(time_start).c_str(), this->date(time_end).c_str());
  }

  if (delta <= 0) {
    throw RuntimeError::formatted(PISM_ERROR_LOCATION, "dt <= 0 in time range a:dt:b (got dt = %f seconds)", delta);
  }

  int k = 0;
  double t = time_start;

  result.clear();
  do {
    if (t >= this->start() && t <= this->end()) {
      result.push_back(t);
    }
    k += 1;
    t = time_start + k * delta;
  } while (t <= time_end);
}

void Time::compute_times(double time_start, double time_end,
                         const Interval &interval,
                         std::vector<double> &result) const {
  double delta = interval.dt;
  switch (interval.type) {
  case YEARLY:
    delta = years_to_seconds(1.0);
    break;
  case MONTHLY:
    delta = years_to_seconds(1.0/12.0);
    break;
  case SIMPLE:
    break;
  default:
    throw RuntimeError::formatted(PISM_ERROR_LOCATION,
                                  "unknown time range type: %d",
                                  interval.type);
  }

  compute_times_simple(time_start, delta, time_end, result);
}

double Time::convert_time_interval(double T, const std::string &units) const {
  if (member(units, {"year", "years", "yr", "a"})) {
    return this->seconds_to_years(T); // uses year length here
  }
  return convert(m_unit_system, T, "seconds", units);
}

double Time::current_years() const {
  return seconds_to_years(current());
}

} // end of namespace pism
