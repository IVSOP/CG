#include <iostream>
#include <vector>
#include <algorithm>

#include "Box.h"
#include "Plane.h"
#include "Consts.h"

std::vector<Vertex> Box::createBoxPoints(float length, int divisions) {
    std::vector<Vertex> planeFacingUp = Plane::createPlanePointsNoTranslate(length, divisions);
    std::vector<Vertex> planeFacingDown(planeFacingUp.rbegin(), planeFacingUp.rend()); // vetor invertido produz plano ao contrario

    std::vector<Vertex> res = std::vector<Vertex>();
    
    glm::vec4 currPoint;
    const float shiftValue = static_cast<float>(length)/2.0f;
    
    glm::vec4 shift(shiftValue,shiftValue,shiftValue,0.0f);

    //Down face
    for (auto point: planeFacingDown) {
        currPoint = point.getCoords();
        currPoint -= shift;
        res.emplace_back(currPoint.x, currPoint.y, currPoint.z,0.0f,-1.0f,0.0f);
    }

    //Left face
    glm::mat4 rotZMatrix = Consts::rotZMatrix(90.0f);

    for (auto point: planeFacingUp) {
        currPoint = rotZMatrix * point.getCoords();
        currPoint -= shift;

        res.emplace_back(currPoint.x, currPoint.y, currPoint.z,-1.0f,0.0f,0.0f);
    }

    //Right face
    for (auto point: planeFacingDown) {
        currPoint = rotZMatrix * point.getCoords();
        currPoint -= shift;
        res.emplace_back(currPoint.x + float(length), currPoint.y, currPoint.z,1.0f,0.0f,0.0f);
    }

    //Back face
    glm::mat4 rotXMatrix = Consts::rotXMatrix(-90.0f);

    for (auto point: planeFacingUp) {
        currPoint = rotXMatrix * point.getCoords();
        currPoint -= shift;
        res.emplace_back(currPoint.x, currPoint.y, currPoint.z,0.0f,0.0f,-1.0f);
    }

    //Front face
    for (auto point: planeFacingDown) {
        currPoint = rotXMatrix * point.getCoords();
        currPoint -= shift;
        res.emplace_back(currPoint.x, currPoint.y, currPoint.z + float(length), 0.0f,0.0f,1.0f);
    }

    //Up face
    for (auto point: planeFacingUp) {
        currPoint = point.getCoords();
        currPoint -= shift;
        res.emplace_back(currPoint.x, currPoint.y + float(length), currPoint.z,0.0f,1.0f,0.0f);
    }

    // TEXT COORDINATES - em fase de teste
    for(auto vertex: res) {
        vertex.tex_coord[0] = asin(vertex.getX()) / M_PI + 0.5f;
        vertex.tex_coord[1] = asin(vertex.getY()) / M_PI + 0.5f;
    }
    return res;
}
