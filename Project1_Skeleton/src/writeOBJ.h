#pragma once
#include <eigen3/Eigen/Eigen>
#include <string>
#include <fstream>

template <typename DerivedV, typename DerivedF>
inline bool writeOBJ(
    std::string str,
    const Eigen::MatrixBase<DerivedV> &V,
    const Eigen::MatrixBase<DerivedF> &F)
{
    using namespace std;
    using namespace Eigen;
    assert(V.cols() == 3 && "V should have 3 columns");
    ofstream s(str);
    if (!s.is_open())
    {
        fprintf(stderr, "IOError: writeOBJ() could not open %s\n", str.c_str());
        return false;
    }
    s << V.format(IOFormat(FullPrecision, DontAlignCols, " ", "\n", "v ", "", "", "\n")) << (F.array() + 1).format(IOFormat(FullPrecision, DontAlignCols, " ", "\n", "f ", "", "", "\n"));
    return true;
}