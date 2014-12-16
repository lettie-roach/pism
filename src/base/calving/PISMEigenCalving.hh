/* Copyright (C) 2013, 2014 PISM Authors
 *
 * This file is part of PISM.
 *
 * PISM is free software; you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 3 of the License, or (at your option) any later
 * version.
 *
 * PISM is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PISM; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#ifndef _PISMEIGENCALVING_H_
#define _PISMEIGENCALVING_H_

#include "iceModelVec.hh"
#include "PISMComponent.hh"

namespace pism {

class StressBalance;

class EigenCalving : public Component
{
public:
  EigenCalving(IceGrid &g,
               StressBalance *stress_balance);
  virtual ~EigenCalving();

  virtual void init(Vars &vars);
  virtual void update(double dt,
                      IceModelVec2Int &pism_mask,
                      IceModelVec2S &Href,
                      IceModelVec2S &ice_thickness);

  virtual void max_timestep(double my_t, double &my_dt, bool &restrict);

  // empty methods that we're required to implement:
  virtual void add_vars_to_output(const std::string &keyword, std::set<std::string> &result);
  virtual void define_variables(const std::set<std::string> &vars, const PIO &nc,
                                IO_Type nctype);
  virtual void write_variables(const std::set<std::string> &vars, const PIO& nc);
protected:
  IceModelVec2 m_strain_rates;
  IceModelVec2S m_thk_loss;
  const int m_stencil_width;
  IceModelVec2Int *m_mask;
  StressBalance *m_stress_balance;
  double m_K;
  bool m_restrict_timestep;

  void update_strain_rates();
  void remove_narrow_tongues(IceModelVec2Int &pism_mask,
                             IceModelVec2S &ice_thickness);
};


} // end of namespace pism

#endif /* _PISMEIGENCALVING_H_ */
