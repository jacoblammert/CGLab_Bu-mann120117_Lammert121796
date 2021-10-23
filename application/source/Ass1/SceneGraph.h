//
// Created by Jacob on 23.10.2021.
//

#ifndef OPENGLFRAMEWORK_SCENEGRAPH_H
#define OPENGLFRAMEWORK_SCENEGRAPH_H

#include <string>
#include <vector>
#include string


class SceneGraph {
public:
    SceneGraph();

    std::string getName();


private:
    std::String name = "";
    void setName(std::string cost& name);

};


#endif //OPENGLFRAMEWORK_SCENEGRAPH_H
