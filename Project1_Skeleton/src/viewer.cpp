#include "viewer.h"
#include <iostream>
using namespace std;

static Viewer *__viewer;
static void glfw_error_callback(int error, const char *description)
{
    fputs(description, stderr);
}

// static void glfw_mouse_press(GLFWwindow *window, int button, int action, int modifier)
// {

//     Viewer::MouseButton mb;

//     if (button == GLFW_MOUSE_BUTTON_1)
//         mb = Viewer::MouseButton::Left;
//     else if (button == GLFW_MOUSE_BUTTON_2)
//         mb = Viewer::MouseButton::Right;
//     else //if (button == GLFW_MOUSE_BUTTON_3)
//         mb = Viewer::MouseButton::Middle;

//     if (action == GLFW_PRESS)
//         __viewer->mouse_down(mb, modifier);
//     else
//         __viewer->mouse_up(mb, modifier);
// }

static void glfw_key_callback(GLFWwindow *window, int key, int scancode, int action, int modifier)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    // if (action == GLFW_PRESS)
    //     __viewer->key_down(key, modifier);
    // else if (action == GLFW_RELEASE)
    //     __viewer->key_up(key, modifier);
}

Viewer::Viewer() : data_list(1),
                   selected_data_index(0),
                   next_data_id(1),
                   selected_core_index(0),
                   next_core_id(2)

{
    window = nullptr;
    data_list.front().id = 0;

    core_list.emplace_back(ViewerCore());
    core_list.front().id = 1;

    data().set_face_based(false);
}

int Viewer::launch(bool resizable /*= true*/,
                   bool fullscreen /*= false*/,
                   const std::string &name,
                   int windowWidth /*= 0*/,
                   int windowHeight /*= 0*/)
{
    launch_init(resizable, fullscreen, name, windowWidth, windowHeight);
    launch_rendering(true);
    launch_shut();
    return true;
}

int Viewer::launch_init(bool resizable,
                        bool fullscreen,
                        const std::string &name,
                        int windowWidth,
                        int windowHeight)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
    {
        return EXIT_FAILURE;
    }

    glfwWindowHint(GLFW_SAMPLES, 8);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    if (fullscreen)
    {
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        window = glfwCreateWindow(mode->width, mode->height, name.c_str(), monitor, nullptr);
        windowWidth = mode->width;
        windowHeight = mode->height;
    }
    else
    {
        // Set default windows width
        if (windowWidth <= 0 && core_list.size() == 1 && core().viewport[2] > 0)
            windowWidth = core().viewport[2];
        else if (windowWidth <= 0)
            windowWidth = 1280;
        // Set default windows height
        if (windowHeight <= 0 && core_list.size() == 1 && core().viewport[3] > 0)
            windowHeight = core().viewport[3];
        else if (windowHeight <= 0)
            windowHeight = 800;
        window = glfwCreateWindow(windowWidth, windowHeight, name.c_str(), nullptr, nullptr);
    }

    if (!window)
    {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    // Load OpenGL and its extensions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        printf("Failed to load OpenGL and its extensions\n");
        return (-1);
    }

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    __viewer = this;
    glfwSetKeyCallback(window, glfw_key_callback);
    init();
    return true;
}

void Viewer::resize(int w, int h)
{
    core().viewport = Vector4f(0.0, 0.0, w, h);
}

void Viewer::init()
{
}

bool Viewer::launch_rendering(bool loop)
{
    while (!glfwWindowShouldClose(window))
    {
        draw();
        glfwSwapBuffers(window);
        // if (!loop)
        // {
        //     return !glfwWindowShouldClose(window);
        // }
        glfwPollEvents();
    }
    return EXIT_SUCCESS;
}

void Viewer::launch_shut()
{
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Viewer::draw()
{
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);

    int width_window, height_window;
    glfwGetWindowSize(window, &width_window, &height_window);
    resize(width, height);

    for (auto &core : core_list)
    {
        core.clear_framebuffers();
    }

    for (auto &core : core_list)
    {
        for (auto &mesh : data_list)
        {
            if (core.id)
            {
                core.draw(mesh);
            }
        }
    }
}

ViewerCore &Viewer::core(unsigned core_id /*= 0*/)
{
    assert(!core_list.empty() && "core_list should never be empty");
    int core_index;
    if (core_id == 0)
        core_index = selected_core_index;
    else
        core_index = this->core_index(core_id);
    assert((core_index >= 0 && core_index < core_list.size()) && "selected_core_index should be in bounds");
    return core_list[core_index];
}

const ViewerCore &Viewer::core(unsigned core_id /*= 0*/) const
{
    assert(!core_list.empty() && "core_list should never be empty");
    int core_index;
    if (core_id == 0)
        core_index = selected_core_index;
    else
        core_index = this->core_index(core_id);
    assert((core_index >= 0 && core_index < core_list.size()) && "selected_core_index should be in bounds");
    return core_list[core_index];
}

size_t Viewer::core_index(const int id) const
{
    for (size_t i = 0; i < core_list.size(); ++i)
    {
        if (core_list[i].id == id)
            return i;
    }
    return 0;
}

bool Viewer::load_mesh(const MatrixXf &V, const MatrixXi &F)
{
    // Create new data slot and set to selected
    if (!(data().F.rows() == 0 && data().V.rows() == 0))
    {
        append_mesh();
    }
    data().clear();
    data().set_mesh(V, F);

    for (int i = 0; i < core_list.size(); i++)
        core_list[i].align_camera_center(data().V, data().F);
}

int Viewer::append_mesh()
{
    assert(data_list.size() >= 1);
    data_list.emplace_back();
    selected_data_index = data_list.size() - 1;
    data_list.back().id = next_data_id++;
    return data_list.back().id;
}

ViewerData &Viewer::data(int mesh_id)
{
    assert(!data_list.empty() && "data_list should never be empty");
    int index;
    if (mesh_id == -1)
        index = selected_data_index;
    else
        index = mesh_index(mesh_id);

    assert((index >= 0 && index < data_list.size()) &&
           "selected_data_index or mesh_id should be in bounds");
    return data_list[index];
}

const ViewerData &Viewer::data(int mesh_id) const
{
    assert(!data_list.empty() && "data_list should never be empty");
    int index;
    if (mesh_id == -1)
        index = selected_data_index;
    else
        index = mesh_index(mesh_id);

    assert((index >= 0 && index < data_list.size()) &&
           "selected_data_index or mesh_id should be in bounds");
    return data_list[index];
}

size_t Viewer::mesh_index(const int id) const
{
    for (size_t i = 0; i < data_list.size(); ++i)
    {
        if (data_list[i].id == id)
            return i;
    }
    return 0;
}