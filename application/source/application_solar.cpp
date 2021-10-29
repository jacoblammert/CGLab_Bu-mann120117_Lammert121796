#include "application_solar.hpp"
#include "window_handler.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"
#include "Ass1/GeometryNode.h"
#include "Ass1/CameraNode.h"

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding 
using namespace gl;

//dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE

#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>


ApplicationSolar::ApplicationSolar(std::string const &resource_path)
        : Application{resource_path}, planet_object{},
          m_view_transform{glm::translate(glm::fmat4{}, glm::fvec3{0.0f, 0.0f, 4.0f})} // camera
        , m_view_projection{utils::calculate_projection_matrix(initial_aspect_ratio)} // camera
{

    initializeGeometry();
    initializeShaderPrograms();

    root = std::make_shared<Node>(Node());


    std::shared_ptr<CameraNode> camera = std::make_shared<CameraNode>(CameraNode());


    std::shared_ptr<GeometryNode> planet_1 = std::make_shared<GeometryNode>(GeometryNode());
    std::shared_ptr<Node> planet_node_1 = std::make_shared<Node>(Node());


    std::shared_ptr<Node> moon_node = std::make_shared<Node>(Node());
    std::shared_ptr<GeometryNode> moon = std::make_shared<GeometryNode>(GeometryNode());

    moon_node->addChildren(moon);

    //planet_node_1->addChildren(moon_node);
    planet_node_1->addChildren(planet_1);

    planet_1->setLocalTransform(glm::fmat4(1,0,0,1,
                                           0,1,0,0,
                                           0,0,1,0,
                                           0,0,0,1));


    moon->setLocalTransform(glm::fmat4(1,0,0,1,
                                           0,1,0,1,
                                           0,0,1,1,
                                           0,0,0,1));


    root->addChildren(planet_node_1);


}

ApplicationSolar::~ApplicationSolar() {
    glDeleteBuffers(1, &planet_object.vertex_BO);
    glDeleteBuffers(1, &planet_object.element_BO);
    glDeleteVertexArrays(1, &planet_object.vertex_AO);
}

void ApplicationSolar::render() const {
    // bind shader to upload uniforms
    glUseProgram(m_shaders.at("planet").handle);

    glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime()), glm::fvec3{0.0f, 1.0f, 0.0f});
    model_matrix = glm::translate(model_matrix, glm::fvec3{0.0f, 0.0f, -1.0f});

    // root node get

    std::vector<std::shared_ptr<Node>> nodes = root->getChildrenList();
    std::vector<std::shared_ptr<GeometryNode>> planets = std::vector<std::shared_ptr<GeometryNode>>{};


    int i = 0;
    while (i < nodes.size()) {

        std::cout<<nodes.size()<<"\n";

        //std::cout << typeid(nodes[i]).name() << "\n";

        if (typeid(nodes[i]) == typeid(std::shared_ptr<Node>)) {
            //std::vector<Node*> =
            //std::vector<Node *> children = nodes[i]->getChildrenList();

            std::cout << "Children: " << nodes[i]->getChildrenList().size() << "\n";

            for (auto &&node : nodes[i]->getChildrenList()) {
                std::cout << typeid(node).name() << "\n";

                try {
                    //Test for GeometryNodes
                    //std::cout << "Test cast\n";
                    std::shared_ptr<GeometryNode> mDerived = std::static_pointer_cast<GeometryNode, Node>(node);

                    //std::cout << "Test cast complete\n";

                    if (typeid(mDerived) == typeid(std::shared_ptr<GeometryNode>)) {
                        //std::cout << "Test\n";
                        mDerived->setGeometry(planet_object);
                        //std::cout << "Set geometry\n";
                        planets.push_back(mDerived);
                        //std::cout << "add to list\n";
                        //continue;
                    }
                } catch (const std::invalid_argument &e) {

                    nodes.push_back(node);
                }
                //TODO PointLightNode "test"
                //TODO singleton in SceneGraph


                // we tested for all classes, if we are here, it must be a node



            }
        }
        i++;
    }
    std::cout<<planets.size()<<"\n";



    std::cout << "planets size: " << planets.size() << "\n";

    for (int i = 0; i < planets.size(); ++i) {


        glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime()), glm::fvec3{0.0f, 1.0f, 0.0f});
        model_matrix = glm::translate(model_matrix, glm::fvec3{0.0f, 0.0f, -1.0f});



        glm::fmat4 rotation = glm::rotate(glm::fmat4 {}, float(glfwGetTime()), glm::fvec3{0.0f, 1.0f, 0.0f})
                * planets[i]->getLocalTransform();


        model_matrix = planets[i]->getWorldTransform() * rotation;
        planets[i]->setWorldTransform(model_matrix);




        glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                           1, GL_FALSE, glm::value_ptr(model_matrix));

        // extra matrix for normal transformation to keep them orthogonal to surface
        glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
        glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                           1, GL_FALSE, glm::value_ptr(normal_matrix));

        std::cout << "Geometry\n" << planets[i]->getGeometry().element_BO;
        // bind the VAO to draw
        glBindVertexArray(planets[i]->getGeometry().vertex_AO);

        std::cout << "Geometry2\n";
        // draw bound vertex array using bound shader
        glDrawElements(planets[i]->getGeometry().draw_mode, planet_object.num_elements, model::INDEX.type, NULL);
    }

}

void ApplicationSolar::uploadView() {
    // vertices are transformed in camera space, so camera transform must be inverted
    glm::fmat4 view_matrix = glm::inverse(m_view_transform);
    // upload matrix to gpu
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
                       1, GL_FALSE, glm::value_ptr(view_matrix));
}

void ApplicationSolar::uploadProjection() {
    // upload matrix to gpu
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
                       1, GL_FALSE, glm::value_ptr(m_view_projection));
}

// update uniform locations
void ApplicationSolar::uploadUniforms() {
    // bind shader to which to upload unforms
    glUseProgram(m_shaders.at("planet").handle);
    // upload uniform values to new locations
    uploadView();
    uploadProjection();
}

///////////////////////////// intialisation functions /////////////////////////
// load shader sources
void ApplicationSolar::initializeShaderPrograms() {
    // store shader program objects in container
    m_shaders.emplace("planet", shader_program{{{GL_VERTEX_SHADER, m_resource_path + "shaders/simple.vert"},
                                                       {GL_FRAGMENT_SHADER, m_resource_path + "shaders/simple.frag"}}});
    // request uniform locations for shader program
    m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
    m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
    m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
    m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;
}

// load models
void ApplicationSolar::initializeGeometry() {
    model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);

    // generate vertex array object
    glGenVertexArrays(1, &planet_object.vertex_AO);
    // bind the array for attaching buffers
    glBindVertexArray(planet_object.vertex_AO);

    // generate generic buffer
    glGenBuffers(1, &planet_object.vertex_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ARRAY_BUFFER, planet_object.vertex_BO);
    // configure currently bound array buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * planet_model.data.size(), planet_model.data.data(), GL_STATIC_DRAW);

    // activate first attribute on gpu
    glEnableVertexAttribArray(0);
    // first attribute is 3 floats with no offset & stride
    glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, planet_model.vertex_bytes,
                          planet_model.offsets[model::POSITION]);
    // activate second attribute on gpu
    glEnableVertexAttribArray(1);
    // second attribute is 3 floats with no offset & stride
    glVertexAttribPointer(1, model::NORMAL.components, model::NORMAL.type, GL_FALSE, planet_model.vertex_bytes,
                          planet_model.offsets[model::NORMAL]);

    // generate generic buffer
    glGenBuffers(1, &planet_object.element_BO);
    // bind this as an vertex array buffer containing all attributes
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planet_object.element_BO);
    // configure currently bound array buffer
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * planet_model.indices.size(), planet_model.indices.data(),
                 GL_STATIC_DRAW);

    // store type of primitive to draw
    planet_object.draw_mode = GL_TRIANGLES;
    // transfer number of indices to model object
    planet_object.num_elements = GLsizei(planet_model.indices.size());
}

///////////////////////////// callback functions for window events ////////////
// handle key input
void ApplicationSolar::keyCallback(int key, int action, int mods) {
    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -0.1f});
        uploadView();
    } else if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 0.1f});
        uploadView();
    } else if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{-0.1f, 0.0f, 0.0f});
        uploadView();
    } else if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.1f, 0.0f, 0.0f});
        uploadView();
    }
}

//handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {
    // mouse handling
    // glm::fmat4 model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime()), glm::fvec3{0.0f, 1.0f, 0.0f});
    //m_view_transform = glm::rotate(m_view_transform,float(pos_x), glm::fvec3{0, 1.0f, 0});
    //m_view_transform = glm::rotate(m_view_transform,float(pos_y), glm::fvec3{1, 0, 0});

}

//handle resizing
void ApplicationSolar::resizeCallback(unsigned width, unsigned height) {
    // recalculate projection matrix for new aspect ration
    m_view_projection = utils::calculate_projection_matrix(float(width) / float(height));
    // upload new projection matrix
    uploadProjection();
}


// exe entry point
int main(int argc, char *argv[]) {


    Application::run<ApplicationSolar>(argc, argv, 3, 2);
}