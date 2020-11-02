#include "viewer.h"

#include "skeleton.h"
#include "tokenizer.h"
#include "create.h"
#include "matlab.h"
#include "TimeUtil.h"
#include <iostream>
using namespace std;

int main()
{
    const char *fname = "data/test.skel";
    Skeleton skeleton;
    skeleton.load(fname);
    skeleton.update();
    skeleton.save_mesh("skeleton.obj");

    // MatrixXf V;
    // MatrixXi F;
    // // Vector3f boxmin(-1, -1, -1), boxmax(1, 1, 1);
    // // make_box(boxmin * 0.5, boxmax * 0.5, V, F);
    // make_triangle(V, F);
    // // V = MatrixXf::Random(10000, 3);
    // // F = ((MatrixXf::Random(300000, 3).array() + 1) * 4999).cast<int>();

    // Viewer viewer;
    // viewer.load_mesh(V, F);
    // viewer.launch();
    return 0;
}