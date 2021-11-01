//
// Created by Jacob on 23.10.2021.
//

#ifndef OPENGLFRAMEWORK_SCENEGRAPH_H
#define OPENGLFRAMEWORK_SCENEGRAPH_H

#include <string>
#include <vector>
#include "Node.h"
#include "GeometryNode.h"
#include "model.hpp"
#include "CameraNode.h"


class SceneGraph {
public:
    std::string getName();

    std::shared_ptr<Node> getRoot() const;

    std::string printGraph();

    static SceneGraph *getInstance() {
        // Singleton pattern as discribed in: https://www.youtube.com/watch?v=IJKu2pebSwc&t=91s
        if (sceneGraph_ == NULL) {
            // If the static object is not initialized, it will be, otherwise it will be returned
            sceneGraph_ = new SceneGraph();
        }
        return sceneGraph_;
    }

    SceneGraph();

private:

    static SceneGraph *sceneGraph_; // since it is static, we only have one instance

    ~SceneGraph();

    std::string name_ = "";
    std::shared_ptr<Node> root_ = nullptr;

    void setName(std::string name);

    void setRoot(std::shared_ptr<Node> root);

};


#endif //OPENGLFRAMEWORK_SCENEGRAPH_H
