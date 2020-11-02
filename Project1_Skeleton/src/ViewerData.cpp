#include <iostream>

#include "ViewerData.h"
#include "MeshGL.h"
#include "MeshUtil.h"
#include "material_colors.h"
using namespace std;

ViewerData::ViewerData() : id(-1)
{
    clear();
}

void ViewerData::clear()
{
    V = MatrixXf(0, 3);
    F = MatrixXi(0, 3);

    F_material_ambient = Eigen::MatrixXf(0, 4);
    F_material_diffuse = Eigen::MatrixXf(0, 4);
    F_material_specular = Eigen::MatrixXf(0, 4);

    V_material_ambient = Eigen::MatrixXf(0, 4);
    V_material_diffuse = Eigen::MatrixXf(0, 4);
    V_material_specular = Eigen::MatrixXf(0, 4);

    F_normals = MatrixXf(0, 3);
    V_normals = MatrixXf(0, 3);

    line_color = Vector3f(0.0, 0.0, 0.0);
    line_width = 1.0;

    face_based = false;
    show_faces = true;
    show_lines = false;
}

void ViewerData::set_mesh(const MatrixXf &_V, const MatrixXi &_F)
{
    assert(_V.cols() == 3 && "Mesh vertex must contains x,y,z coordinates!");

    if (V.rows() == 0 && F.rows() == 0)
    {
        V = _V;
        F = _F;

        compute_normals();
        uniform_colors(
            Eigen::Vector3f(GOLD_AMBIENT[0], GOLD_AMBIENT[1], GOLD_AMBIENT[2]),
            Eigen::Vector3f(GOLD_DIFFUSE[0], GOLD_DIFFUSE[1], GOLD_DIFFUSE[2]),
            Eigen::Vector3f(GOLD_SPECULAR[0], GOLD_SPECULAR[1], GOLD_SPECULAR[2]));
    }

    dirty |= MeshGL::DIRTY_FACE | MeshGL::DIRTY_POSITION;
}

void ViewerData::uniform_colors(const Vector3f &ambient,
                                const Vector3f &diffuse,
                                const Vector3f &specular)
{
    Eigen::Vector4f ambient4;
    Eigen::Vector4f diffuse4;
    Eigen::Vector4f specular4;

    ambient4 << ambient, 1;
    diffuse4 << diffuse, 1;
    specular4 << specular, 1;
    V_material_ambient.resize(V.rows(), 4);
    V_material_diffuse.resize(V.rows(), 4);
    V_material_specular.resize(V.rows(), 4);
    for (unsigned i = 0; i < V.rows(); ++i)
    {
        V_material_ambient.row(i) = ambient4;
        V_material_diffuse.row(i) = diffuse4;
        V_material_specular.row(i) = specular4;
    }
    F_material_ambient.resize(F.rows(), 4);
    F_material_diffuse.resize(F.rows(), 4);
    F_material_specular.resize(F.rows(), 4);

    for (unsigned i = 0; i < F.rows(); ++i)
    {
        F_material_ambient.row(i) = ambient4;
        F_material_diffuse.row(i) = diffuse4;
        F_material_specular.row(i) = specular4;
    }
    dirty |= MeshGL::DIRTY_SPECULAR | MeshGL::DIRTY_DIFFUSE | MeshGL::DIRTY_AMBIENT;
}

void ViewerData::updateGL(const ViewerData &data, MeshGL &meshgl)
{
    if (!meshgl.is_initialized)
    {
        meshgl.init();
    }

    bool per_corner_normals = (data.F_normals.rows() == 3 * data.F.rows());
    meshgl.dirty |= data.dirty;

    // Copy Face attribute to 3 corners
    // Input:
    //   X  #F by dim quantity
    // Output:
    //   X_vbo  #F*3 by dim scattering per corner
    const auto per_face = [&data](const MatrixXf &X, MeshGL::RowMatrixXf &X_vbo) {
        X_vbo.resize(data.F.rows() * 3, X.cols());
        for (unsigned i = 0; i < data.F.rows(); ++i)
            for (unsigned j = 0; j < 3; j++)
                // copy attribute from one face to three vertices
                X_vbo.row(i * 3 + j) = X.row(i);
    };

    // Copy Vertex attributes to 3 corners
    // Input:
    //   X  #V by dim quantity
    // Output:
    //   X_vbo  #F*3 by dim scattering per corner
    const auto per_corner = [&data](const MatrixXf &X, MeshGL::RowMatrixXf &X_vbo) {
        X_vbo.resize(data.F.rows() * 3, X.cols());
        for (unsigned i = 0; i < data.F.rows(); ++i)
            for (unsigned j = 0; j < 3; j++)
                X_vbo.row(i * 3 + j) = X.row(data.F(i, j));
    };

    if (!data.face_based)
    {
        if (!per_corner_normals)
        {
            cout << __FILE__ << " " << __LINE__ << " copying vertex and face..." << endl;
            // #normals = #V
            // Vertex
            if (meshgl.dirty & MeshGL::DIRTY_POSITION)
            {
                meshgl.V_vbo = data.V.cast<float>();
            }
            // Vertex Normals
            if (meshgl.dirty & MeshGL::DIRTY_NORMAL)
            {
                meshgl.V_normals_vbo = data.V_normals.cast<float>();
            }
            // Per-vertex material
            if (meshgl.dirty & MeshGL::DIRTY_AMBIENT)
                meshgl.V_ambient_vbo = data.V_material_ambient.cast<float>();
            if (meshgl.dirty & MeshGL::DIRTY_DIFFUSE)
                meshgl.V_diffuse_vbo = data.V_material_diffuse.cast<float>();
            if (meshgl.dirty & MeshGL::DIRTY_SPECULAR)
                meshgl.V_specular_vbo = data.V_material_specular.cast<float>();

            // Face indices
            if (meshgl.dirty & MeshGL::DIRTY_FACE)
                meshgl.F_vbo = data.F.cast<unsigned>();
        }
        else
        {
            // // per vertex properties with per corner UVs
            // // Vertex
            // if (meshgl.dirty & MeshGL::DIRTY_POSITION)
            // {
            //     per_corner(data.V, meshgl.V_vbo);
            // }

            // if (meshgl.dirty & MeshGL::DIRTY_AMBIENT)
            // {
            //     per_corner(data.V_material_ambient, meshgl.V_ambient_vbo);
            // }
            // if (meshgl.dirty & MeshGL::DIRTY_DIFFUSE)
            // {
            //     per_corner(data.V_material_diffuse, meshgl.V_diffuse_vbo);
            // }
            // if (meshgl.dirty & MeshGL::DIRTY_SPECULAR)
            // {
            //     per_corner(data.V_material_specular, meshgl.V_specular_vbo);
            // }
            // //ToDo: use Face Normal instead of Vertex Normal
            // if (meshgl.dirty & MeshGL::DIRTY_NORMAL)
            // {
            //     per_corner(data.V_normals, meshgl.V_normals_vbo);
            // }

            // // Face indices
            // if (meshgl.dirty & MeshGL::DIRTY_FACE)
            // {
            //     meshgl.F_vbo.resize(data.F.rows(), 3);
            //     for (unsigned i = 0; i < data.F.rows(); ++i)
            //         meshgl.F_vbo.row(i) << i * 3 + 0, i * 3 + 1, i * 3 + 2;
            // }
        }
    }
    else
    {
        if (meshgl.dirty & MeshGL::DIRTY_POSITION)
        {
            per_corner(data.V, meshgl.V_vbo);
        }
        if (meshgl.dirty & MeshGL::DIRTY_AMBIENT)
        {
            per_face(data.F_material_ambient, meshgl.V_ambient_vbo);
        }
        if (meshgl.dirty & MeshGL::DIRTY_DIFFUSE)
        {
            per_face(data.F_material_diffuse, meshgl.V_diffuse_vbo);
        }
        if (meshgl.dirty & MeshGL::DIRTY_SPECULAR)
        {
            per_face(data.F_material_specular, meshgl.V_specular_vbo);
        }

        if (meshgl.dirty & MeshGL::DIRTY_NORMAL)
        {
            per_face(data.F_normals, meshgl.V_normals_vbo);
        }

        if (meshgl.dirty & MeshGL::DIRTY_FACE)
        {
            meshgl.F_vbo.resize(data.F.rows(), 3);
            for (unsigned i = 0; i < data.F.rows(); ++i)
                meshgl.F_vbo.row(i) << i * 3 + 0, i * 3 + 1, i * 3 + 2;
        }
    }
}

void ViewerData::compute_normals()
{
    per_face_normals(V, F, F_normals);
    per_vertex_normals(V, F, F_normals, V_normals);
    dirty |= MeshGL::DIRTY_NORMAL;
}

void ViewerData::set_face_based(bool newvalue)
{
    if (face_based != newvalue)
    {
        face_based = newvalue;
        dirty = MeshGL::DIRTY_ALL;
    }
}