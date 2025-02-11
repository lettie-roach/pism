/* Copyright (C) 2020, 2021 PISM Authors
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
#ifndef PISM_ICEMODELVEC2V_H
#define PISM_ICEMODELVEC2V_H

#include "pism/util/IceModelVec2.hh"

namespace pism {

/** Class for storing and accessing 2D vector fields
*/
class IceModelVec2V : public IceModelVec2<Vector2> {
public:
  IceModelVec2V(IceGrid::ConstPtr grid, const std::string &short_name,
                IceModelVecKind ghostedp, unsigned int stencil_width = 1);
  virtual ~IceModelVec2V() = default;

  typedef std::shared_ptr<IceModelVec2V> Ptr;
  typedef std::shared_ptr<const IceModelVec2V> ConstPtr;
};

std::shared_ptr<IceModelVec2V> duplicate(const IceModelVec2V &source);

} // end of namespace pism

#endif /* PISM_ICEMODELVEC2V_H */
