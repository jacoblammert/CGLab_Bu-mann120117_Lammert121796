#include "application_solar.hpp"
#include "window_handler.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"
#include "Ass1/GeometryNode.h"
#include "Ass1/CameraNode.h"
#include "Ass1/SceneGraph.h"
#include "Ass1/PointLightNode.h"

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


    sceneGraph_ = new SceneGraph(); // we initialize the scingelton object with our root node
    std::shared_ptr<Node> root = sceneGraph_->getRoot(); // we get the root node


    // Camera, Light Node

    std::shared_ptr<CameraNode> camera = std::make_shared<CameraNode>(CameraNode());
    std::shared_ptr<PointLightNode> point_light = std::make_shared<PointLightNode>(PointLightNode());

    root->addChildren(camera);
    root->addChildren(point_light);
    point_light->setParent(root);// set parent of point light

    point_light->setName("Light"); // for debugging




    std::vector<std::string> names = {"Sun","Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus","Neptune","Pluto"};
    // List with Names for all of our planets

    // values for different positions
    std::vector<int> position = {10, -5, -2, 6, -4, 20, 8,-1, 2, 4, -3};

    for (int i = 0; i < names.size(); ++i) {

        std::shared_ptr<GeometryNode> planet = std::make_shared<GeometryNode>(GeometryNode()); // initializing geometry node


        std::shared_ptr<Node> planet_node = std::make_shared<Node>(Node()); // node which contains the geometry node as child

        planet->setName(names[i]); // we set the name of the planet (we draw shapes with names)

        if(i == 0){
            point_light->addChildren(planet); // add child (sun) to point_light
            planet->setParent(point_light); // add point light as parent for sun

            planet->setLocalTransform(glm::fmat4(2, 0, 0, 0,
                                                 0, 2, 0, 0,
                                                 0, 0, 2, 0,
                                                 0, 0, 0, 1)); // transformation for the sun
        }else{
            planet_node->addChildren(planet); // add planet as child to planet node
            planet->setParent(planet_node);
            planet_node->setParent(root);
            root->addChildren(planet_node); // add planet node to root
            // set parents of the two nodes
        }




        // unique position for each planet
        float x = position[(i*(i+1)) % position.size()];
        float z = position[i % position.size()];
        float scale = 1; // all planets have the same size

        if (i == 0){
            x = 0;
            z = 0;
            scale = 2;
            // planet 0 is the sun -> size is different, pos = (0,0,0)
        }

        // we set local transformation with y = 0 all planets are in the same plane
        planet_node->setLocalTransform(glm::fmat4(scale, 0, 0, x,
                                                  0, scale, 0, 0,
                                                  0, 0, scale, z,
                                                  0, 0, 0, 1));
    }

    // moon + moon node initialization
    std::shared_ptr<GeometryNode> moon = std::make_shared<GeometryNode>(GeometryNode());
    std::shared_ptr<Node> moon_node = std::make_shared<Node>(Node());

    // we search for the earth and get its parent (is not equal to the root node)
    std::shared_ptr<Node> earth_node = root->getChildren("Earth")->getParent();
    // earth_node is the earth hold node


    // set children and parents accordingly
    earth_node->addChildren(moon_node);
    moon_node->setParent(earth_node);
    moon_node->addChildren(moon);
    moon->setParent(moon_node);



    moon->setName("Moon"); // set the name for the moon (to draw it)

    /**/earth_node->setLocalTransform(glm::fmat4(1, 0, 0, 10, // Earth
                                                 0, 1, 0, 0,
                                                0, 0, 1, 0,
                                                0, 0, 0, 1));
/**/

    moon_node->setLocalTransform(glm::fmat4(0.1f, 0, 0, 3, // Moon
                                            0, 0.1f, 0, 0,
                                            0, 0, 0.1f, 0,
                                            0, 0, 0, 1));



    std::cout << sceneGraph_->printGraph() << "\n";
    // we print the graph of the solar system
}

ApplicationSolar::~ApplicationSolar() {
    glDeleteBuffers(1, &planet_object.vertex_BO);
    glDeleteBuffers(1, &planet_object.element_BO);
    glDeleteVertexArrays(1, &planet_object.vertex_AO);
}

void ApplicationSolar::render() const {
    // bind shader to upload uniforms
    glUseProgram(m_shaders.at("planet").handle);

    std::shared_ptr<Node> root = sceneGraph_->getRoot(); // we get the root node of the scene graph

    std::vector<std::shared_ptr<Node>> nodes = root->getChildrenList();
    // we get all the children of the root node
    std::vector<std::shared_ptr<GeometryNode>> planets = std::vector<std::shared_ptr<GeometryNode>>{};
    // we create a vector with all planets we want to draw

    int i = 0;

    do {
        for (auto &&node : nodes[i]->getChildrenList()) {
            nodes.push_back(node);
            // we add all nodes in the scene graph to our nodes vector
        }
        if (!nodes[i]->getName().empty()) {
            // check if node has name, if not, ignore, else add to planets
            planets.push_back(std::static_pointer_cast<GeometryNode, Node>(nodes[i]));
            // since we need to cast the node (we return a node object), we loose the geometry data
            planets[planets.size() - 1]->setGeometry(planet_object);
            // we need to set the geometry again because we get nodes from getChildrenList and not geometry nodes
        }

        i++;
    } while (i < nodes.size());


    for (int i = 0; i < planets.size(); ++i) {
        // loop over all planets, draw each one


        glm::fmat4 model_matrix = planets[i]->getWorldTransform(); // the model matrix will return the matrix with our world transform
        // -> more calculations than necessary, but its simpler to implement


        glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                           1, GL_FALSE, glm::value_ptr(model_matrix));

        // extra matrix for normal transformation to keep them orthogonal to surface
        glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
        glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                           1, GL_FALSE, glm::value_ptr(normal_matrix));


        // bind the VAO to draw
        glBindVertexArray(planets[i]->getGeometry().vertex_AO);




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
        // moving ahead
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -0.1f});
        uploadView();
    } else if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        // moving backwards
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, 0.1f});
        uploadView();
    } else if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        // movement to the left
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{-0.1f, 0.0f, 0.0f});
        uploadView();
    } else if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        // movement to the right
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.1f, 0.0f, 0.0f});
        uploadView();
    }
}

//handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {
    // mouse handling
    m_view_transform = glm::rotate(m_view_transform, glm::radians(-(float) pos_x / 60), glm::vec3{0.0f, 1.0f, 0.0f}); // horizontal rotation
    m_view_transform = glm::rotate(m_view_transform, glm::radians(-(float) pos_y / 60), glm::vec3{1.0f, 0.0f, 0.0f}); // vertical rotation
    uploadView();
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