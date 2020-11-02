#pragma once
#include <vector>
#include <eigen3/Eigen/Eigen>
#include "tokenizer.h"
using namespace Eigen;

class Joint
{
public:
    Joint();
    ~Joint();

    void update(const Matrix4f &parent);
    bool load(Tokenizer &tok);
    void add_child();
    void draw();
    void dfs(std::vector<Joint *> &joints);

private:
    Vector3f offset;
    Vector3f boxmin;
    Vector3f boxmax;
    Vector3f rotmin;
    Vector3f rotmax;
    Vector3f pose;
    std::vector<Joint *> children;

    Matrix4f local_matrix;
    Matrix4f world_matrix;

public:
    char name[256];
    MatrixXf boxV;
    MatrixXi boxF;
};