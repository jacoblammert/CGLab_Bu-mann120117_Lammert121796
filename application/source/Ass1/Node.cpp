//
// Created by Jacob on 23.10.2021.
//

#include "Node.h"
#include <string>
#include <utility>
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

Node::Node() {

}

std::shared_ptr<Node> Node::getParent() {
    return parent;
}

std::shared_ptr<Node> Node::getChildren(std::string name) {
    for (int i = 0; i < children.size(); ++i) {
        if (children[i]->getName() == name) {
            return children[i];
        }
    }
    std::shared_ptr<Node> found;
    for (int i = 0; i < children.size(); ++i) {
        found = children[i]->getChildren(name);
        if (found != nullptr) {
            return found;
        }
    }
    return nullptr;
}

std::shared_ptr<Node> Node::removeChildren(std::string name) {

    std::shared_ptr<Node> child = nullptr;

    for (int i = 0; i < children.size(); ++i) {
        if (children[i]->getName() == name) {
            child = children[i];
            children.erase(children.begin() + i, children.begin() + i + 1);
            return child;
        }
    }

    return child;
}

std::vector<std::shared_ptr<Node>> Node::getChildrenList() {
    return children;
}

void Node::setParent(std::shared_ptr<Node> parenta) {
    this->parent = std::move(parenta);
}

void Node::setLocalTransform(const glm::fmat4 &localtransform) {
    localTransform = localtransform;
}

void Node::setWorldTransform(const glm::fmat4 &worldtransform) {
    worldTransform = worldtransform;
}

void Node::addChildren(std::shared_ptr<Node> child) {
    children.push_back(child);
}

glm::fmat4 Node::getLocalTransform() {
    glm::fmat4 model_matrix;
    // rotation
    model_matrix = glm::rotate(glm::fmat4{}, float(glfwGetTime() * (name.length() + 1)), glm::fvec3{0.0f, 1.0f, 0.0f});

    // translation
    model_matrix = glm::translate(model_matrix,
                                  glm::vec3(localTransform[0][3], localTransform[1][3], localTransform[2][3]));

    // rotation
    model_matrix = glm::rotate(model_matrix, float(glfwGetTime() * (name.length() + 1)), glm::fvec3{0.0f, 1.0f, 0.0f});

    // Scaling
    model_matrix = model_matrix * glm::fmat4(localTransform[0][0], localTransform[0][1], localTransform[0][2], 0,
                                             localTransform[1][0], localTransform[1][1], localTransform[1][2], 0,
                                             localTransform[2][0], localTransform[2][1], localTransform[2][2], 0,
                                             0, 0, 0, 1);

    // 1. rotate planet (own axis)
    // 2. translate planet (local transform)
    // 3. world transform
    return model_matrix;
}

glm::fmat4 Node::getWorldTransform() {
    //return worldTransform;
    if (parent != nullptr) {
        return parent->getWorldTransform() * getLocalTransform();
    }
    return localTransform;
}

std::string Node::getName() {
    return name;
}

std::string Node::getPath() {
    std::string path_ = "";
    if (getDepth() == 1) {
        path_ = "root ";
    } else if (name.empty()) {
        path_ += "[ " + std::to_string(getDepth()) + " ]";
    } else {
        path_ = name;
    }

    if (children.empty()) {
        return path_;
    }

    path_ = path_ + " -> (";

    for (int i = 0; i < children.size(); ++i) {

        path_ += children[i]->getPath();
        if (i < children.size() - 1) {
            path_ += ",";
        }
    }
    path_ = path_ + ")";

    return path_;

    /*/
    std::string path_ = "";
    if (parent != nullptr) {
        if (!name.empty()) {
            path = parent->getPath() + " -> " + name;
        }else{
            path = parent->getPath() + " -> [ ]";
        }
    }

    path_ = "Path: ";
    if (!name.empty()){
        return path_ + " " + name;
    } else{
        return path_ + " [ ]";
    }/**/
}

int Node::getDepth() {
    if (parent != nullptr) {
        return parent->getDepth() + depth;
    }
    return depth;
}

void Node::setName(std::string name) {
    this->name = name;
}
