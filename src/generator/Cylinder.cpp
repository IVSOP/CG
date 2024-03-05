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
                ans.emplace_back(baseCoords.x, baseCoords.y, baseCoords.z);
                ans.emplace_back(prevCoords.x, prevCoords.y, prevCoords.z);
                ans.emplace_back(currCoords.x, currCoords.y, currCoords.z);
            }

            if(i == stacks){
                ans.emplace_back(currCoords.x, currCoords.y, currCoords.z);
                ans.emplace_back(prevCoords.x, prevCoords.y, prevCoords.z);
                ans.emplace_back(baseCoords.x, baseCoords.y, baseCoords.z);
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

    /*for(Vertex v : ans){
        // if(std::abs(v.getX()) >= 6 || std::abs(v.getY()) > 18 || std::abs(v.getZ()) >= 6) std::cout << v << std::endl;

        // if(isinf(v.getX()) || isinf(v.getY()) || isinf(v.getZ())) std::cout << v << std::endl;
    }*/

    // std::cout << ans.size() << std::endl;

    return ans;
}
