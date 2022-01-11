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
#include <texture_loader.hpp>
#include <fstream>


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
    point_light->setColor(std::vector<float>{0.5f,0.5f,0.5f});


    std::vector<std::string> names = {"Sun", "Mercury", "Venus", "Earth", "Mars", "Jupiter", "Saturn", "Uranus", "Neptune", "Pluto"};
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
    load_textures();
    std::cout<<"Textures loaded!"<<"\n";
    generate_skybox(); // TODO add skyboxes to array
    generate_framebuffer(640,480);
    generate_screen();
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

    ////////////////////////////////////////////////////////////////////////////////////

    glUseProgram(m_shaders.at("framebuffer").handle);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_.handle);
    glClearColor(0.1f,0.1f,0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    ////////////////////////////////////////////////////////////////////////////////////

    glUseProgram(m_shaders.at("skybox").handle);
    glDepthFunc(GL_ALWAYS); // we always want to draw the skybox

    // bind the VAO to draw
    glBindVertexArray(skybox_object.vertex_AO);
    glActiveTexture(skybox_texture_index); // activate the texture

    // add sampler
    int samplerLocation = glGetUniformLocation(m_shaders.at("skybox").handle,"skybox_texture");
    glUniform1i(samplerLocation, skybox_texturer_objects[active_skybox_texture_object].handle);

    // draw
    glDrawArrays(GL_TRIANGLES, 0, skybox_object.num_elements); // draw skybox one by one
    glDepthFunc(GL_LESS); // change depth dunction back to the normal one

    glUseProgram(m_shaders.at("planet").handle);
    for (int i = 0; i < planets_.size(); ++i) {
        if (planets_[i]->getName() == "Light") { // we dont draw a planet or a ring if we have the light node
            glm::mat4 light_transfrom = planets_[i]->getWorldTransform(); // transform matrix of the light
            glUniform1f(m_shaders.at("planet").u_locs.at("light_intensity"),planets_[i]->getIntensity()); // intensity of the light
            glUniform3f(m_shaders.at("planet").u_locs.at("light_pos"),light_transfrom[3][0],light_transfrom[3][1],light_transfrom[3][2]); // position of the light
            //glUniform3f(m_shaders.at("planet").u_locs.at("color_diffuse_"),0.5f,0.5f,0.5f); // diffuse color of the light (setting caused memory access violation error earlier)
            glUniform3f(m_shaders.at("planet").u_locs.at("color_specular_"),1,1,1); // specular color of the light
        }
    }
    int count = 0;
    for (int i = 0; i < planets_.size(); ++i) {
        if (planets_[i]->getName() != "Light"){ // we dont draw a planet or a ring if we have the light node

            // bind shader to upload uniforms
            glUseProgram(m_shaders.at("planet").handle);
            // loop over all planets, draw each one

            glUniform1b(m_shaders.at("planet").u_locs.at("sun"),false);
            if (planets_[i]->getName() == "Sun"){
                glUniform1b(m_shaders.at("planet").u_locs.at("sun"),true);
            }
            glm::fmat4 model_matrix = planets_[i]->getWorldTransform(); // the model matrix will return the matrix with our world transform
            // -> more calculations than necessary, but its simpler to implement


            glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                               1, GL_FALSE, glm::value_ptr(model_matrix));

            // bind the VAO to draw
            glBindVertexArray(planet_object.vertex_AO);

            std::vector<float> color = planets_[i]->getColor();

            //std::cout<< "Color: " << color[0] <<" " << color[1] <<" " << color[2] << "\n";

            glUniform3f(m_shaders.at("planet").u_locs.at("camera_position"),m_view_transform[3][0],m_view_transform[3][1],m_view_transform[3][2]);
            //glUniform3f(m_shaders.at("planet").u_locs.at("color_ambient_"),color[0],color[1],color[2]);

            // draw bound vertex array using bound shader


            // bind the VAO to draw
            glBindVertexArray(planet_object.vertex_AO);

            texture_object texture = planets_[i]->getTexture();

            //std::cout<< "Planet: " << planets_[i]->getName()<<"\n";

            glActiveTexture(GL_TEXTURE1 + count);
            count++;
            glBindTexture(texture.target, texture.handle);
            // add sampler

            glUniform1i(glGetUniformLocation(m_shaders.at("planet").handle, "texture_"), texture.handle);


            texture_object texture_normal = planets_[i]->getTextureNormal();

            //std::cout<< "Planet: " << planets_[i]->getName()<<"\n";

            glActiveTexture(GL_TEXTURE1 + count);
            count++;
            glBindTexture(texture_normal.target, texture_normal.handle);
            // add sampler

            glUniform1i(glGetUniformLocation(m_shaders.at("planet").handle, "texture_normal"), texture_normal.handle);

            glDrawElements(planets_[i]->getGeometry().draw_mode, planet_object.num_elements, model::INDEX.type, NULL);



            // Draw Trails for each Planet (not for the sun)
            glUseProgram(m_shaders.at("ring").handle); // we switch to the ring shader

            if (planets_[i]->getName() != "Sun" && // we dont want to draw a ring for the sun
            planets_[i]->getParent()->getParent() != nullptr) { // the parent of the parent must not be a nullptr


                glm::fmat4 local = planets_[i]->getParent()->getLocalTransform(); // local transform
                float dist = (float) sqrt(pow(local[0][3],2) + pow(local[1][3],2) + pow(local[2][3],2)); // distance to the origin of rotation (dist to planet it rotates around)

                glUniform1f(m_shaders.at("ring").u_locs.at("dist"),dist); // distance from the planet to its origin of rotation
                glUniform1f(m_shaders.at("ring").u_locs.at("angle"),-float((0.02f * 3.14 * glfwGetTime()) / pow(local[0][3]+1,3)));


                model_matrix = planets_[i]->getParent()->getParent()->getWorldTransform(); // world transform of the parent planet = parent parent nodes world transform
                glUniformMatrix4fv(m_shaders.at("ring").u_locs.at("ModelMatrix"),
                                   1, GL_FALSE, glm::value_ptr(model_matrix));


                model_object ring = planets_[i]->getTrail();//　花
                // bind the VAO to draw
                glBindVertexArray(ring.vertex_AO);
                glDrawArrays(ring.draw_mode, GLint(0), ring.num_elements);


                if (planets_[i]->getName() == "Saturn"){
                    int nr_rings = 10;
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

    /////////////////////////////////////////////////////////////////////////////////

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.0f,1.0f,1.0f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    glUseProgram(m_shaders.at("framebuffer").handle);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,framebuffer_object_.texture_obj.handle);

    glUniform1i(glGetUniformLocation(m_shaders.at("framebuffer").handle,"screenTexture"),0);

    // set texture coordinates
    glBindVertexArray(full_screenquad_.vertex_AO);
    glDrawArrays(full_screenquad_.draw_mode, 0, full_screenquad_.num_elements);

    /////////////////////////////////////////////////////////////////////////////////////
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

    glUseProgram(m_shaders.at("skybox").handle);
    glUniformMatrix4fv(m_shaders.at("skybox").u_locs.at("ViewMatrix"),
                       1, GL_FALSE, glm::value_ptr(view_matrix));

    glUseProgram(m_shaders.at("framebuffer").handle);
    glUniform2f(m_shaders.at("framebuffer").u_locs.at("textureSize"),screen_width,screen_height);
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

    glUseProgram(m_shaders.at("skybox").handle);
    glUniformMatrix4fv(m_shaders.at("skybox").u_locs.at("ProjectionMatrix"),
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
    m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
    m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
    m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;

    m_shaders.at("planet").u_locs["light_pos"] = -1;
    m_shaders.at("planet").u_locs["light_intensity"] = -1;
    m_shaders.at("planet").u_locs["color_specular_"] = -1;
    m_shaders.at("planet").u_locs["camera_position"] = -1;
    m_shaders.at("planet").u_locs["toon_shading"] = -1;
    m_shaders.at("planet").u_locs["sun"] = -1;
    m_shaders.at("planet").u_locs["texture_"] = -1;
    m_shaders.at("planet").u_locs["texture_normal"] = -1;


    // store shader program objects in container
    m_shaders.emplace("star", shader_program{{{GL_VERTEX_SHADER, m_resource_path + "shaders/star.vert"},
                                                     {GL_FRAGMENT_SHADER, m_resource_path + "shaders/star.frag"}}});
    // request uniform locations for shader program
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
    m_shaders.at("ring").u_locs["angle"] = -1;

    // skybox shader
    m_shaders.emplace("skybox", shader_program{{{GL_VERTEX_SHADER, m_resource_path + "shaders/skybox.vert"},
                                                       {GL_FRAGMENT_SHADER, m_resource_path + "shaders/skybox.frag"}}});
    // request uniform locations for shader program
    m_shaders.at("skybox").u_locs["ProjectionMatrix"] = -1;
    m_shaders.at("skybox").u_locs["ViewMatrix"] = -1;
    m_shaders.at("skybox").u_locs["skybox_texture"] = -1;

    //store quad shader for working with extra framebuffer
    m_shaders.emplace("framebuffer", shader_program{{{GL_VERTEX_SHADER, m_resource_path + "shaders/framebuffer.vert"},
                                                     {GL_FRAGMENT_SHADER, m_resource_path + "shaders/framebuffer.frag"}}});

    m_shaders.at("framebuffer").u_locs["screenTexture"] = -1;
    m_shaders.at("framebuffer").u_locs["horizontal_mirroring"]= 0;
    m_shaders.at("framebuffer").u_locs["vertical_mirroring"]= 0;
    m_shaders.at("framebuffer").u_locs["luminance_preserving_greyscale"]= 0;
    m_shaders.at("framebuffer").u_locs["gaussian_blur"]= 0;
    m_shaders.at("framebuffer").u_locs["textureSize"] = -1;
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
    glUseProgram(m_shaders.at("planet").handle); // using the planet shader for setting boolean values
    if (key == GLFW_KEY_1) {
        // if 1 is pressed, we add the toon shading
        glUniform1b(m_shaders.at("planet").u_locs.at("toon_shading"),true);
    }else if (key == GLFW_KEY_2){
        // if 2 is pressed, we only use the normal shading
        glUniform1b(m_shaders.at("planet").u_locs.at("toon_shading"),false);
    }else if (key == GLFW_KEY_3  && action == GLFW_PRESS) {
        skybox_counter_ = (skybox_counter_ + 1) % skybox_texturers_.size();
        active_skybox_texture_object = (active_skybox_texture_object + 1) % skybox_texturers_.size();
        skybox_texture_index = skybox_texturers_[skybox_counter_];
        std::cout<< "counter " << skybox_counter_ <<"\n";
        std::cout<< "index " << skybox_texture_index <<"\n";
    }else if(key == GLFW_KEY_7 && action == GLFW_PRESS){
        glUseProgram(m_shaders.at("framebuffer").handle);
        post_processing_effects_[0] = !post_processing_effects_[0];
        glUniform1b(m_shaders.at("framebuffer").u_locs.at("luminance_preserving_greyscale"),post_processing_effects_[0]);
    }else if (key == GLFW_KEY_8 && action == GLFW_PRESS){
        glUseProgram(m_shaders.at("framebuffer").handle);
        post_processing_effects_[1] = !post_processing_effects_[1];
        glUniform1b(m_shaders.at("framebuffer").u_locs.at("horizontal_mirroring"),post_processing_effects_[1]);
    }else if (key == GLFW_KEY_9 && action == GLFW_PRESS){
        glUseProgram(m_shaders.at("framebuffer").handle);
        post_processing_effects_[2] = !post_processing_effects_[2];
        glUniform1b(m_shaders.at("framebuffer").u_locs.at("vertical_mirroring"),post_processing_effects_[2]);
    }else if (key == GLFW_KEY_0 && action == GLFW_PRESS){
        glUseProgram(m_shaders.at("framebuffer").handle);
        post_processing_effects_[3] = !post_processing_effects_[3];
        glUniform1b(m_shaders.at("framebuffer").u_locs.at("gaussian_blur"),post_processing_effects_[3]);
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
    generate_framebuffer(width, height);
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

    int resolution = 1000;
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

        float r = 1-(float) i / (float) resolution;
        float g = 1-(float) i / (float) resolution;
        float b = 1-(float) i / (float) resolution;

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

void ApplicationSolar::load_textures() {

    int count = 0;
    for (int i = 0; i < planets_.size(); i++) {
        if (planets_[i]->getName() != "Light") {
            std::cout<< "Loading planet: " << planets_[i]->getName() << "\n";
            pixel_data planet_texture = texture_loader::file(m_resource_path + "textures/" + planets_[i]->getName() + ".png");
            //pixel_data planet_texture = texture_loader::file(m_resource_path + "textures/Mars_normal.png");

            GLenum channel_type = planet_texture.channel_type;

            glActiveTexture(GL_TEXTURE1 + count);
            count++;
            texture_object texture;
            glGenTextures(1, &texture.handle);
            texture.target = GL_TEXTURE_2D;

            planets_[i]->setTexture(texture);

            glBindTexture(texture.target, texture.handle);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, planet_texture.channels, (GLsizei) planet_texture.width, (GLsizei) planet_texture.height,
                         0, planet_texture.channels, channel_type, planet_texture.ptr());
            std::cout<<"Loading normals\n";
/**/
            pixel_data normal_texture;

            std::ifstream inputFile(m_resource_path + "textures/" + planets_[i]->getName() + "_normal.png");

            if (inputFile.good()) {
                normal_texture = texture_loader::file(m_resource_path + "textures/" + planets_[i]->getName() + "_normal.png");
            } else {
                normal_texture = texture_loader::file(m_resource_path + "textures/Empty_normal.png");
            }


            glActiveTexture(GL_TEXTURE1 + count);
            count++;
            texture_object texture_normal;
            glGenTextures(1, &texture_normal.handle);
            texture_normal.target = GL_TEXTURE_2D;

            planets_[i]->setTextureNormal(texture_normal);

            glBindTexture(texture_normal.target, texture_normal.handle);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            glTexImage2D(GL_TEXTURE_2D, 0, normal_texture.channels, (GLsizei) normal_texture.width, (GLsizei) normal_texture.height,
                         0, normal_texture.channels, normal_texture.channel_type, normal_texture.ptr());



        }
    }

    skybox_texture_index = GL_TEXTURE1 + count;
}

/**
 * Loades the points of the triangles first and then calls the function to load the skybox texture
 */
void ApplicationSolar::generate_skybox() {


    std::vector<GLfloat> skybox_points = std::vector<GLfloat>(); // We will load all points from this file line by line
    skybox_points.reserve(108 * sizeof(GLfloat)); // we have 3 values for each point, 3 Points per triangle and 2 Triangles per side (6 Sides)
    std::ifstream skybox_file(m_resource_path + "models/skybox_points.txt"); // We store each point one by one

    // The order of the points is from: https://amin-ahmadi.com/2019/07/28/creating-a-skybox-using-c-qt-and-opengl/

    while(!skybox_file.eof()) {
        float x, y ,z;
        skybox_file>>x>>y>>z; // put the by space separated values into x y and z
        skybox_points.push_back((GLfloat)x); // push back to vector
        skybox_points.push_back((GLfloat)y); // push back to vector
        skybox_points.push_back((GLfloat)z); // push back to vector
    }


    model skybox_model{};
    skybox_model.data = skybox_points;                  // store points in vector
    skybox_model.vertex_num = skybox_points.size()/3;   // set number of vertices


    glGenVertexArrays(1, &skybox_object.vertex_AO);
    glBindVertexArray(skybox_object.vertex_AO);         // generate and bind vertex array

    glGenBuffers(1, &skybox_object.vertex_BO);
    glBindBuffer(GL_ARRAY_BUFFER, skybox_object.vertex_BO);

    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * skybox_points.size(), skybox_points.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, GLsizei(3 * sizeof(float)), 0);


    skybox_object.draw_mode = GL_TRIANGLES;
    skybox_object.num_elements = GLsizei(skybox_points.size());

    // loading textures

    loadSkyboxTextures(" (2)");
    loadSkyboxTextures("");
    loadSkyboxTextures("1");
    loadSkyboxTextures("2");
}

/**
 * Loads the specified texture
 * @param name of the texture (left.png -> name = " ",  left_123.png -> name = "_123")
 */
void ApplicationSolar::loadSkyboxTextures(std::string name) {

    pixel_data skybox_texture_side;
    glActiveTexture(skybox_texture_index);          // activating the texture (important)

    texture_object skybox_texture;
    glGenTextures(1, &skybox_texture.handle);
    skybox_texture.target = GL_TEXTURE_CUBE_MAP;    // we draw a cubemap

    glBindTexture(skybox_texture.target, skybox_texture.handle); // Binding the texture


    // generated by: https://tools.wwwtyro.net/space-3d/index.html
    std::vector<std::string> direction = {"right", "left", "bottom", "top", "front", "back"}; // right order for the textures

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // Parameters for the skybox
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    for (int i = 0; i < direction.size(); ++i) {
        skybox_texture_side = texture_loader::file(m_resource_path + "textures/" + direction[i] + name +".png");

        if(skybox_texture_side.ptr()) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, skybox_texture_side.channels, skybox_texture_side.width,
                         skybox_texture_side.height, 0, skybox_texture_side.channels, skybox_texture_side.channel_type,
                         skybox_texture_side.ptr());
        }else {
            std::cout<<"Error loading skymap for "<<i<<" face.\n";
        }
    }
    skybox_texturer_objects.push_back(skybox_texture);
    skybox_texturers_.push_back(skybox_texture_index);
    skybox_texture_index = skybox_texture_index + 1;
}

bool ApplicationSolar::generate_framebuffer(unsigned width, unsigned height) {

    std::cout<<"width: " << width << "\n";
    std::cout<<"height: " << height << "\n";

    screen_width = width;
    screen_height = height;

    //generate framebuffer
    glGenFramebuffers(1, &framebuffer_object_.handle);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_object_.handle);

    //create texture attachment as color_attachment
    texture_object texture;
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1,&texture.handle);
    glBindTexture(GL_TEXTURE_2D, texture.handle);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture.handle, 0);

    framebuffer_object_.texture_obj = texture; // setting the texture with the new width and height as the texture for our framebuffer

    //create renderbuffer attachment (depth attachment)
    unsigned int renderbuffer_object;
    glGenRenderbuffers(1, &renderbuffer_object);
    glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer_object);

    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, width, height);

    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, renderbuffer_object);

    framebuffer_object_.renderbuffer_handle = renderbuffer_object;
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    GLenum drawBuffers[1] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1,drawBuffers);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout<<"framebuffer could not get initialized\n";
        return false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    return true;
}

void ApplicationSolar::generate_screen(){
    // Generates the screen object with texturecoordinates and position


    //create texture coordinates
    // Source: https://riptutorial.com/opengl/example/23675/basics-of-framebuffers
    std::vector<GLfloat> quad = {
            // positions        texture coordinates
            -1.0f,  1.0f,       0.0f, 1.0f, // fist triangle
            -1.0f, -1.0f,       0.0f, 0.0f,
            1.0f, -1.0f,        1.0f, 0.0f,

            -1.0f,  1.0f,       0.0f, 1.0f, // second triangle
            1.0f, -1.0f,        1.0f, 0.0f,
            1.0f,  1.0f,        1.0f, 1.0f
    }; // (x + 1) / 2

    //create a new VertexArray
    glGenVertexArrays(1, &full_screenquad_.vertex_AO);
    glBindVertexArray(full_screenquad_.vertex_AO);

    //generate a new Buffer and bind it to the new VertexArray
    glGenBuffers(1, &full_screenquad_.vertex_BO);
    glBindBuffer(GL_ARRAY_BUFFER, full_screenquad_.vertex_BO);
    //specify the size of the data
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*quad.size(), quad.data(), GL_STATIC_DRAW);

    // first attribArray for positions
    glEnableVertexAttribArray(0); // positions of the triangle vertices
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, GLsizei(4 * sizeof(float)), 0);
    glEnableVertexAttribArray(1); // texture coordinates of the triangle vertices
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, GLsizei(4 * sizeof(float)), (void*)(2 * sizeof(float)));

    //specify the draw mode and the number of elements
    full_screenquad_.draw_mode = GL_TRIANGLE_STRIP;
    full_screenquad_.num_elements = GLsizei(quad.size()/4); // 6 points for 2 triangles

}

// exe entry point
int main(int argc, char *argv[]) {
    Application::run<ApplicationSolar>(argc, argv, 3, 2);
}
