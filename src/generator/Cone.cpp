#include <iostream>
#include "Cone.h"
#include "Consts.h"

std::vector<Vertex> Cone::createConePoints(float radius, float height, int slices, int stacks) {
    std::vector<Vertex> basePoints = std::vector<Vertex>();
    std::vector<Vertex> prevBasePoints = std::vector<Vertex>();
    std::vector<Vertex> ans = std::vector<Vertex>();

    float angle = 360.0f / static_cast<float>(slices);
    float stackStep = height / static_cast<float>(stacks);
    float radiusStep = radius / static_cast<float>(stacks);

    glm::vec4 currPoint;
    glm::vec4 prevPoint;
    glm::vec4 basePoint;
    glm::mat4 rotYMatrix = Consts::rotYMatrix(angle);


    for(int i = 0; i < stacks; i++) {
        basePoints.emplace_back(0.0f, static_cast<float>(i) * stackStep, 0.0f);
        basePoints.emplace_back(radius - (static_cast<float>(i) * radiusStep), static_cast<float>(i) * stackStep, 0.0f);

        for (int j = 1; j <= slices; j++) {
            glm::vec4 baseCoords = basePoints[0].getCoords();
            basePoint = glm::vec4(baseCoords.x, baseCoords.y, baseCoords.z, 1.0f);

            glm::vec4 prevCoords = basePoints[j].getCoords();
            prevPoint = glm::vec4(prevCoords.x, prevCoords.y, prevCoords.z, 1.0f);

            glm::vec4 currCoords = rotYMatrix * prevCoords;
            currPoint = glm::vec4(currCoords.x, currCoords.y, currCoords.z, 1.0f);

            basePoints.emplace_back(currPoint.x, currPoint.y, currPoint.z);

            ans.emplace_back(basePoint.x, basePoint.y, basePoint.z);
            ans.emplace_back(currPoint.x, currPoint.y, currPoint.z);
            ans.emplace_back(prevPoint.x, prevPoint.y, prevPoint.z);

            if(i > 0){
                glm::vec4 rightCoords = basePoints[j].getCoords();
                glm::vec4 rightDownCoords = prevBasePoints[j].getCoords();
                glm::vec4 downCoords = prevBasePoints[j+1].getCoords();

                // Triangle |\
                //          |_\

                ans.emplace_back(rightDownCoords.x, rightDownCoords.y, rightDownCoords.z);
                ans.emplace_back(rightCoords.x, rightCoords.y, rightCoords.z);
                ans.emplace_back(currPoint.x, currPoint.y, currPoint.z);

                // Triangle \‾|
                //           \|

                ans.emplace_back(rightDownCoords.x, rightDownCoords.y, rightDownCoords.z);
                ans.emplace_back(currPoint.x, currPoint.y, currPoint.z);
                ans.emplace_back(downCoords.x, downCoords.y, downCoords.z);
            }
        }

        prevBasePoints.clear();
        prevBasePoints.insert(prevBasePoints.end(), basePoints.begin(), basePoints.end());

        basePoints.clear();
    }


    for(int i = 2; i < prevBasePoints.size(); i++){
        glm::vec4 leftCoords = prevBasePoints[i].getCoords();
        glm::vec4 rightCoords = prevBasePoints[i-1].getCoords();

        ans.emplace_back(leftCoords.x, leftCoords.y, leftCoords.z);
        ans.emplace_back(rightCoords.x, rightCoords.y, rightCoords.z);
        ans.emplace_back(0.0f, height, 0.0f);
    }

    return ans;
}
