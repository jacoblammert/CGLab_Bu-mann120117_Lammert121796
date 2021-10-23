//
// Created by Jacob on 23.10.2021.
//

#ifndef OPENGLFRAMEWORK_NODE_H
#define OPENGLFRAMEWORK_NODE_H

#include <string>
#include <vector>
#include <glm/detail/type_mat4x4.hpp>


class Node {

public:
    Node();

    Node* getParent();
    Node* getChildren(std::string name);
    Node* removeChildren(std::string name);

    std::vector<Node*> getChildrenList();

    void setParent(Node* parent);
    void setLocalTransform(glm::fmat4 localtransform);
    void setWorldTransform(glm::fmat4 worldtransform);
    void addChildren(Node* child);

    glm::fmat4 getLocalTransform();
    glm::fmat4 getWorldTransform();

    std::string getName();
    std::string getPath();

    int getDepth();



private:
    Node *parent;
    std::vector<Node*> children;
    std::string name;
    std::string path;
    int depth;
    glm::fmat4 localTransform;
    glm::fmat4 worldTransform;


};


#endif //OPENGLFRAMEWORK_NODE_H
