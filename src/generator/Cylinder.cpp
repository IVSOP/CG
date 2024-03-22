#include "Cylinder.h"

#include <iostream>
#include "Consts.h"

#include <math.h>

std::vector<Vertex> Cylinder::createCylinderPoints(float radius, float height, int slices, int stacks) {
    std::vector<Vertex> basePoints = std::vector<Vertex>();
    std::vector<Vertex> prevBasePoints = std::vector<Vertex>();
    std::vector<Vertex> ans = std::vector<Vertex>();

    float angle = 360.0f / static_cast<float>(slices);
    float stackStep = height / static_cast<float>(stacks);
    // float radiusStep = radius / static_cast<float>(stacks);

    glm::vec4 currCoords;
    glm::vec4 prevCoords;
    glm::vec4 baseCoords;

    glm::mat4 rotYMatrix = Consts::rotYMatrix(-angle);

    for(int i = 0; i <= stacks; i++) {
        basePoints.emplace_back(0.0f , static_cast<float>(i) * stackStep - height / 2.0f, 0.0f);
        basePoints.emplace_back(radius, static_cast<float>(i) * stackStep - height / 2.0f, 0.0f);

        for (int j = 1; j <= slices; j++) {
            baseCoords = basePoints[0].getCoords();
            prevCoords = basePoints[j].getCoords();

            if(j != slices) currCoords = rotYMatrix * prevCoords;
            else currCoords = basePoints[1].getCoords();

            if(i == 0){
                ans.emplace_back(baseCoords.x, baseCoords.y, baseCoords.z,0.0f,-1.0f,0.0f);
                ans.emplace_back(prevCoords.x, prevCoords.y, prevCoords.z,0.0f,-1.0f,0.0f);
                ans.emplace_back(currCoords.x, currCoords.y, currCoords.z,0.0f,-1.0f,0.0f);
            }

            if(i == stacks){
                ans.emplace_back(currCoords.x, currCoords.y, currCoords.z,0.0f,1.0f,0.0f);
                ans.emplace_back(prevCoords.x, prevCoords.y, prevCoords.z,0.0f,1.0f,0.0f);
                ans.emplace_back(baseCoords.x, baseCoords.y, baseCoords.z,0.0f,1.0f,0.0f);
            }

            if(i > 0) {
                glm::vec4 rightCoords = basePoints[j].getCoords();
                glm::vec4 rightDownCoords = prevBasePoints[j].getCoords();
                glm::vec4 downCoords = prevBasePoints[j+1].getCoords();

                //Normais dos pontos
                glm::vec3 rightDownCoordsNormal = glm::normalize(glm::vec3(rightDownCoords.x,0.0f,rightDownCoords.z));
                glm::vec3 rightCoordsNormal = glm::normalize(glm::vec3(rightCoords.x,0.0f,rightCoords.z));
                glm::vec3 currCoordsNormal = glm::normalize(glm::vec3(currCoords.x,0.0f,currCoords.z));
                glm::vec3 downCoordsNormal = glm::normalize(glm::vec3(downCoords.x,0.0f,downCoords.z));
                // Triangle  |‾/
                //           |/

                ans.emplace_back(rightDownCoords.x, rightDownCoords.y, rightDownCoords.z,rightDownCoordsNormal.x,0.0f,rightDownCoordsNormal.z);
                ans.emplace_back(rightCoords.x, rightCoords.y, rightCoords.z,rightCoordsNormal.x,0.0f,rightCoordsNormal.z);
                ans.emplace_back(currCoords.x, currCoords.y, currCoords.z,currCoordsNormal.x,0.0f,currCoordsNormal.z);

                // Triangle  /|
                //          /_|

                ans.emplace_back(currCoords.x, currCoords.y, currCoords.z,currCoordsNormal.x,0.0f,currCoordsNormal.z);
                ans.emplace_back(downCoords.x, downCoords.y, downCoords.z,downCoordsNormal.x,0.0f,downCoordsNormal.z);
                ans.emplace_back(rightDownCoords.x, rightDownCoords.y, rightDownCoords.z, rightDownCoordsNormal.x,0.0f,rightDownCoordsNormal.z);
            }

            basePoints.emplace_back(currCoords.x, currCoords.y, currCoords.z);
        }

        prevBasePoints.clear();
        prevBasePoints.insert(prevBasePoints.end(), basePoints.begin(), basePoints.end());

        basePoints.clear();
    }

    return ans;
}
