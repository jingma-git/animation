#include <iostream>
#include "../includes/glad/glad.h"
#include "ViewerCore.h"
#include "MeshGL.h"
#include "MeshUtil.h"
#include "transform.h"

using namespace Eigen;
using namespace std;

ViewerCore::ViewerCore()
{
    // Default colors
    background_color << 0.3f, 0.3f, 0.5f, 1.0f;

    // Default lights settings
    light_position << 0.0f, 0.3f, 0.0f;
    lighting_factor = 1.0f; //on

    // Camera parameters
    camera_base_zoom = 1.0f;
    camera_zoom = 1.0f;
    orthographic = false;
    camera_view_angle = 45.0;
    camera_dnear = 1.0;
    camera_dfar = 100.0;
    camera_base_translation << 0, 0, 0;
    camera_translation << 0, 0, 0;
    camera_eye << 0, 0, 5;
    camera_center << 0, 0, 0;
    camera_up << 0, 1, 0;

    depth_test = true;

    is_animating = false;
    animation_max_fps = 30.;

    viewport.setZero();
}

void ViewerCore::clear_framebuffers()
{
    // The glScissor call ensures we only clear this core's buffers,
    // (in case the user wants different background colors in each viewport.)
    // glScissor(viewport(0), viewport(1), viewport(2), viewport(3));
    // glEnable(GL_SCISSOR_TEST);
    glClearColor(background_color[0],
                 background_color[1],
                 background_color[2],
                 background_color[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glDisable(GL_SCISSOR_TEST);
}

void ViewerCore::draw(ViewerData &data, bool update_matrices)
{
    if (depth_test)
    {
        glEnable(GL_DEPTH_TEST);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Bind
    if (data.dirty)
    {
        // convert data to opengl format, setup opengl program if it is not initialized
        data.updateGL(data, data.meshgl);
        data.dirty = MeshGL::DIRTY_NONE;
    }
    data.meshgl.bind_mesh();

    // Uniform
    glViewport(viewport(0), viewport(1), viewport(2), viewport(3));
    if (update_matrices)
    {
        view = Matrix4f::Identity();
        proj = Matrix4f::Identity();
        norm = Matrix4f::Identity();

        float width = viewport(2);
        float height = viewport(3);
        look_at(camera_eye, camera_center, camera_up, view);
        //ToDo: add scale and rotation
        norm = view.inverse().transpose();

        if (orthographic)
        {
            float length = (camera_eye - camera_center).norm();
            float h = tan(camera_view_angle / 360.0 * M_PI) * (length);
            ortho(-h * width / height, h * width / height, -h, h, camera_dnear, camera_dfar, proj);
        }
        else
        {
            float fH = tan(camera_view_angle / 360.0 * M_PI) * camera_dnear;
            float fW = fH * (double)width / (double)height;
            frustum(-fW, fW, -fH, fH, camera_dnear, camera_dfar, proj);
        }
    }
    // Send transformations to the GPU
    GLint viewi = glGetUniformLocation(data.meshgl.shader_mesh, "view");
    GLint proji = glGetUniformLocation(data.meshgl.shader_mesh, "proj");
    GLint normi = glGetUniformLocation(data.meshgl.shader_mesh, "normal_matrix");
    glUniformMatrix4fv(viewi, 1, GL_FALSE, view.data());
    glUniformMatrix4fv(proji, 1, GL_FALSE, proj.data());
    glUniformMatrix4fv(normi, 1, GL_FALSE, norm.data());

    GLint fixed_colori = glGetUniformLocation(data.meshgl.shader_mesh, "fixed_color");

    if (data.V.rows() > 0)
    {
        // Render fill
        if (data.show_faces)
        {
            data.meshgl.draw_mesh(true);
        }

        // Render Wireframe
        if (data.show_lines)
        {
            glLineWidth(data.line_width);
            glUniform4f(fixed_colori, data.line_color[0], data.line_color[1], data.line_color[2], 1.0);
            data.meshgl.draw_mesh(false);
            glUniform4f(fixed_colori, 0.0f, 0.0f, 0.0f, 0.0f);
        }
    }
}

void ViewerCore::align_camera_center(
    const Eigen::MatrixXf &V,
    const Eigen::MatrixXi &F)
{
    if (V.rows() == 0)
        return;
    get_scale_and_shift_to_fit_mesh(V, F, camera_base_zoom, camera_base_translation);
    if (V.size() > 0)
    {
        object_scale = (V.colwise().maxCoeff() - V.colwise().minCoeff()).norm();
    }
}

void ViewerCore::get_scale_and_shift_to_fit_mesh(
    const Eigen::MatrixXf &V,
    const Eigen::MatrixXi &F,
    float &zoom,
    Eigen::Vector3f &shift)
{
    MatrixXf BC;
    barycenter(V, F, BC);

    auto min_point = BC.colwise().minCoeff();
    auto max_point = BC.colwise().maxCoeff();
    auto centroid = (0.5 * (min_point + max_point)).eval();
    shift.setZero();
    shift.head(centroid.size()) = -centroid;
    zoom = 2.0 / (max_point - min_point).array().abs().maxCoeff();
}