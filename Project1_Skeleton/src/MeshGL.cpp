#include "GLUtil.h"
#include "MeshGL.h"
#include <string>
#include <glad/glad.h>

using namespace std;

MeshGL::MeshGL()
{
}

void MeshGL::init()
{
    if (is_initialized)
        return;
    is_initialized = true;

    //Generate and Bind Buffers
    init_buffers();

    //GLSL
    //Compile shader program
    string vs_code = load_glsl("./resources/simple.vs");
    string fs_code = load_glsl("./resources/simple.fs");
    create_shader_program(vs_code, fs_code, shader_mesh);
}

void MeshGL::init_buffers()
{
    // Mesh: Vertex Array Object & Buffer objects
    glGenVertexArrays(1, &vao_mesh);
    glBindVertexArray(vao_mesh);
    glGenBuffers(1, &vbo_V);
    glGenBuffers(1, &vbo_V_normals);
    glGenBuffers(1, &vbo_V_ambient);
    glGenBuffers(1, &vbo_V_diffuse);
    glGenBuffers(1, &vbo_V_specular);
    glGenBuffers(1, &vbo_F);

    dirty = MeshGL::DIRTY_ALL;
}

void MeshGL::bind_mesh()
{
    glBindVertexArray(vao_mesh);
    glUseProgram(shader_mesh);
    cout << __FILE__ << " " << __LINE__ << ": V_vbo: " << V_vbo << endl;
    bind_vertex_attrib_array(shader_mesh, "position", vbo_V, V_vbo, dirty & MeshGL::DIRTY_POSITION);
    bind_vertex_attrib_array(shader_mesh, "normal", vbo_V_normals, V_normals_vbo, dirty & MeshGL::DIRTY_NORMAL);
    bind_vertex_attrib_array(shader_mesh, "Ka", vbo_V_ambient, V_ambient_vbo, dirty & MeshGL::DIRTY_AMBIENT);
    bind_vertex_attrib_array(shader_mesh, "Kd", vbo_V_diffuse, V_diffuse_vbo, dirty & MeshGL::DIRTY_DIFFUSE);
    bind_vertex_attrib_array(shader_mesh, "Ks", vbo_V_specular, V_specular_vbo, dirty & MeshGL::DIRTY_SPECULAR);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo_F);
    if (dirty & MeshGL::DIRTY_FACE)
    {
        cout << __FILE__ << " " << __LINE__ << ": F_vbo: " << F_vbo << endl;
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned) * F_vbo.size(), F_vbo.data(), GL_DYNAMIC_DRAW);
    }

    dirty &= ~MeshGL::DIRTY_MESH;
}

void MeshGL::free()
{
    const auto free = [](GLuint &id) {
        if (id)
        {
            destroy_shader_program(id);
            id = 0;
        }
    };

    if (is_initialized)
    {
        free(shader_mesh);
        free_buffers();
    }
}

void MeshGL::free_buffers()
{
    if (is_initialized)
    {
        glDeleteVertexArrays(1, &vao_mesh);

        glDeleteBuffers(1, &vbo_V);
        glDeleteBuffers(1, &vbo_V_normals);
        glDeleteBuffers(1, &vbo_V_ambient);
        glDeleteBuffers(1, &vbo_V_diffuse);
        glDeleteBuffers(1, &vbo_V_specular);
        glDeleteBuffers(1, &vbo_F);
    }
}

void MeshGL::draw_mesh(bool show_faces)
{
    // glPolygonMode(GL_FRONT_AND_BACK, show_faces ? GL_FILL : GL_LINE);

    // /* Avoid Z-buffer fighting between filled triangles & wireframe lines */
    // if (show_faces)
    // {
    //     glEnable(GL_POLYGON_OFFSET_FILL);
    //     glPolygonOffset(1.0, 1.0);
    // }
    glDrawElements(GL_TRIANGLES, 3 * F_vbo.rows(), GL_UNSIGNED_INT, 0);

    // glDisable(GL_POLYGON_OFFSET_FILL);
    // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}