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

protected:
    void load_planets();

    void load_textures();

    void generate_skybox();

    void generate_stars();

    void generate_trails();


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
    texture_object skybox_texture;

    model_object skybox_object;

    // camera transform matrix
    glm::fmat4 m_view_transform;
    // camera projection matrix
    glm::fmat4 m_view_projection;
    // root Node (contains camera)
};

#endif