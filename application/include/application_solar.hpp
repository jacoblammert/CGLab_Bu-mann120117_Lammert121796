#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"
#include "../source/Ass1/Node.h"
#include "../source/Ass1/SceneGraph.h"

// gpu representation of model
class ApplicationSolar : public Application {
public:
    // allocate and initialize objects
    ApplicationSolar(std::string const &resource_path);

    // free allocated objects
    ~ApplicationSolar();

    // react to key input
    void keyCallback(int key, int action, int mods);

    //handle delta mouse movement input
    void mouseCallback(double pos_x, double pos_y);

    //handle resizing
    void resizeCallback(unsigned width, unsigned height);

    // draw all objects
    void render() const;

    SceneGraph *sceneGraph_; // scene graph, contains the scene

    std::vector<std::shared_ptr<GeometryNode>> planets_;

    model_object stars_;

    model_object full_screenquad_;

    //framebuffer_object framebuffer_obj;

protected:
    void load_planets();

    void load_textures();

    void generate_skybox();

    void generate_stars();

    void generate_trails();

    void generate_screen();


    void initializeShaderPrograms();

    void initializeGeometry();

    // update uniform values
    void uploadUniforms();

    // upload projection matrix
    void uploadProjection();

    // upload view matrix
    void uploadView();

    void loadSkyboxTextures(std::string name);

    // cpu representation of model
    model_object planet_object;

    GLenum skybox_texture_index;
    int skybox_counter_ = 0;


    model_object skybox_object;

    bool generate_framebuffer(unsigned width, unsigned height);

    unsigned screen_width;
    unsigned screen_height;
    framebuffer_object framebuffer_object_;

    std::vector<GLboolean> post_processing_effects_ = {false, false, false, false};
    std::vector<GLenum> skybox_texturers_ = {};
    std::vector<texture_object> skybox_texturer_objects = {};
    int active_skybox_texture_object = 0;

    // camera transform matrix
    glm::fmat4 m_view_transform;
    // camera projection matrix
    glm::fmat4 m_view_projection;
    // root Node (contains camera)
};

#endif