#include <iostream>
#include "Cone.h"
#include "Consts.h"

#define _USE_MATH_DEFINES
#include <math.h>

std::vector<Vertex> Cone::createConePoints(float radius, float height, int slices, int stacks) {
    std::vector<Vertex> basePoints = std::vector<Vertex>();
    std::vector<Vertex> prevBasePoints = std::vector<Vertex>();
    std::vector<Vertex> ans = std::vector<Vertex>();

    float angle = 360.0f / static_cast<float>(slices);
    float stackStep = height / static_cast<float>(stacks);
    float radiusStep = radius / static_cast<float>(stacks);
    float cone_face_prep_tang = height / radius / 2.0f;

    glm::vec4 currCoords;
    glm::vec4 prevCoords;
    glm::vec4 baseCoords;

    glm::mat4 rotYMatrix = Consts::rotYMatrix(-angle);

    for(int i = 0; i < stacks; i++) {
        basePoints.emplace_back(0.0f , static_cast<float>(i) * stackStep, 0.0f);

        glm::vec3 base_normal = glm::normalize(glm::vec3(1.0f, cone_face_prep_tang, 0.0f));

        basePoints.emplace_back(radius - (static_cast<float>(i) * radiusStep) , static_cast<float>(i) * stackStep, 0.0f, base_normal.x, base_normal.y, base_normal.z, 0.0f, static_cast<float>(i) * stackStep / height);

        for (int j = 1; j <= slices; j++) {
            baseCoords = basePoints[0].getCoords();
            prevCoords = basePoints[j].getCoords();

            if(j != slices) currCoords = rotYMatrix * prevCoords;
            else currCoords = basePoints[1].getCoords();

            glm::vec3 cur_normal = glm::normalize(glm::vec3(cos(glm::radians(angle * static_cast<float>(j))), cone_face_prep_tang, sin(glm::radians(angle * static_cast<float>(j)))));

            if(i == 0){
                ans.emplace_back(baseCoords.x, baseCoords.y, baseCoords.z, 0.0f, -1.0f, 0.0f);
                ans.emplace_back(prevCoords.x, prevCoords.y, prevCoords.z, 0.0f, -1.0f, 0.0f);
                ans.emplace_back(currCoords.x, currCoords.y, currCoords.z, 0.0f, -1.0f, 0.0f);
            }

            if(i > 0) {
                Vertex right_v = basePoints[j];
                Vertex rightDown_v = prevBasePoints[j];
                Vertex down_v = prevBasePoints[j+1];

                glm::vec4 rightCoords = right_v.getCoords();
                glm::vec4 rightDownCoords = rightDown_v.getCoords();
                glm::vec4 downCoords = down_v.getCoords();

                // Triangle  |‾/
                //           |/

                ans.emplace_back(rightDown_v);
                ans.emplace_back(right_v);

                if(j != slices) {
                    ans.emplace_back(currCoords.x, currCoords.y, currCoords.z, cur_normal.x, cur_normal.y, cur_normal.z, j * angle / 360.0f, static_cast<float>(i) * stackStep / height);
                }
                else ans.emplace_back(basePoints[1]);

                // Triangle  /|
                //          /_|

                if(j != slices) {
                    ans.emplace_back(currCoords.x, currCoords.y, currCoords.z, cur_normal.x, cur_normal.y, cur_normal.z, j * angle / 360.0f, static_cast<float>(i) * stackStep / height);
                }

                else ans.emplace_back(basePoints[1]);

                ans.emplace_back(down_v);
                ans.emplace_back(rightDown_v);
            }

            if(j != slices) basePoints.emplace_back(currCoords.x, currCoords.y, currCoords.z, cur_normal.x, cur_normal.y, cur_normal.z, j * angle / 306.0f, static_cast<float>(i) * stackStep / height);
            else basePoints.emplace_back(basePoints[1]);
        }

        prevBasePoints.clear();
        prevBasePoints.insert(prevBasePoints.end(), basePoints.begin(), basePoints.end());

        basePoints.clear();
    }


    for(unsigned int i = 2; i < prevBasePoints.size(); i++){
        Vertex leftCoords = prevBasePoints[i];
        Vertex rightCoords = prevBasePoints[i-1];

        ans.emplace_back(rightCoords);
        ans.emplace_back(0.0f, height, 0.0f, 0.0f, 1.0f, 0.0f);
        ans.emplace_back(leftCoords);
    }

    return ans;
}
