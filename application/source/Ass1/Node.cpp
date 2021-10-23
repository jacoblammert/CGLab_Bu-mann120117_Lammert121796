//
// Created by Jacob on 23.10.2021.
//

#include "Node.h"
#include <string>

Node::Node() {

}

Node *Node::getParent() {
    return parent;
}

Node *Node::getChildren(std::string name) {

    for (int i = 0; i < children.size(); ++i){
        if (children[i]->getName() == name){
            return children[i];
        }
    }

    return nullptr;
}

Node *Node::removeChildren(std::string name) {

    Node* child = nullptr;

    for (int i = 0; i < children.size(); ++i){
        if (children[i]->getName() == name){
            child = children[i];
            children.erase(children.begin() + i,children.begin() + i + 1);
            return child;
        }
    }

    return child;
}

std::vector<Node *> Node::getChildrenList() {
    return children;
}

void Node::setParent(Node *parenta) {
    this->parent = parenta;
}

void Node::setLocalTransform(glm::fmat4 localtransform) {
    this->localTransform = localtransform;
}

void Node::setWorldTransform(glm::fmat4 worldtransform) {
    worldTransform = worldtransform;
}

void Node::addChildren(Node *child) {
    children.push_back(child);
}

glm::fmat4 Node::getLocalTransform() {
    return localTransform;
}

glm::fmat4 Node::getWorldTransform() {
    return worldTransform;
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
