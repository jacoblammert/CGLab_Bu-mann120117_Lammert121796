//
// Created by Jacob on 23.10.2021.
//

#ifndef OPENGLFRAMEWORK_GEOMETRYNODE_H
#define OPENGLFRAMEWORK_GEOMETRYNODE_H


#include <structs.hpp>
#include "Node.h"

class GeometryNode : public Node{

public:
    model_object getGeometry();
    void setGeometry(model_object model);

    model_object getTrail();
    void setTrail(model_object model);

    void setTexture(texture_object texture);
    texture_object getTexture();

    void setTextureNormal(texture_object texture);
    texture_object getTextureNormal();

private:
    model_object geometry_;
    model_object trail_;
    texture_object texture_;
    texture_object texture_normal_;

};


#endif //OPENGLFRAMEWORK_GEOMETRYNODE_H
