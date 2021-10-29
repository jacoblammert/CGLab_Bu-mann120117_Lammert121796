//
// Created by Jacob on 23.10.2021.
//

#ifndef OPENGLFRAMEWORK_NODE_H
#define OPENGLFRAMEWORK_NODE_H

#include <string>
#include <vector>
#include <glm/detail/type_mat4x4.hpp>
#include <memory>



class Node {

public:
    Node();

    std::shared_ptr<Node> getParent();
    std::shared_ptr<Node> getChildren(std::string name);
    std::shared_ptr<Node> removeChildren(std::string name);

    std::vector<std::shared_ptr<Node>> getChildrenList();

    void setParent(std::shared_ptr<Node> parent);
    void setLocalTransform(const glm::fmat4& localtransform);
    void setWorldTransform(const glm::fmat4& worldtransform);
    void addChildren(std::shared_ptr<Node> child);

    glm::fmat4 getLocalTransform();
    glm::fmat4 getWorldTransform();

    std::string getName();
    std::string getPath();

    int getDepth();



private:
    std::shared_ptr<Node> parent;
    std::vector<std::shared_ptr<Node>> children;
    std::string name;
    std::string path;
    int depth;
    glm::fmat4 localTransform = glm::fmat4(1.0);
    glm::fmat4 worldTransform = glm::fmat4(1.0);


};


#endif //OPENGLFRAMEWORK_NODE_H
