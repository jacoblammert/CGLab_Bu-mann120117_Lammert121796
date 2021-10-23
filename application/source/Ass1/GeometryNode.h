//
// Created by Jacob on 23.10.2021.
//

#ifndef OPENGLFRAMEWORK_GEOMETRYNODE_H
#define OPENGLFRAMEWORK_GEOMETRYNODE_H


#include <structs.hpp>
#include "Node.h"

class GeometryNode : Node{

public:
    model_object getGeometry();
    void getGeometry(model_object model);

private:
    model_object geometry;
    //TODO check if model_object class is the right one

};


#endif //OPENGLFRAMEWORK_GEOMETRYNODE_H
