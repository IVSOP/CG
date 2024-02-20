#include <iostream>

#include "Box.h"
#include "Plane.h"
#include "Consts.h"

std::vector<Vertex> Box::createBoxPoints(int length, int divisions) {
    std::vector<Vertex> ans = Plane::createPlanePoints(length,divisions);
    std::vector<Vertex> ans2 = std::vector<Vertex>();
    
    glm::vec4 currPoint;
    float shiftValue = float(length)/2;
    glm::vec4 shift(shiftValue,shiftValue,shiftValue,0.0f);
    //plano base
    ans2.insert(ans2.end(), ans.begin(), ans.end());
    
    //plano esq
    glm::mat4 rotZMatrix = Consts::rotZMatrix(90.0f);

    for (auto point: ans) {
        currPoint = rotZMatrix * point.getCoords();
        currPoint -= shift;
        ans2.emplace_back(currPoint.x, currPoint.y, currPoint.z);
    }

        // glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    //plano dir 
    for (auto point: ans) {
        currPoint = rotZMatrix * point.getCoords();
        currPoint -= shift;
        ans2.emplace_back(currPoint.x + float(length), currPoint.y, currPoint.z);
    }

    //plano trás
    glm::mat4 rotXMatrix = Consts::rotZMatrix(270.0f);

    for (auto point: ans) {
        currPoint = rotXMatrix * point.getCoords();
        currPoint -= shift;
        ans2.emplace_back(currPoint.x, currPoint.y, currPoint.z);
    }

    //plano frente
    glm::mat4 rotXMatrix = Consts::rotZMatrix(270.0f);

    for (auto point: ans) {
        currPoint = rotXMatrix * point.getCoords();
        currPoint -= shift;
        ans2.emplace_back(currPoint.x, currPoint.y, currPoint.z + float(length));
    }

    //plano cima
    for (auto point: ans) {
        currPoint = rotXMatrix * point.getCoords();
        currPoint -= shift;
        ans2.emplace_back(currPoint.x, currPoint.y + float(length), currPoint.z);
    }

    return ans2;
}
