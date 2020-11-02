#pragma once

#include <eigen3/Eigen/Eigen>

template <
    typename Derivedeye,
    typename Derivedcenter,
    typename Derivedup,
    typename DerivedR>
inline void look_at(
    const Eigen::PlainObjectBase<Derivedeye> &eye,
    const Eigen::PlainObjectBase<Derivedcenter> &center,
    const Eigen::PlainObjectBase<Derivedup> &up,
    Eigen::PlainObjectBase<DerivedR> &R)
{
    typedef Eigen::Matrix<typename DerivedR::Scalar, 3, 1> Vector3S;
    Vector3S f = (center - eye).normalized();
    Vector3S s = f.cross(up).normalized();
    Vector3S u = s.cross(f);
    R = Eigen::Matrix<typename DerivedR::Scalar, 4, 4>::Identity();
    R(0, 0) = s(0);
    R(0, 1) = s(1);
    R(0, 2) = s(2);
    R(1, 0) = u(0);
    R(1, 1) = u(1);
    R(1, 2) = u(2);
    R(2, 0) = -f(0);
    R(2, 1) = -f(1);
    R(2, 2) = -f(2);
    R(0, 3) = -s.transpose() * eye;
    R(1, 3) = -u.transpose() * eye;
    R(2, 3) = f.transpose() * eye;
}

template <typename DerivedP>
inline void ortho(
    const typename DerivedP::Scalar left,
    const typename DerivedP::Scalar right,
    const typename DerivedP::Scalar bottom,
    const typename DerivedP::Scalar top,
    const typename DerivedP::Scalar nearVal,
    const typename DerivedP::Scalar farVal,
    Eigen::PlainObjectBase<DerivedP> &P)
{
    P.setIdentity();
    P(0, 0) = 2. / (right - left);
    P(1, 1) = 2. / (top - bottom);
    P(2, 2) = -2. / (farVal - nearVal);
    P(0, 3) = -(right + left) / (right - left);
    P(1, 3) = -(top + bottom) / (top - bottom);
    P(2, 3) = -(farVal + nearVal) / (farVal - nearVal);
}

template <typename DerivedP>
inline void frustum(
    const typename DerivedP::Scalar left,
    const typename DerivedP::Scalar right,
    const typename DerivedP::Scalar bottom,
    const typename DerivedP::Scalar top,
    const typename DerivedP::Scalar nearVal,
    const typename DerivedP::Scalar farVal,
    Eigen::PlainObjectBase<DerivedP> &P)
{
    P.setConstant(4, 4, 0.);
    P(0, 0) = (2.0 * nearVal) / (right - left);
    P(1, 1) = (2.0 * nearVal) / (top - bottom);
    P(0, 2) = (right + left) / (right - left);
    P(1, 2) = (top + bottom) / (top - bottom);
    P(2, 2) = -(farVal + nearVal) / (farVal - nearVal);
    P(3, 2) = -1.0;
    P(2, 3) = -(2.0 * farVal * nearVal) / (farVal - nearVal);
}

inline void euler_matrix(float ai, float aj, float ak, Eigen::Matrix4f &M)
{
    // rotation around x, y, z
    float si = sin(ai);
    float sj = sin(aj);
    float sk = sin(ak);

    float ci = cos(ai);
    float cj = cos(aj);
    float ck = cos(ak);

    float cc = ci * ck;
    float cs = ci * sk;
    float sc = si * ck;
    float ss = si * sk;

    int i = 0, j = 1, k = 2;
    M = Eigen::Matrix4f::Identity();
    M(i, i) = cj * ck;
    M(i, j) = sj * sc - cs;
    M(i, k) = sj * cc + ss;
    M(j, i) = cj * sk;
    M(j, j) = sj * ss + cc;
    M(j, k) = sj * cs - sc;
    M(k, i) = -sj;
    M(k, j) = cj * si;
    M(k, k) = cj * ci;
}