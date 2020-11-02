#pragma once
#include <eigen3/Eigen/Eigen>
#include "MeshGL.h"
using namespace Eigen;

class ViewerData
{
    typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> RowMatrixXf;
    typedef Eigen::Matrix<unsigned, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> RowMatrixXu;

public:
    ViewerData();

    void set_mesh(const MatrixXf &_V, const MatrixXi &_F);
    void set_face_based(bool newvalue);
    void uniform_colors(const Vector3f &ambient, const Vector3f &diffuse, const Vector3f &specular);
    void clear();

    void updateGL(const ViewerData &data, MeshGL &meshgl);
    void compute_normals();
    // OpenGL representation of the mesh
    MeshGL meshgl;

    // Vertex, Face
    MatrixXf V;
    MatrixXi F;

    // Per face attributes
    MatrixXf F_normals;
    Eigen::MatrixXf F_material_ambient;  // Per face ambient color
    Eigen::MatrixXf F_material_diffuse;  // Per face diffuse color
    Eigen::MatrixXf F_material_specular; // Per face specular color

    // Per vertex attributes
    MatrixXf V_normals;                  // One normal per vertex
    Eigen::MatrixXf V_material_ambient;  // Per vertex ambient color
    Eigen::MatrixXf V_material_diffuse;  // Per vertex diffuse color
    Eigen::MatrixXf V_material_specular; // Per vertex specular color

    // Lines
    Vector3f line_color;

    int id;
    // Marks dirty buffers that need to be uploaded to OpenGL
    uint32_t dirty;

    // Enable per-face or per-vertex properties
    bool face_based;
    bool show_faces;
    bool show_lines;

    float line_width;
};