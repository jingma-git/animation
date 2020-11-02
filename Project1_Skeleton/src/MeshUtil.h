#pragma once
#include <eigen3/Eigen/Eigen>
#include <iostream>
using namespace Eigen;

template <typename TV, typename TF, typename TN>
inline void area(const MatrixBase<TV> &V,
                 const MatrixBase<TF> &F,
                 PlainObjectBase<TN> &A)
{
    using namespace std;

    A.resize(F.rows(), 1);
    int Frows = F.rows();
#pragma omp parallel for if (Frows > 10000)
    for (int i = 0; i < Frows; i++)
    {
        Matrix<typename TV::Scalar, 1, 3> v1 = V.row(F(i, 1)) - V.row(F(i, 0));
        Matrix<typename TV::Scalar, 1, 3> v2 = V.row(F(i, 2)) - V.row(F(i, 0));
        const auto a = 0.5 * (v1.cross(v2)).norm();
        A(i, 0) = a;
        // cout << i << " area=" << A.row(i) << endl;
    }
}

template <typename TV, typename TF, typename TN>
inline void per_face_normals(
    const MatrixBase<TV> &V,
    const MatrixBase<TF> &F,
    PlainObjectBase<TN> &N)
{
    using namespace std;

    N.resize(F.rows(), 3);
    int Frows = F.rows();
    Matrix<typename TV::Scalar, 1, 3> zeros(0, 0, 0);
#pragma omp parallel for if (Frows > 10000)
    for (int i = 0; i < Frows; i++)
    {
        Matrix<typename TV::Scalar, 1, 3> v1 = V.row(F(i, 1)) - V.row(F(i, 0));
        Matrix<typename TV::Scalar, 1, 3> v2 = V.row(F(i, 2)) - V.row(F(i, 0));
        N.row(i) = v1.cross(v2);
        typename TV::Scalar r = N.row(i).norm();
        if (r == 0)
        {
            N.row(i) = zeros;
        }
        else
        {
            N.row(i) /= r;
            // cout << i << ": v1=" << V.row(F(i, 0)) << " v2=" << V.row(F(i, 1)) << " v3=" << V.row(F(i, 2)) << " normal=" << N.row(i) << endl;
        }
    }
}

template <typename TV, typename TF, typename TFN, typename TN>
inline void per_vertex_normals(
    const MatrixBase<TV> &V,
    const MatrixBase<TF> &F,
    const MatrixBase<TFN> &FN,
    PlainObjectBase<TN> &N)
{
    using namespace std;

    N.setZero(V.rows(), 3);
    int Frows = F.rows();
    Matrix<typename TN::Scalar, TF::RowsAtCompileTime, 3> W(Frows, 3);
    Matrix<typename TN::Scalar, TF::RowsAtCompileTime, 1> A(Frows, 1);
    area(V, F, A);
    W = A.replicate(1, 3);

#pragma omp parallel for if (Frows > 10000)
    for (int i = 0; i < Frows; i++)
    {
        for (int j = 0; j < V.cols(); j++)
        {
            N.row(F(i, j)) += W(i, j) * FN.row(i);
        }
    }
    N.rowwise().normalize();
}

template <typename DerivedV, typename DerivedF, typename DerivedBC>
inline void barycenter(
    const Eigen::MatrixBase<DerivedV> &V,
    const Eigen::MatrixBase<DerivedF> &F,
    Eigen::PlainObjectBase<DerivedBC> &BC)
{
    BC.setZero(F.rows(), V.cols());
    // Loop over faces
    for (int i = 0; i < F.rows(); i++)
    {
        // loop around face
        for (int j = 0; j < F.cols(); j++)
        {
            // Accumulate
            BC.row(i) += V.row(F(i, j));
        }
        // average
        BC.row(i) /= double(F.cols());
    }
}