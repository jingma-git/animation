#include "joint.h"
#include "create.h"
#include "transform.h"
#include <iostream>
using namespace std;

#define ROT_MIN -100000
#define ROT_MAX 100000

Joint::Joint()
{
    offset = Vector3f(0, 0, 0);
    boxmin = Vector3f(-0.1, -0.1, -0.1);
    boxmax = Vector3f(0.1, 0.1, 0.1);
    rotmin = Vector3f(ROT_MIN, ROT_MIN, ROT_MIN);
    rotmax = Vector3f(ROT_MAX, ROT_MAX, ROT_MAX);
    pose = Vector3f(0, 0, 0);

    local_matrix = Matrix4f::Identity();
    world_matrix = Matrix4f::Identity();
}

Joint::~Joint()
{
}

void Joint::update(const Matrix4f &parent)
{
    // compute local matrix
    // compute world matrix
    // recursively call update() on children
    euler_matrix(pose(0), pose(1), pose(2), local_matrix);
    local_matrix.block(0, 3, 3, 1) = offset;
    world_matrix = parent * local_matrix;
    // cout << name << endl;
    // cout << "-------------local" << endl;
    // cout << local_matrix << endl;
    // cout << "-------------world" << endl;
    // cout << world_matrix << endl;
    make_box(boxmin, boxmax, boxV, boxF);
    MatrixX4f v4(boxV.rows(), 4);
    v4.block(0, 0, boxV.rows(), 3) = boxV;
    v4.block(0, 3, boxV.rows(), 1) = VectorXf::Ones(boxV.rows());
    boxV = (world_matrix * v4.transpose()).transpose().block(0, 0, boxV.rows(), 3);
    cout << this->name << endl;
    cout << v4 << endl;
    cout << "------------------" << endl;
    cout << boxV << endl;

    for (int i = 0; i < children.size(); i++)
    {
        children[i]->update(world_matrix);
    }
}

bool Joint::load(Tokenizer &tok)
{
    tok.get_token(name);
    // printf("%s\n", name);
    tok.find_token("{");
    while (true)
    {
        char temp[256];
        tok.get_token(temp);
        // printf("%s\n", temp);
        if (strcmp(temp, "offset") == 0)
        {
            float x = tok.get_float();
            float y = tok.get_float();
            float z = tok.get_float();
            offset = Vector3f(x, y, z);
        }
        else if (strcmp(temp, "boxmin") == 0)
        {
            float x = tok.get_float();
            float y = tok.get_float();
            float z = tok.get_float();
            boxmin = Vector3f(x, y, z);
        }
        else if (strcmp(temp, "boxmax") == 0)
        {
            float x = tok.get_float();
            float y = tok.get_float();
            float z = tok.get_float();
            boxmax = Vector3f(x, y, z);
        }
        else if (strcmp(temp, "rotxlimit") == 0)
        {
            rotmin.x() = tok.get_float();
            rotmax.x() = tok.get_float();
        }
        else if (strcmp(temp, "rotylimit") == 0)
        {
            rotmin.y() = tok.get_float();
            rotmax.y() = tok.get_float();
        }
        else if (strcmp(temp, "rotzlimit") == 0)
        {
            rotmin.z() = tok.get_float();
            rotmax.z() = tok.get_float();
        }
        else if (strcmp(temp, "pose") == 0)
        {
            float x = tok.get_float();
            float y = tok.get_float();
            float z = tok.get_float();
            pose = Vector3f(x, y, z);
        }
        else if (strcmp(temp, "balljoint") == 0)
        {
            Joint *joint = new Joint;
            joint->load(tok);
            children.push_back(joint);
        }
        else if (strcmp(temp, "}") == 0)
        {
            return true;
        }
        else
        { // unrecognized token
            tok.skip_line();
        }
    }
}

void Joint::add_child()
{
}

void Joint::draw()
{
    // do some opengl
    // recursively call draw() on children
}

void Joint::dfs(std::vector<Joint *> &joints)
{
    joints.push_back(this);
    for (int i = 0; i < children.size(); i++)
    {
        children[i]->dfs(joints);
    }
}