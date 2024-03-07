#include <iostream>
#include "Cone.h"
#include "Consts.h"

#include <math.h>

std::vector<Vertex> Cone::createConePoints(float radius, float height, int slices, int stacks) {
    std::vector<Vertex> basePoints = std::vector<Vertex>();
    std::vector<Vertex> prevBasePoints = std::vector<Vertex>();
    std::vector<Vertex> ans = std::vector<Vertex>();

    float angle = 360.0f / static_cast<float>(slices);
    float stackStep = height / static_cast<float>(stacks);
    float radiusStep = radius / static_cast<float>(stacks);

    glm::vec4 currCoords;
    glm::vec4 prevCoords;
    glm::vec4 baseCoords;

    glm::mat4 rotYMatrix = Consts::rotYMatrix(-angle);

    for(int i = 0; i < stacks; i++) {
        basePoints.emplace_back(0.0f , static_cast<float>(i) * stackStep, 0.0f);
        basePoints.emplace_back(radius - (static_cast<float>(i) * radiusStep) , static_cast<float>(i) * stackStep, 0.0f);

        for (int j = 1; j <= slices; j++) {
            baseCoords = basePoints[0].getCoords();
            prevCoords = basePoints[j].getCoords();

            if(j != slices) currCoords = rotYMatrix * prevCoords;
            else currCoords = basePoints[1].getCoords();

            if(i == 0){
                ans.emplace_back(baseCoords.x, baseCoords.y, baseCoords.z);
                ans.emplace_back(prevCoords.x, prevCoords.y, prevCoords.z);
                ans.emplace_back(currCoords.x, currCoords.y, currCoords.z);
            }

            if(i > 0) {
                glm::vec4 rightCoords = basePoints[j].getCoords();
                glm::vec4 rightDownCoords = prevBasePoints[j].getCoords();
                glm::vec4 downCoords = prevBasePoints[j+1].getCoords();

                // Triangle  |‾/
                //           |/

                ans.emplace_back(rightDownCoords.x, rightDownCoords.y, rightDownCoords.z);
                ans.emplace_back(rightCoords.x, rightCoords.y, rightCoords.z);
                ans.emplace_back(currCoords.x, currCoords.y, currCoords.z);

                // Triangle  /|
                //          /_|

                ans.emplace_back(currCoords.x, currCoords.y, currCoords.z);
                ans.emplace_back(downCoords.x, downCoords.y, downCoords.z);
                ans.emplace_back(rightDownCoords.x, rightDownCoords.y, rightDownCoords.z);
            }

            basePoints.emplace_back(currCoords.x, currCoords.y, currCoords.z);
        }

        prevBasePoints.clear();
        prevBasePoints.insert(prevBasePoints.end(), basePoints.begin(), basePoints.end());

        basePoints.clear();
    }


    for(unsigned int i = 1; i < prevBasePoints.size(); i++){
        glm::vec4 leftCoords = prevBasePoints[i].getCoords();
        glm::vec4 rightCoords = prevBasePoints[i-1].getCoords();

        ans.emplace_back(rightCoords.x, rightCoords.y, rightCoords.z);
        ans.emplace_back(0.0f, height, 0.0f);
        ans.emplace_back(leftCoords.x, leftCoords.y, leftCoords.z);
    }

    return ans;
}
