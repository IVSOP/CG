#include <iostream>
#include <vector>
#include <algorithm>

#include "Box.h"
#include "Plane.h"
#include "Consts.h"

#define _USE_MATH_DEFINES
#include <math.h>

std::vector<Vertex> Box::createBoxPoints(float length, int divisions) {
    std::vector<Vertex> planeFacingUp = Plane::createPlanePointsNoTranslate(length, divisions);

    std::vector<Vertex> planeFacingDown = std::vector<Vertex>(); // vetor invertido produz plano ao contrario

    for(Vertex v : planeFacingUp){
        v.tex_coord = glm::vec2(1.0f - v.tex_coord.x, 1.0f - v.tex_coord.y);

        planeFacingDown.emplace(planeFacingDown.begin(), v);
    }

    std::vector<Vertex> res = std::vector<Vertex>();
    
    glm::vec4 currPoint;
    const float shiftValue = static_cast<float>(length)/2.0f;
    
    glm::vec4 shift(shiftValue,shiftValue,shiftValue,0.0f);

    //Down face
    for (auto point: planeFacingDown) {
        currPoint = point.getCoords();
        currPoint -= shift;

        res.emplace_back(currPoint.x, currPoint.y, currPoint.z, 0.0f,-1.0f,0.0f, point.tex_coord.x, point.tex_coord.y);
    }

    //Left face
    glm::mat4 rotZMatrix = Consts::rotZMatrix(90.0f);

    for (auto point: planeFacingUp) {
        currPoint = rotZMatrix * point.getCoords();
        currPoint -= shift;
        point.tex_coord = glm::vec2(1 - point.tex_coord.y, 1 - point.tex_coord.x);

        res.emplace_back(currPoint.x, currPoint.y, currPoint.z,-1.0f,0.0f,0.0f, point.tex_coord.x, point.tex_coord.y);
    }

    //Right face
    for (auto point: planeFacingDown) {
        currPoint = rotZMatrix * point.getCoords();
        currPoint -= shift;
        point.tex_coord = glm::vec2(point.tex_coord.y, point.tex_coord.x);

        res.emplace_back(currPoint.x + float(length), currPoint.y, currPoint.z,1.0f,0.0f,0.0f, point.tex_coord.x, point.tex_coord.y);
    }

    //Back face
    glm::mat4 rotXMatrix = Consts::rotXMatrix(-90.0f);

    for (auto point: planeFacingUp) {
        currPoint = rotXMatrix * point.getCoords();
        currPoint -= shift;
        point.tex_coord = glm::vec2(1 - point.tex_coord.x, 1 - point.tex_coord.y);

        res.emplace_back(currPoint.x, currPoint.y, currPoint.z,0.0f,0.0f,-1.0f, point.tex_coord.x, point.tex_coord.y);
    }

    //Front face
    for (auto point: planeFacingDown) {
        currPoint = rotXMatrix * point.getCoords();
        currPoint -= shift;
        point.tex_coord = glm::vec2(point.tex_coord.x, point.tex_coord.y);

        res.emplace_back(currPoint.x, currPoint.y, currPoint.z + float(length), 0.0f,0.0f,1.0f, point.tex_coord.x, point.tex_coord.y);
    }

    //Up face
    for (auto point: planeFacingUp) {
        currPoint = point.getCoords();
        currPoint -= shift;

        res.emplace_back(currPoint.x, currPoint.y + float(length), currPoint.z,0.0f,1.0f,0.0f, point.tex_coord.x, point.tex_coord.y);
    }

    return res;
}
