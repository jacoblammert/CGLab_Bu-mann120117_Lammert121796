//
// Created by Jacob on 23.10.2021.
//

#ifndef OPENGLFRAMEWORK_SCENEGRAPH_H
#define OPENGLFRAMEWORK_SCENEGRAPH_H

#include <string>
#include <vector>
#include "Node.h"


class SceneGraph {
public:
    SceneGraph();

    std::string getName();
    std::shared_ptr<Node> getRoot();

private:
    std::string name = "";
    std::shared_ptr<Node> root;
    void setName(std::string name);
    void setRoot(std::shared_ptr<Node> root);

};


#endif //OPENGLFRAMEWORK_SCENEGRAPH_H
