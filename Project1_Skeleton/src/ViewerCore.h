#pragma once
#include <eigen3/Eigen/Eigen>
#include "ViewerData.h"
using namespace Eigen;

class ViewerCore
{
public:
    ViewerCore();

    void clear_framebuffers();
    void draw(ViewerData &data, bool update_matrices = true);

    void align_camera_center(
        const Eigen::MatrixXf &V,
        const Eigen::MatrixXi &F);

    void get_scale_and_shift_to_fit_mesh(
        const Eigen::MatrixXf &V,
        const Eigen::MatrixXi &F,
        float &zoom,
        Eigen::Vector3f &shift);

    unsigned int id = 1u;
    Vector4f background_color;

    // light
    Vector3f light_position;
    float lighting_factor;

    // Camera parameters
    float camera_base_zoom;
    float camera_zoom;
    bool orthographic;
    Eigen::Vector3f camera_base_translation;
    Eigen::Vector3f camera_translation;
    Eigen::Vector3f camera_eye;
    Eigen::Vector3f camera_up;
    Eigen::Vector3f camera_center;
    float camera_view_angle;
    float camera_dnear;
    float camera_dfar;

    bool depth_test;

    // Animation
    bool is_animating;
    double animation_max_fps;

    // Viewport size
    Eigen::Vector4f viewport;

    float object_scale;

    Eigen::Matrix4f view;
    Eigen::Matrix4f proj;
    Eigen::Matrix4f norm;
};