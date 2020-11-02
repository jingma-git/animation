#pragma once
#include "joint.h"
#include <vector>
class Skeleton
{
public:
    Skeleton();
    ~Skeleton();

    bool load(const char *filename);
    void update();
    void draw();
    void save_mesh(std::string fname);

private:
    void dfs(std::vector<Joint *> &joints);

    Joint *root;
    Matrix4f world_matrix;
};