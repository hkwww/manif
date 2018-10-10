#ifndef _MANIF_MANIF_SIM2TANGENT_BASE_H_
#define _MANIF_MANIF_SIM2TANGENT_BASE_H_

#include "manif/impl/SIM2/SIM2_properties.h"
#include "manif/impl/tangent_base.h"

namespace manif
{

///////////////
///         ///
/// Tangent ///
///         ///
///////////////

template <typename _Derived>
struct SIM2TangentBase : TangentBase<_Derived>
{
private:

  using Base = TangentBase<_Derived>;
  using Type = SIM2TangentBase<_Derived>;

public:

  MANIF_TANGENT_PROPERTIES
  MANIF_TANGENT_TYPEDEF
  MANIF_INHERIT_TANGENT_OPERATOR

  using Base::data;
  using Base::coeffs;

  /// Tangent common API

  LieAlg hat() const;

  Manifold retract(OptJacobianRef J_m_t = {}) const;

  Jacobian rjac() const;
  Jacobian ljac() const;

  Jacobian adj() const;

  /// SIM2Tangent specific API

  Scalar x() const;
  Scalar y() const;
  Scalar angle() const;
};

template <typename _Derived>
typename SIM2TangentBase<_Derived>::LieAlg
SIM2TangentBase<_Derived>::hat() const
{
  return ( LieAlg() <<
             Scalar(0), -angle(),   x(),
             angle(),    Scalar(0), y(),
             Scalar(0),  Scalar(0), Scalar(0) ).finished();
}

template <typename _Derived>
typename SIM2TangentBase<_Derived>::Manifold
SIM2TangentBase<_Derived>::retract(OptJacobianRef J_m_t) const
{
  using std::abs;
  using std::cos;
  using std::sin;

  const Scalar theta = angle();
  const Scalar cos_theta = cos(theta);
  const Scalar sin_theta = sin(theta);
  const Scalar theta_sq = theta * theta;

  Scalar A,  // sin_theta_by_theta
         B;  // one_minus_cos_theta_by_theta

  if (theta_sq < Constants<Scalar>::eps_s)
  {
    // Taylor approximation
    A = Scalar(1) - Scalar(1. / 6.) * theta_sq;
    B = Scalar(.5) * theta - Scalar(1. / 24.) * theta * theta_sq;
  }
  else
  {
    // Euler
    A = sin_theta / theta;
    B = (Scalar(1) - cos_theta) / theta;
  }

  if (J_m_t)
  {
    // Jr
    J_m_t->setIdentity();
    (*J_m_t)(0,0) =  A;
    (*J_m_t)(0,1) =  B;
    (*J_m_t)(1,0) = -B;
    (*J_m_t)(1,1) =  A;

    if (theta_sq < Constants<Scalar>::eps_s)
    {
      (*J_m_t)(0,2) = -y() / Scalar(2) + theta * x() / Scalar(6);
      (*J_m_t)(1,2) =  x() / Scalar(2) + theta * y() / Scalar(6);
    }
    else
    {
      (*J_m_t)(0,2) = (-y() + theta*x() + y()*cos_theta - x()*sin_theta)/theta_sq;
      (*J_m_t)(1,2) = ( x() + theta*y() - x()*cos_theta - y()*sin_theta)/theta_sq;
    }
  }

  return Manifold( A * x() - B * y(),
                   B * x() + A * y(),
                   cos_theta, sin_theta );
}

template <typename _Derived>
typename SIM2TangentBase<_Derived>::Jacobian
SIM2TangentBase<_Derived>::rjac() const
{
//  const Scalar theta = angle();
//  const Scalar cos_theta = cos(theta);
//  const Scalar sin_theta = sin(theta);
//  const Scalar theta_sq = theta * theta;

//  Scalar A,  // sin_theta_by_theta
//         B;  // one_minus_cos_theta_by_theta

//  if (abs(theta) < Constants<Scalar>::eps)
//  {
//    // Taylor approximation
//    A = Scalar(1) - Scalar(1. / 6.) * theta_sq;
//    B = Scalar(.5) * theta - Scalar(1. / 24.) * theta * theta_sq;
//  }
//  else
//  {
//    // Euler
//    A = sin_theta / theta;
//    B = (Scalar(1) - cos_theta) / theta;
//  }

//  Jacobian Jr = Jacobian::Identity();
//  Jr(0,0) =  A;
//  Jr(0,1) =  B;
//  Jr(1,0) = -B;
//  Jr(1,1) =  A;

//  Jr(0,2) = (-y() + theta*x() + y()*cos_theta - x()*sin_theta)/theta_sq;
//  Jr(1,2) = ( x() + theta*y() - x()*cos_theta - y()*sin_theta)/theta_sq;

//  return Jr;

  Jacobian Jr;
  retract(Jr);

  return Jr;
}

template <typename _Derived>
typename SIM2TangentBase<_Derived>::Jacobian
SIM2TangentBase<_Derived>::ljac() const
{
  const Scalar theta = angle();
  const Scalar cos_theta = cos(theta);
  const Scalar sin_theta = sin(theta);
  const Scalar theta_sq = theta * theta;

  Scalar A,  // sin_theta_by_theta
         B;  // one_minus_cos_theta_by_theta

  if (theta_sq < Constants<Scalar>::eps_s)
  {
    // Taylor approximation
    A = Scalar(1) - Scalar(1. / 6.) * theta_sq;
    B = Scalar(.5) * theta - Scalar(1. / 24.) * theta * theta_sq;
  }
  else
  {
    // Euler
    A = sin_theta / theta;
    B = (Scalar(1) - cos_theta) / theta;
  }

  Jacobian Jl = Jacobian::Identity();
  Jl(0,0) =  A;
  Jl(0,1) = -B;
  Jl(1,0) =  B;
  Jl(1,1) =  A;

  Jl(0,2) = ( y() + theta*x() - y()*cos_theta - x()*sin_theta)/theta_sq;
  Jl(1,2) = (-x() + theta*y() + x()*cos_theta - y()*sin_theta)/theta_sq;

  return Jl;
}

template <typename _Derived>
typename SIM2TangentBase<_Derived>::Jacobian
SIM2TangentBase<_Derived>::adj() const
{
  Jacobian adj = Jacobian::Zero();

  adj(0,1) = -angle();
  adj(1,0) =  angle();
  adj(0,2) =  y();
  adj(1,2) = -x();

  return adj;
}

/// SIM2Tangent specific API

template <typename _Derived>
typename SIM2TangentBase<_Derived>::Scalar
SIM2TangentBase<_Derived>::x() const
{
  return coeffs().x();
}

template <typename _Derived>
typename SIM2TangentBase<_Derived>::Scalar
SIM2TangentBase<_Derived>::y() const
{
  return coeffs().y();
}

template <typename _Derived>
typename SIM2TangentBase<_Derived>::Scalar
SIM2TangentBase<_Derived>::angle() const
{
  return coeffs().z();
}

} /* namespace manif */

#endif /* _MANIF_MANIF_SIM2_BASE_H_ */