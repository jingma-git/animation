#pragma once
#include <eigen3/Eigen/Eigen>
using namespace Eigen;

inline void make_box(const Vector3f &boxMin,
                     const Vector3f &boxMax,
                     MatrixXf &V, MatrixXi &F)
{
    V.resize(8, 3);
    F.resize(12, 3);
    V << boxMin.x(), boxMin.y(), boxMin.z(),
        boxMin.x(), boxMax.y(), boxMin.z(),
        boxMax.x(), boxMax.y(), boxMin.z(),
        boxMax.x(), boxMin.y(), boxMin.z(),
        boxMin.x(), boxMin.y(), boxMax.z(),
        boxMin.x(), boxMax.y(), boxMax.z(),
        boxMax.x(), boxMax.y(), boxMax.z(),
        boxMax.x(), boxMin.y(), boxMax.z();
    F << 0, 1, 3,
        3, 1, 2,
        0, 4, 1,
        1, 4, 5,
        3, 2, 7,
        7, 2, 6,
        4, 0, 3,
        7, 4, 3,
        6, 4, 7,
        6, 5, 4,
        1, 5, 6,
        2, 1, 6;
}

inline void make_triangle(MatrixXf &V, MatrixXi &F)
{
    V.resize(3, 3);
    F.resize(1, 3);
    V << -0.5, 0.0, 0.0,
        0.0, 0.5, 0.0,
        0.5, 0.0, 0.0;
    F << 0, 1, 2;
}