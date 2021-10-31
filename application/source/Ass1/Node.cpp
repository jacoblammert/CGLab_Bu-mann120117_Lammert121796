//
// Created by Jacob on 23.10.2021.
//

#include "Node.h"
#include <string>
#include <utility>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

/**
 * Node constructor
 * creates a node
 */
Node::Node() {

}

/**
 *
 * @return returns the parent of a node, nullptr if a node has no partent
 */
std::shared_ptr<Node> Node::getParent() {
    return parent_;
}

/**
 * returns the Children (or a child of a child in the childrenlist) with a given name
 * @param name of the childe
 * @return shared node pointer
 */
std::shared_ptr<Node> Node::getChildren(std::string name) {
    for (int i = 0; i < children_.size(); ++i) {
        if (children_[i]->getName() == name) {
            return children_[i];
            // found the correct node with the given name
        }
    }
    // check each child, if it has a child with the name
    std::shared_ptr<Node> found;
    for (int i = 0; i < children_.size(); ++i) {
        found = children_[i]->getChildren(name);
        if (found != nullptr) {
            return found;
        }
    }
    // if we found no child with the name, we return a nullptr
    return nullptr;
}

/**
 * removes a child if the name is a given name
 * @param name of pointer we delete
 * @return removed child or nullptr
 */
std::shared_ptr<Node> Node::removeChildren(std::string name) {

    std::shared_ptr<Node> child = nullptr;

    for (int i = 0; i < children_.size(); ++i) {
        if (children_[i]->getName() == name) {
            child = children_[i];
            children_.erase(children_.begin() + i, children_.begin() + i + 1);
            return child;
            // remove parent of the removed child?
        }
    }

    return child;
}

/**
 *
 * @return all children of the node
 */

std::vector<std::shared_ptr<Node>> Node::getChildrenList() {
    return children_;
}
/**
 * sets current parent to a given nodeptr
 * @param parenta
 */
void Node::setParent(std::shared_ptr<Node> parenta) {
    this->parent_ = std::move(parenta);
}

/**
 * sets the local transform to a given 4x4 matrix
 * @param localtransform
 */
void Node::setLocalTransform(const glm::fmat4 &localtransform) {
    localTransform_ = localtransform;
}
/**
 * sets the world transform to a given 4x4 matrix
 * @param localtransform
 */
void Node::setWorldTransform(const glm::fmat4 &worldtransform) {
    worldTransform_ = worldtransform;
}

/**
 * adds a child, the childs parent will not be set!
 * @param child
 */
void Node::addChildren(std::shared_ptr<Node> child) {
    children_.push_back(child);
}

/**
 *
 * @return new transformation with rotated planet
 */
glm::fmat4 Node::getLocalTransform() {
    glm::fmat4 model_matrix = glm::fmat4{1.0};
    // rotation around the center (sun)
    model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime() * (name_.length() + 1)), glm::fvec3{0.0f, 1.0f, 0.0f});

    // translation
    model_matrix = glm::translate(model_matrix,
                                  glm::vec3(localTransform_[0][3], localTransform_[1][3], localTransform_[2][3]));

    // rotation around its axis
    model_matrix = glm::rotate(model_matrix, float(glfwGetTime() * (name_.length() + 1)), glm::fvec3{0.0f, 1.0f, 0.0f});

    // Scaling
    model_matrix = model_matrix * glm::fmat4(localTransform_[0][0], localTransform_[0][1], localTransform_[0][2], 0,
                                             localTransform_[1][0], localTransform_[1][1], localTransform_[1][2], 0,
                                             localTransform_[2][0], localTransform_[2][1], localTransform_[2][2], 0,
                                             0, 0, 0, 1);
    // 1. rotate planet (around sun)
    // 2. translate planet (local transform)
    // 3. world transform
    // 4. rotate planet (own axis)
    return model_matrix;
}

/**
 * local transformation in regards to the worldtransformation of the parents
 * @return
 */
glm::fmat4 Node::getWorldTransform() {
    //return worldTransform;
    if (parent_ != nullptr) {
        return parent_->getWorldTransform() * getLocalTransform();
    }
    return localTransform_;
}

/**
 *
 * @return Name of the node, only set for planets
 */
std::string Node::getName() {
    return name_;
}

/**
 *
 * @return path of all planets as string
 */
std::string Node::getPath() {
    std::string path_ = "";
    if (getDepth() == 1) {
        // root, because 1 is the lowest depth
        path_ = "root ";
    } else if (name_.empty()) {
        // name is empty, we have no planet, but a node -> planet
        path_ += "[ " + std::to_string(getDepth()) + " ]";
    } else {
        // we have a planetname in the path_ variable
        path_ = name_;
    }

    if (children_.empty()) {
        // deepest depth, we return "root", [depth] or PlanetX
        return path_;
    }

    path_ = path_ + " -> (";
    // we add -> ( because we but children here )

    for (int i = 0; i < children_.size(); ++i) {

        // We put all children (name or depth) in the brackets
        path_ += children_[i]->getPath();
        if (i < children_.size() - 1) {
            // the last child doesent need a "," at the end
            path_ += ",";
        }
    }
    path_ = path_ + ")"; // closing brackets

    return path_;
}

/**
 * we get the depth through recursion
 * @return
 */
int Node::getDepth() {
    // the  depth is initialized with 1
    if (parent_ != nullptr) {
        return parent_->getDepth() + depth_;
    }
    return depth_;
}

/**
 * We set the name of the node to name
 * this makes it a "planet" and it will be drawn on screen
 * @param name
 */
void Node::setName(std::string name) {
    this->name_ = name;
}
