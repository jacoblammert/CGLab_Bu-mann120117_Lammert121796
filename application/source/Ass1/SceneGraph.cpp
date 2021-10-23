//
// Created by Jacob on 23.10.2021.
//

#include "SceneGraph.h"
#include <Node.h>

SceneGraph::SceneGraph(){}

std::string SceneGraph::getName(){
    return name;
}

void SceneGraph::setName(std::string name){
    this->name = name;
}

void SceneGraph::setRoot(Node* root){
    this->root = root;
}

Node* SceneGraph::getRoot(){
    return root;
}