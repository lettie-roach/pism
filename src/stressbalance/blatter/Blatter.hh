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
#ifndef PISM_BLATTER_H
#define PISM_BLATTER_H

#include "pism/stressbalance/ShallowStressBalance.hh"
#include "pism/util/petscwrappers/SNES.hh"
#include "pism/util/petscwrappers/DM.hh"
#include "pism/util/petscwrappers/Vec.hh"
#include "pism/util/fem/FEM.hh"

namespace pism {

namespace fem {
class Element3;
class Q1Element3Face;
} // end of namespace fem

namespace stressbalance {

class Blatter : public ShallowStressBalance {
public:
  Blatter(IceGrid::ConstPtr grid, int Mz, int coarsening_factor);
  virtual ~Blatter() = default;

  void update(const Inputs &inputs, bool);

  IceModelVec3::Ptr velocity_u_sigma() const;
  IceModelVec3::Ptr velocity_v_sigma() const;

  /*!
   * 2D input parameters
   */
  struct Parameters {
    // elevation (z coordinate) of the bottom domain boundary
    double bed;
    // thickness of the domain
    double thickness;
    // NodeType stored as double
    double node_type;
    // basal yield stress
    double tauc;
    // sea level elevation (used to determine if a location is grounded)
    double sea_level;
    // floatation function (positive where floating, zero or negative where grounded)
    double floatation;
    // FIXME: Add Dirichlet BC at a map plane location.
  };

protected:
  // u and v components of ice velocity on the fine sigma grid
  IceModelVec3::Ptr m_u_sigma, m_v_sigma;

  // 3D dof=2 DM used by m_snes
  petsc::DM m_da;
  // storage for the solution
  petsc::Vec m_x;
  // storage for the old solution during parameter continuation
  petsc::Vec m_x_old;
  // solver
  petsc::SNES m_snes;

  IceModelVec2<Parameters> m_parameters;

  // Scaling of quadrature weights (note: this does not seem to matter).
  double m_scaling;

  // Ice density times g
  double m_rho_ice_g;

  // Water density times g
  double m_rho_ocean_g;

  // The flow law Glen exponent
  double m_glen_exponent;

  // Use the eta-transformation to compute surface gradient
  bool m_eta_transform;

  // Viscosity regularization constant
  double m_viscosity_eps;

  // Enhancement factor for ice viscosity.
  double m_E_viscosity;

  // True if the Eisenstat-Walker method of adjusting linear solver tolerances is enabled.
  bool m_ksp_use_ew;

  static const int m_Nq = 100;
  static const int m_n_work = 9;

  double m_work[m_n_work][m_Nq];

  Vector2 m_work2[m_n_work][m_Nq];

  fem::Q1Element3Face m_face4;
  fem::Q1Element3Face m_face100;

  void init_impl();

  void define_model_state_impl(const File &output) const;

  void write_model_state_impl(const File &output) const;

  static bool exterior_element(const int *node_type);

  static bool grounding_line(const double *F);

  static bool partially_submerged_face(int face, const double *z, const double *sea_level);

  void compute_node_type(double min_thickness);

  virtual void nodal_parameter_values(const fem::Q1Element3 &element,
                                      Parameters **P,
                                      int i,
                                      int j,
                                      int *node_type,
                                      double *bottom,
                                      double *thickness,
                                      double *surface,
                                      double *sea_level) const;

  virtual bool marine_boundary(int face,
                               const int *node_type,
                               const double *ice_bottom,
                               const double *sea_level);

  virtual bool dirichlet_node(const DMDALocalInfo &info, const fem::Element3::GlobalIndex& I);

  virtual Vector2 u_bc(double x, double y, double z) const;

  void compute_jacobian(DMDALocalInfo *info, const Vector2 ***x, Mat A, Mat J);

  void jacobian_dirichlet(const DMDALocalInfo &info, Parameters **P, Mat J);

  virtual void jacobian_f(const fem::Q1Element3 &element,
                          const Vector2 *u_nodal,
                          const double *B_nodal,
                          double K[2 * fem::q13d::n_chi][2 * fem::q13d::n_chi]);

  virtual void jacobian_basal(const fem::Q1Element3Face &face,
                              const double *tauc_nodal,
                              const double *f_nodal,
                              const Vector2 *u_nodal,
                              double K[2 * fem::q13d::n_chi][2 * fem::q13d::n_chi]);

  void compute_residual(DMDALocalInfo *info, const Vector2 ***X, Vector2 ***R);

  void residual_dirichlet(const DMDALocalInfo &info,
                          Parameters **P,
                          const Vector2 ***x,
                          Vector2 ***R);

  virtual void residual_f(const fem::Q1Element3 &element,
                          const Vector2 *u_nodal,
                          const double *B_nodal,
                          Vector2 *residual);

  virtual void residual_source_term(const fem::Q1Element3 &element,
                                    const double *surface,
                                    const double *bed,
                                    Vector2 *residual);

  virtual void residual_basal(const fem::Q1Element3 &element,
                              const fem::Q1Element3Face &face,
                              const double *tauc_nodal,
                              const double *f_nodal,
                              const Vector2 *u_nodal,
                              Vector2 *residual);

  virtual void residual_surface(const fem::Q1Element3 &element,
                                const fem::Q1Element3Face &face,
                                Vector2 *residual);

  virtual void residual_lateral(const fem::Q1Element3 &element,
                                const fem::Q1Element3Face &face,
                                const double *surface_nodal,
                                const double *z_nodal,
                                const double *sl_nodal,
                                Vector2 *residual);

  static PetscErrorCode jacobian_callback(DMDALocalInfo *info,
                                          const Vector2 ***x,
                                          Mat A, Mat J,
                                          Blatter *solver);

  static PetscErrorCode function_callback(DMDALocalInfo *info,
                                          const Vector2 ***x, Vector2 ***f,
                                          Blatter *solver);

  virtual void init_2d_parameters(const Inputs &inputs);

  void init_ice_hardness(const Inputs &inputs, const petsc::DM &da);

  // Guts of the constructor. This method wraps PETSc calls to simplify error checking.
  PetscErrorCode setup(DM pism_da, Periodicity p, int Mz, int coarsening_factor,
                       const std::string &prefix);

  void set_initial_guess(const IceModelVec3 &u_sigma, const IceModelVec3 &v_sigma);

  void copy_solution();

  void compute_averaged_velocity(IceModelVec2V &result);

  void get_basal_velocity(IceModelVec2V &result);

  void report_mesh_info();

  struct SolutionInfo {
    PetscInt snes_it, ksp_it, mg_coarse_ksp_it;
    SNESConvergedReason snes_reason;
    KSPConvergedReason ksp_reason;
  };

  SolutionInfo solve();
  SolutionInfo parameter_continuation();
};

} // end of namespace stressbalance
} // end of namespace pism

#endif /* PISM_BLATTER_H */
