#include "skeleton.h"
#include "tokenizer.h"
#include "writeOBJ.h"
#include <iostream>
using namespace std;

Skeleton::Skeleton()
{
    world_matrix = Matrix4f::Identity();
}

Skeleton::~Skeleton()
{
}

bool Skeleton::load(const char *filename)
{
    Tokenizer tok;
    tok.open(filename);

    bool found = tok.find_token("balljoint");
    if (!found)
    {
        printf("Error: Cannot find root joint!\n");
        return false;
    }
    root = new Joint;
    root->load(tok);
    tok.close();

    return true;
}

void Skeleton::update()
{
    // traverses tree & compute joint matrices
    root->update(world_matrix);
}

void Skeleton::draw()
{
}

void Skeleton::save_mesh(std::string fname)
{
    std::vector<Joint *> joints;
    dfs(joints);
    MatrixXf V((joints.size()) * 8, 3);
    MatrixXi F((joints.size()) * 12, 3);

    int offset = 0;
    for (int i = 0; i < joints.size(); i++)
    {
        V.block(offset * 8, 0, 8, 3) = joints[i]->boxV;
        F.block(offset * 12, 0, 12, 3).array() = (joints[i]->boxF.array() + offset * 8);
        offset++;
    }
    writeOBJ(fname, V, F);
    cout << __FILE__ << " " << __LINE__ << ": joints num=" << joints.size() << endl;
}

void Skeleton::dfs(std::vector<Joint *> &joints)
{
    root->dfs(joints);
}