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
#include <random>


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
    point_light->setIntensity(1.0f);
    point_light->setColor({1,1,0.8f});


    std::vector<std::string> names = {"Sun", "Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus",
                                      "Neptune", "Pluto"};
    // List with Names for all of our planets
    std::vector<float> distance = {0, 0.39f, 0.72f, 1.0f, 1.52f, 5.2f, 9.54f, 19.2f, 30.06f, 35}; // distance in Au

    std::vector<std::vector<float>> color = {{1,1,1}, {1,0.9f,0.7f}, {1,0.7f,0}, {0.5f,0.5f,1}, {1,0,0.1f}, {1,0.6f,0.3f}, {0.8f,0.8f,0.4f}, {0.7f,0.7f,0.9f}, {0.4f,0.5f,0.8f}, {0.9f,0,0.2f}}; // distance in Au



    for (int i = 0; i < names.size(); ++i) {

        std::shared_ptr<GeometryNode> planet = std::make_shared<GeometryNode>(GeometryNode()); // initializing geometry node

        planet->setColor(color[i]);
        planet->setIntensity(1);

        std::shared_ptr<Node> planet_node = std::make_shared<Node>(Node()); // node which contains the geometry node as child

        planet->setName(names[i]); // we set the name of the planet (we draw shapes with names)

        if (i == 0) {
            point_light->addChildren(planet); // add child (sun) to point_light
            planet->setParent(point_light); // add point light as parent for sun

            planet->setLocalTransform(glm::fmat4(0.1, 0, 0, 0,
                                                 0, 0.1, 0, 0,
                                                 0, 0, 0.1, 0,
                                                 0, 0, 0, 0.1)); // transformation for the sun
        } else {
            planet_node->addChildren(planet); // add planet as child to planet node
            planet->setParent(planet_node);
            planet_node->setParent(root);
            root->addChildren(planet_node); // add planet node to root
            // set parents of the two nodes
        }


        // we set local transformation with y = 0 all planets are in the same plane
        planet_node->setLocalTransform(glm::fmat4(0.1f, 0, 0, distance[i] * 1.5f,
                                                  0, 0.1f, 0, 0,
                                                  0, 0, 0.1f, 0,
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


    moon->setColor({1,1,0.7f});

    moon_node->setLocalTransform(glm::fmat4(0.3f, 0, 0, 3.3256957366f, // Moon 0.00256957366 Au
                                            0, 0.3f, 0, 0,
                                            0, 0, 0.3f, 0,
                                            0, 0, 0, 1));


    std::cout << sceneGraph_->printGraph() << "\n";
    // we print the graph of the solar system

    load_planets();

    generate_trails();

    generate_stars();
}


void ApplicationSolar::load_planets() {


    std::vector<std::shared_ptr<Node>> nodes = sceneGraph_->getRoot()->getChildrenList();
    // we get all the children of the root node
    planets_ = std::vector<std::shared_ptr<GeometryNode>>{};
    // we create a vector with all planets we want to draw

    int i = 0;

    do {
        for (auto &&node : nodes[i]->getChildrenList()) {
            nodes.push_back(node);
            // we add all nodes in the scene graph to our nodes vector
        }
        if (!nodes[i]->getName().empty()) {
            // check if node has name, if not, ignore, else add to planets
            planets_.push_back(std::static_pointer_cast<GeometryNode, Node>(nodes[i]));
            // since we need to cast the node (we return a node object), we loose the geometry data
            planets_[planets_.size() - 1]->setGeometry(planet_object);
            // we need to set the geometry again because we get nodes from getChildrenList and not geometry nodes
        }

        i++;
    } while (i < nodes.size());

}

ApplicationSolar::~ApplicationSolar() {
    glDeleteBuffers(1, &planet_object.vertex_BO);
    glDeleteBuffers(1, &planet_object.element_BO);
    glDeleteVertexArrays(1, &planet_object.vertex_AO);

    // Delete all the stars
    glDeleteBuffers(1, &stars_.vertex_BO);
    glDeleteBuffers(1, &stars_.element_BO);
    glDeleteVertexArrays(1, &stars_.vertex_AO);

}

void ApplicationSolar::render() const {

    glUseProgram(m_shaders.at("planet").handle);
    for (int i = 0; i < planets_.size(); ++i) {
        if (planets_[i]->getName() == "Light") { // we dont draw a planet or a ring if we have the light node
            glm::mat4 light_transfrom = planets_[i]->getWorldTransform();
            glUniform1f(m_shaders.at("planet").u_locs.at("light_intensity"),planets_[i]->getIntensity());
            glUniform3f(m_shaders.at("planet").u_locs.at("light_pos"),light_transfrom[3][0],light_transfrom[3][1],light_transfrom[3][2]);
            glUniform3f(m_shaders.at("planet").u_locs.at("color_diffuse_"),0.5f,0.5f,0.5f);
            glUniform3f(m_shaders.at("planet").u_locs.at("color_specular_"),1,1,1);
        }
    }

    for (int i = 0; i < planets_.size(); ++i) {
        if (planets_[i]->getName() != "Light"){ // we dont draw a planet or a ring if we have the light node

            // bind shader to upload uniforms
            glUseProgram(m_shaders.at("planet").handle);
            // loop over all planets, draw each one

            glm::fmat4 model_matrix = planets_[i]->getWorldTransform(); // the model matrix will return the matrix with our world transform
            // -> more calculations than necessary, but its simpler to implement


            glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                               1, GL_FALSE, glm::value_ptr(model_matrix));

            // extra matrix for normal transformation to keep them orthogonal to surface
            glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
            glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                               1, GL_FALSE, glm::value_ptr(normal_matrix));


            // bind the VAO to draw
            glBindVertexArray(planets_[i]->getGeometry().vertex_AO);

            std::vector<float> color = planets_[i]->getColor();

            //std::cout<< "Color: " << color[0] <<" " << color[1] <<" " << color[2] << "\n";

            glUniform3f(m_shaders.at("planet").u_locs.at("camera_position"),m_view_transform[3][0],m_view_transform[3][1],m_view_transform[3][2]);
            glUniform3f(m_shaders.at("planet").u_locs.at("color_ambient_"),color[0],color[1],color[2]);

            // draw bound vertex array using bound shader
            glDrawElements(planets_[i]->getGeometry().draw_mode, planet_object.num_elements, model::INDEX.type, NULL);



            // Draw Trails for each Planet (not for the sun)
            glUseProgram(m_shaders.at("ring").handle); // we switch to the ring shader

            if (planets_[i]->getName() != "Sun" && // we dont want to draw a ring for the sun
            planets_[i]->getParent()->getParent() != nullptr) { // the parent of the parent must not be a nullptr


                glm::fmat4 local = planets_[i]->getParent()->getLocalTransform(); // local transform
                float dist = (float) sqrt(pow(local[0][3],2) + pow(local[1][3],2) + pow(local[2][3],2)); // distance to the origin of rotation (dist to planet it rotates around)

                glUniform1f(m_shaders.at("ring").u_locs.at("dist"),dist); // distance from the planet to its origin of rotation



                model_matrix = planets_[i]->getParent()->getParent()->getWorldTransform(); // world transform of the parent planet = parent parent nodes world transform
                glUniformMatrix4fv(m_shaders.at("ring").u_locs.at("ModelMatrix"),
                                   1, GL_FALSE, glm::value_ptr(model_matrix));


                model_object ring = planets_[i]->getTrail();//　花
                // bind the VAO to draw
                glBindVertexArray(ring.vertex_AO);
                glDrawArrays(ring.draw_mode, GLint(0), ring.num_elements);


                if (planets_[i]->getName() == "Saturn"){
                    int nr_rings = 7;
                    float inner_radius = 1.5f;
                    float outer_radius = 2.5f;
                    for (int j = 0; j < nr_rings; ++j) {


                        float dist = ((float)j/(float)nr_rings); // 0 to 1
                        dist *= (outer_radius-inner_radius); // width of the jth ring
                        dist += inner_radius; // inner radius for dist from center

                        glUniform1f(m_shaders.at("ring").u_locs.at("dist"),dist);

                        model_matrix = planets_[i]->getParent()->getWorldTransform(); // ring around saturn
                        glUniformMatrix4fv(m_shaders.at("ring").u_locs.at("ModelMatrix"),
                                           1, GL_FALSE, glm::value_ptr(model_matrix));

                        model_object ring = planets_[i]->getTrail();//　花
                        // bind the VAO to draw
                        glBindVertexArray(ring.vertex_AO);
                        glDrawArrays(ring.draw_mode, GLint(0), ring.num_elements);
                    }
                }
            }
        }
    }


    glUseProgram(m_shaders.at("star").handle);

    // bind the VAO to draw
    glBindVertexArray(stars_.vertex_AO);
    glDrawArrays(stars_.draw_mode, GLint(0), stars_.num_elements);
}

void ApplicationSolar::uploadView() {
    // bind shader to which to upload unforms
    glUseProgram(m_shaders.at("planet").handle);
    // vertices are transformed in camera space, so camera transform must be inverted
    glm::fmat4 view_matrix = glm::inverse(m_view_transform);
    // upload matrix to gpu
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
                       1, GL_FALSE, glm::value_ptr(view_matrix));
    // bind shader to which to upload unforms
    glUseProgram(m_shaders.at("star").handle);
    glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ViewMatrix"),
                       1, GL_FALSE, glm::value_ptr(view_matrix));
    // bind shader to which to upload unforms
    glUseProgram(m_shaders.at("ring").handle);
    glUniformMatrix4fv(m_shaders.at("ring").u_locs.at("ViewMatrix"),
                       1, GL_FALSE, glm::value_ptr(view_matrix));
}

void ApplicationSolar::uploadProjection() {
    // bind shader to which to upload unforms
    glUseProgram(m_shaders.at("planet").handle);
    // upload matrix to gpu
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
                       1, GL_FALSE, glm::value_ptr(m_view_projection));
    // bind shader to which to upload unforms
    glUseProgram(m_shaders.at("star").handle);
    glUniformMatrix4fv(m_shaders.at("star").u_locs.at("ProjectionMatrix"),
                       1, GL_FALSE, glm::value_ptr(m_view_projection));
    // bind shader to which to upload unforms
    glUseProgram(m_shaders.at("ring").handle);
    glUniformMatrix4fv(m_shaders.at("ring").u_locs.at("ProjectionMatrix"),
                       1, GL_FALSE, glm::value_ptr(m_view_projection));
}

// update uniform locations
void ApplicationSolar::uploadUniforms() {
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

    m_shaders.at("planet").u_locs["light_pos"] = -1;
    m_shaders.at("planet").u_locs["light_intensity"] = -1;
    m_shaders.at("planet").u_locs["color_ambient_"] = -1;
    m_shaders.at("planet").u_locs["color_diffuse_"] = -1;
    m_shaders.at("planet").u_locs["color_specular_"] = -1;
    m_shaders.at("planet").u_locs["camera_position"] = -1;
    m_shaders.at("planet").u_locs["toon_shading"] = -1;


    // store shader program objects in container
    m_shaders.emplace("star", shader_program{{{GL_VERTEX_SHADER, m_resource_path + "shaders/star.vert"},
                                                     {GL_FRAGMENT_SHADER, m_resource_path + "shaders/star.frag"}}});
    // request uniform locations for shader program
    //m_shaders.at("star").u_locs["ModelMatrix"] = -1;
    m_shaders.at("star").u_locs["ViewMatrix"] = -1;
    m_shaders.at("star").u_locs["ProjectionMatrix"] = -1;

    // ring shader

    // store shader program objects in container
    m_shaders.emplace("ring", shader_program{{{GL_VERTEX_SHADER, m_resource_path + "shaders/ring.vert"},
                                                     {GL_FRAGMENT_SHADER, m_resource_path + "shaders/ring.frag"}}});
    // request uniform locations for shader program
    m_shaders.at("ring").u_locs["ModelMatrix"] = -1;
    m_shaders.at("ring").u_locs["ViewMatrix"] = -1;
    m_shaders.at("ring").u_locs["ProjectionMatrix"] = -1;
    m_shaders.at("ring").u_locs["dist"] = -1;

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
    float speed = 0.1f;
    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        // moving ahead
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, -speed});
    }  if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        // moving backwards
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{0.0f, 0.0f, speed});
    }  if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        // movement to the left
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{-speed, 0.0f, 0.0f});
    }  if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        // movement to the right
        m_view_transform = glm::translate(m_view_transform, glm::fvec3{speed, 0.0f, 0.0f});
    }
    uploadView();
    glUseProgram(m_shaders.at("planet").handle);
    if (key == GLFW_KEY_1) {
        glUniform1b(m_shaders.at("planet").u_locs.at("toon_shading"),true);
    }else if (key == GLFW_KEY_2){
        glUniform1b(m_shaders.at("planet").u_locs.at("toon_shading"),false);
    }
}

//handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {
    // mouse handling
    m_view_transform = glm::rotate(m_view_transform, glm::radians(-(float) pos_x / 60),
                                   glm::vec3{0.0f, 1.0f, 0.0f}); // horizontal rotation
    m_view_transform = glm::rotate(m_view_transform, glm::radians(-(float) pos_y / 60),
                                   glm::vec3{1.0f, 0.0f, 0.0f}); // vertical rotation
    uploadView();
}

//handle resizing
void ApplicationSolar::resizeCallback(unsigned width, unsigned height) {
    // recalculate projection matrix for new aspect ration
    m_view_projection = utils::calculate_projection_matrix(float(width) / float(height));
    // upload new projection matrix
    uploadProjection();
}

void ApplicationSolar::generate_stars() {


    int number_stars = 2000;
    std::vector<GLfloat> data;
    data.reserve(number_stars * 6 * sizeof(float));
    std::default_random_engine generator;

    for (int i = 0; i < number_stars; ++i) {
        std::uniform_real_distribution<float> distribution(0.0f, 1.0f);

        float x = (distribution(generator) * 2 - 1);
        float y = (distribution(generator) * 2 - 1);
        float z = (distribution(generator) * 2 - 1);

        float len = (float) sqrt(x * x + y * y + z * z);

        x /= len; // skydome
        y /= len; // skydome
        z /= len; // skydome

        x *= 30;
        y *= 30;
        z *= 30;

        x -= (distribution(generator) * 2 - 1) * 2;
        y -= (distribution(generator) * 2 - 1) * 2;
        z -= (distribution(generator) * 2 - 1) * 2;

        // Position
        data.push_back(x);
        data.push_back(y);
        data.push_back(z);

        float brightness = 0.0f;

        float r = (distribution(generator) * (1 - brightness) + brightness);
        float g = (distribution(generator) * (1 - brightness) + brightness);
        float b = (distribution(generator) * (1 - brightness) + brightness);

        // Color
        data.push_back(r);
        data.push_back(g);
        data.push_back(b);
    }

    glGenVertexArrays(1, &stars_.vertex_AO);
    glBindVertexArray(stars_.vertex_AO);

    glGenBuffers(1, &stars_.vertex_BO);
    glBindBuffer(GL_ARRAY_BUFFER, stars_.vertex_BO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(),
                 data.data(), GL_STATIC_DRAW);

    // attribute array for positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, GLsizei(6 * sizeof(float)), nullptr);

    // attribute array for rgb colour
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, GLsizei(6 * sizeof(float)),
                          (void *) (sizeof(float) * 3));

    stars_.draw_mode = GL_POINTS;
    stars_.num_elements = GLsizei(number_stars);

}


void ApplicationSolar::generate_trails() {

    model_object ring_object;
    std::vector<GLfloat> data;

    int resolution = 100;
    data.reserve(resolution * 6 * sizeof(float));

    for (int i = 0; i < resolution; ++i) {

        // ring has a radius of 1
        float x = (float) cos((4 * acos(0.0) * (float) i) / (float) resolution);
        float y = 0;
        float z = (float) sin((4 * acos(0.0) * (float) i) / (float) resolution);

        // Position
        data.push_back(x);
        data.push_back(y);
        data.push_back(z);

        //float brightness = 0.4f;

        float r = 1;//(float) i / (float) resolution;
        float g = 1;//(float) i / (float) resolution;
        float b = 1;//(float) i / (float) resolution;

        // Color
        data.push_back(r);
        data.push_back(g);
        data.push_back(b);
    }
    // generate model (ring)
    glGenVertexArrays(1, &ring_object.vertex_AO);
    glBindVertexArray(ring_object.vertex_AO);

    glGenBuffers(1, &ring_object.vertex_BO);
    glBindBuffer(GL_ARRAY_BUFFER, ring_object.vertex_BO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * data.size(),
                 data.data(), GL_STATIC_DRAW);

    // attribute array for positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, GLsizei(6 * sizeof(float)), nullptr);

    // attribute array for rgb colour
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, GLsizei(6 * sizeof(float)),
                          (void *) (sizeof(float) * 3));

    ring_object.draw_mode = GL_LINE_LOOP;
    ring_object.num_elements = GLsizei(resolution);

    for (int i = 0; i < planets_.size(); ++i) {

        planets_[i]->setTrail(ring_object);

        std::cout << ring_object.vertex_AO << "\n";
        std::cout << ring_object.vertex_BO << "\n";
        std::cout << ring_object.num_elements << "\n";
        std::cout << ring_object.element_BO << "\n";
    }
}

// exe entry point
int main(int argc, char *argv[]) {
    Application::run<ApplicationSolar>(argc, argv, 3, 2);
}