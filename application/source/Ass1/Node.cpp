//
// Created by Jacob on 23.10.2021.
//

#include "Node.h"
#include <string>
#include <utility>

Node::Node() {

}

std::shared_ptr<Node> Node::getParent() {
    return parent;
}

std::shared_ptr<Node> Node::getChildren(std::string name) {

    for (int i = 0; i < children.size(); ++i){
        if (children[i]->getName() == name){
            return children[i];
        }
    }

    return nullptr;
}

std::shared_ptr<Node> Node::removeChildren(std::string name) {

    std::shared_ptr<Node> child = nullptr;

    for (int i = 0; i < children.size(); ++i){
        if (children[i]->getName() == name){
            child = children[i];
            children.erase(children.begin() + i,children.begin() + i + 1);
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

void Node::setLocalTransform(const glm::fmat4& localtransform) {
    localTransform = localtransform;
}

void Node::setWorldTransform(const glm::fmat4& worldtransform) {
    worldTransform = worldtransform;
}

void Node::addChildren(std::shared_ptr<Node> child) {
    children.push_back(child);
}

glm::fmat4 Node::getLocalTransform() {
    return localTransform;
}

glm::fmat4 Node::getWorldTransform() {
    if (parent != nullptr){
        return parent->getWorldTransform();
    }
    return localTransform;
}

std::string Node::getName() {
    return name;
}

std::string Node::getPath() {
    return path;
}

int Node::getDepth() {
    return depth;
}
