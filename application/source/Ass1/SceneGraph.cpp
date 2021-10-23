//
// Created by Jacob on 23.10.2021.
//

#include "SceneGraph.h"

SceneGraph::SceneGraph(){}

std::string SceneGraph::getName(){
    return name;
}

void SceneGraph::setName(std::string name){
    this->name = name;
}