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

#include "BlatterTestCFBC.hh"

#include "pism/rheology/FlowLaw.hh"
#include "pism/stressbalance/StressBalance.hh"
#include "pism/geometry/Geometry.hh"
#include "manufactured_solutions.hh"

namespace pism {
namespace stressbalance {

BlatterTestCFBC::BlatterTestCFBC(IceGrid::ConstPtr grid, int Mz, int coarsening_factor)
  : Blatter(grid, Mz, coarsening_factor) {

  assert(m_flow_law->exponent() == 1.0);

  m_B = m_flow_law->hardness(1e5, 0.0);

  m_g = m_config->get_number("constants.standard_gravity");

  m_rho_i = m_config->get_number("constants.ice.density");

  m_rho_w = m_config->get_number("constants.sea_water.density");

  m_L = 2.0 * m_grid->Lx();
}

bool BlatterTestCFBC::dirichlet_node(const DMDALocalInfo &info,
                                     const fem::Element3::GlobalIndex& I) {
  (void) info;
  return I.i == 0;
}

Vector2 BlatterTestCFBC::u_bc(double x, double y, double z) const {
  (void) y;

  return blatter_xz_cfbc_exact(x, z, m_B, m_L, m_rho_i, m_rho_w, m_g);
}

void BlatterTestCFBC::residual_source_term(const fem::Q1Element3 &element,
                                           const double *surface,
                                           const double *bed,
                                           Vector2 *residual) {
  (void) surface;
  (void) bed;

  // compute x and z coordinates of quadrature points
  double
    *x = m_work[0],
    *z = m_work[1];
  {
    double
      *x_nodal = m_work[2],
      *z_nodal = m_work[3];

    for (int n = 0; n < fem::q13d::n_chi; ++n) {
      x_nodal[n] = element.x(n);
      z_nodal[n] = element.z(n);
    }

    element.evaluate(x_nodal, x);
    element.evaluate(z_nodal, z);
  }

  // loop over all quadrature points
  for (int q = 0; q < element.n_pts(); ++q) {
    auto W = element.weight(q) / m_scaling;

    auto F = blatter_xz_cfbc_source(x[q], z[q], m_L, m_rho_i, m_rho_w, m_g);

    // loop over all test functions
    for (int t = 0; t < element.n_chi(); ++t) {
      const auto &psi = element.chi(q, t);

      residual[t] += W * psi.val * F;
    }
  }
}

void BlatterTestCFBC::residual_surface(const fem::Q1Element3 &element,
                                       const fem::Q1Element3Face &face,
                                       Vector2 *residual) {
  // compute x and z coordinates of quadrature points
  double *x = m_work[0];
  {
    double *x_nodal = m_work[1];

    for (int n = 0; n < fem::q13d::n_chi; ++n) {
      x_nodal[n] = element.x(n);
    }

    face.evaluate(x_nodal, x);
  }

  for (int q = 0; q < face.n_pts(); ++q) {
    auto W = face.weight(q) / m_scaling;

    auto F = blatter_xz_cfbc_surface(x[q], m_L, m_rho_i, m_rho_w, m_g);

    // loop over all test functions
    for (int t = 0; t < element.n_chi(); ++t) {
      auto psi = face.chi(q, t);

      residual[t] += - W * psi * F;
    }
  }
}


void BlatterTestCFBC::residual_basal(const fem::Q1Element3 &element,
                                     const fem::Q1Element3Face &face,
                                     const double *tauc_nodal,
                                     const double *f_nodal,
                                     const Vector2 *u_nodal,
                                     Vector2 *residual) {
  (void) tauc_nodal;
  (void) f_nodal;
  (void) u_nodal;

  // compute x and z coordinates of quadrature points
  double *x = m_work[0];
  {
    double *x_nodal = m_work[1];

    for (int n = 0; n < fem::q13d::n_chi; ++n) {
      x_nodal[n] = element.x(n);
    }

    face.evaluate(x_nodal, x);
  }

  for (int q = 0; q < face.n_pts(); ++q) {
    auto W = face.weight(q) / m_scaling;

    auto F = blatter_xz_cfbc_base(x[q], m_L, m_rho_i, m_rho_w, m_g);

    // loop over all test functions
    for (int t = 0; t < element.n_chi(); ++t) {
      auto psi = face.chi(q, t);

      residual[t] += - W * psi * F;
    }
  }
}

void BlatterTestCFBC::jacobian_basal(const fem::Q1Element3Face &face,
                                     const double *tauc_nodal,
                                     const double *f_nodal,
                                     const Vector2 *u_nodal,
                                     double K[2 * fem::q13d::n_chi][2 * fem::q13d::n_chi]) {
  (void) face;
  (void) tauc_nodal;
  (void) f_nodal;
  (void) u_nodal;
  (void) K;
  // empty: residual contribution from the basal boundary does not depend on ice velocity
}

/*!
 * Do not use the floatation criterion, i.e. assume that the ice is always grounded.
 */
void BlatterTestCFBC::init_2d_parameters(const Inputs &inputs) {

  Blatter::init_2d_parameters(inputs);

  const IceModelVec2S &b = inputs.geometry->bed_elevation;

  {
    IceModelVec::AccessList list{&b, &m_parameters};

    for (Points p(*m_grid); p; p.next()) {
      const int i = p.i(), j = p.j();

      m_parameters(i, j).bed        = b(i, j);
      m_parameters(i, j).floatation = 0.0;
     }
  }

  m_parameters.update_ghosts();
}

} // end of namespace stressbalance
} // end of namespace pism
