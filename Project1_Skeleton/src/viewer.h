#pragma once
#include "ViewerData.h"
#include "ViewerCore.h"

#include <string>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Viewer
{
public:
    enum class MouseButton
    {
        Left,
        Middle,
        Right
    };
    enum class MouseMode
    {
        None,
        Rotation,
        Zoom,
        Pan,
        Translation
    } mouse_mode;

    Viewer();

    int launch(bool resizable = true, bool fullscreen = false, const std::string &name = "libigl viewer", int width = 0, int height = 0);
    int launch_init(bool resizable = true, bool fullscreen = false, const std::string &name = "libigl viewer", int width = 0, int height = 0);
    bool launch_rendering(bool loop = true);
    void launch_shut();
    void init();

    void draw();

    ViewerCore &core(unsigned core_id = 0);
    const ViewerCore &core(unsigned core_id = 0) const;
    size_t core_index(const int id) const;
    void resize(int w, int h);

    bool load_mesh(const MatrixXf &_V, const MatrixXi &_F);
    ViewerData &data(int mesh_id = -1);
    const ViewerData &data(int mesh_id = -1) const;
    size_t mesh_index(const int id) const;
    int append_mesh();

    // Stores all the data that should be visualized
    std::vector<ViewerData> data_list;
    size_t selected_data_index;
    int next_data_id;

    // Stores all the viewing options
    std::vector<ViewerCore> core_list;
    size_t selected_core_index;
    int next_core_id;

    GLFWwindow *window;
};