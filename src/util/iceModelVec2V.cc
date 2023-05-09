// Copyright (C) 2009--2017, 2020, 2021 Constantine Khroulev
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

#include <memory>               // std::dynamic_pointer_cast

#include "IceModelVec2V.hh"
#include "IceModelVec_impl.hh"

#include "pism_utilities.hh"
#include "IceGrid.hh"

#include "error_handling.hh"    // RuntimeError

#include "pism/util/Context.hh"
#include "pism/util/VariableMetadata.hh"

namespace pism {

IceModelVec2V::IceModelVec2V(IceGrid::ConstPtr grid, const std::string &short_name,
                             IceModelVecKind ghostedp, unsigned int stencil_width)
  : IceModelVec2<Vector2>(grid, short_name, ghostedp, stencil_width) {

  auto sys = m_impl->grid->ctx()->unit_system();

  m_impl->metadata.clear();
  for (const auto* prefix : {"u", "v"}) {
    m_impl->metadata.emplace_back(SpatialVariableMetadata{sys, prefix + short_name});
  }

  set_name("vel" + short_name);
}

std::shared_ptr<IceModelVec2V> duplicate(const IceModelVec2V &source) {

  auto result = std::make_shared<IceModelVec2V>(source.grid(),
                                                source.get_name(),
                                                WITHOUT_GHOSTS,
                                                1);
  result->metadata(0) = source.metadata(0);
  result->metadata(1) = source.metadata(1);

  return result;
}

} // end of namespace pism
