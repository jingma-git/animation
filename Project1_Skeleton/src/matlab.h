#pragma once

#include <eigen3/Eigen/Eigen>
using namespace Eigen;

// https://stackoverflow.com/questions/35798698/eigen-matrix-library-coefficient-wise-modulo-operation
inline MatrixXi mod(const MatrixXi &m, int val)
{
    return (m.array() - (val * (m.array() / val)));
}