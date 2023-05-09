// Copyright (C) 2011, 2013, 2014, 2016, 2017, 2020, 2021 PISM Authors
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

#ifndef _ICEMODELVEC_HELPERS_H_
#define _ICEMODELVEC_HELPERS_H_

namespace pism {

namespace vec {

namespace details {

void compute_params(const IceModelVec* const x, const IceModelVec* const y,
                    const IceModelVec* const z, int &ghosts, bool &scatter);

} // end of namespace details

//! \brief Computes result = x + alpha * y, where x, y, and z are 2D
//! IceModelVecs (scalar or vector).
/*!
 */
template<class V>
void add(const V &x, double alpha, const V &y, V &result, bool scatter=true) {

  IceModelVec::AccessList list{&x, &y, &result};
  for (Points p(*result.grid()); p; p.next()) {
    const int i = p.i(), j = p.j();

    result(i, j) = x(i, j) + y(i, j) * alpha;
  }

  if (scatter) {
    result.update_ghosts();
  }

  result.inc_state_counter();
}

template<class V>
void copy(const V& source, V& destination, bool scatter=true) {

  IceModelVec::AccessList list{&source, &destination};

  for (Points p(*destination.grid()); p; p.next()) {
    const int i = p.i(), j = p.j();

    destination(i, j) = source(i, j);
  }

  if (scatter) {
    destination.update_ghosts();
  }

  destination.inc_state_counter();
}

} // end of namespace vec

} // end of namespace pism

#endif /* _ICEMODELVEC_HELPERS_H_ */
