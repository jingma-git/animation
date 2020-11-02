#pragma once
#include <eigen3/Eigen/Eigen>
using namespace Eigen;

class MeshGL
{
public:
    typedef unsigned int GLuint;
    typedef unsigned int GLint;
    typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> RowMatrixXf;
    typedef Eigen::Matrix<unsigned, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> RowMatrixXu;

    enum DirtyFlags
    {
        DIRTY_NONE = 0x0000,
        DIRTY_POSITION = 0x0001,
        DIRTY_UV = 0x0002,
        DIRTY_NORMAL = 0x0004,
        DIRTY_AMBIENT = 0x0008,
        DIRTY_DIFFUSE = 0x0010,
        DIRTY_SPECULAR = 0x0020,
        DIRTY_TEXTURE = 0x0040,
        DIRTY_FACE = 0x0080,
        DIRTY_MESH = 0x00FF,
        DIRTY_OVERLAY_LINES = 0x0100,
        DIRTY_OVERLAY_POINTS = 0x0200,
        DIRTY_VERTEX_LABELS = 0x0400,
        DIRTY_FACE_LABELS = 0x0800,
        DIRTY_CUSTOM_LABELS = 0x1000,
        DIRTY_ALL = 0xFFFF
    };

    MeshGL();
    void init();
    void init_buffers();
    void bind_mesh();
    void free();
    void free_buffers();
    void draw_mesh(bool show_faces);

    bool is_initialized = false;
    GLuint vao_mesh;
    GLuint shader_mesh;

    GLuint vbo_V;          // Vertices of the current mesh (#V x 3)
    GLuint vbo_V_normals;  // Vertices of the current mesh (#V x 3)
    GLuint vbo_V_ambient;  // Ambient material  (#V x 3)
    GLuint vbo_V_diffuse;  // Diffuse material  (#V x 3)
    GLuint vbo_V_specular; // Specular material  (#V x 3)

    GLuint vbo_F; // Faces of the mesh (#F x 3)

    RowMatrixXf V_vbo;
    RowMatrixXf V_normals_vbo;
    RowMatrixXf V_ambient_vbo;
    RowMatrixXf V_diffuse_vbo;
    RowMatrixXf V_specular_vbo;

    RowMatrixXu F_vbo;

    // Marks dirty buffers that need to be uploaded to OpenGL
    uint32_t dirty;
};