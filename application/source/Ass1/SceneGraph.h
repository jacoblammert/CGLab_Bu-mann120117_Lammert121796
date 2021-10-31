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
    ~SceneGraph();

    std::string getName();
    std::shared_ptr<Node> getRoot() const;

    std::string printGraph();

private:

    //static SceneGraph* sceneGraph;

    std::string name = "";
    std::shared_ptr<Node> root = nullptr;
    void setName(std::string name);
    void setRoot(std::shared_ptr<Node> root);

};


#endif //OPENGLFRAMEWORK_SCENEGRAPH_H
