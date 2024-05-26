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
    

    glm::vec3 heightV = glm::normalize(glm::vec3(0.0f, -height, 0.0f));
    glm::vec3 heightRadiusV = glm::normalize(glm::vec3(radius, -height, 0.0f));
    
    float cos_heightV_heightRadiusV = glm::dot(heightV, heightRadiusV);
    float sin_heightV_heightRadiusV = sqrt(1 - powf(cos_heightV_heightRadiusV, 2));


    float cone_face_prep_tang = radius / std::sqrt(radius * radius + height * height);
    
    glm::vec4 currCoords;
    Vertex prev;
    Vertex base;

    glm::mat4 rotYMatrix = Consts::rotYMatrix(angle);
    glm::vec4 base_normal = glm::normalize(glm::vec4(cos_heightV_heightRadiusV, sin_heightV_heightRadiusV, 0.0f, 0.0f));
    glm::vec3 cur_normal;

    for(int i = 0; i < stacks; i++) {
        basePoints.emplace_back(0.0f , static_cast<float>(i) * stackStep, 0.0f);

        basePoints.emplace_back(radius - (static_cast<float>(i) * radiusStep) , static_cast<float>(i) * stackStep, 0.0f, base_normal.x, base_normal.y, base_normal.z, 0.0f, static_cast<float>(i) * stackStep / height);

        for (int j = 1; j <= slices; j++) {
            base = basePoints[0];
            prev = basePoints[j];

            if(j != slices) currCoords = rotYMatrix * prev.coords;
            else currCoords = basePoints[1].getCoords();

            cur_normal = glm::vec3(0.0f, -1.0f, 0.0f);

            if(i == 0){
                ans.emplace_back(currCoords.x, currCoords.y, currCoords.z, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f); // text.y == 1 -> topo da textura para dentro, text.y == 0 -> topo da textura para dentro
                ans.emplace_back(prev.coords.x, prev.coords.y, prev.coords.z, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f); // text.y == 1 -> topo da textura para dentro, text.y == 0 -> topo da textura para dentro
                ans.emplace_back(base.coords.x, base.coords.y, base.coords.z, 0.0f, -1.0f, 0.0f, 0.5f, 1.0f); // text.y == 0 -> topo da textura para dentro, text.y == 1 -> topo da textura para dentro
            }

            if(i > 0) {
                Vertex right_v = basePoints[j];
                Vertex rightDown_v = prevBasePoints[j];
                Vertex down_v = prevBasePoints[j+1];

                glm::vec4 rightCoords = right_v.getCoords();
                glm::vec4 rightDownCoords = rightDown_v.getCoords();
                glm::vec4 downCoords = down_v.getCoords();

                cur_normal = glm::normalize(
                    glm::cross(
                        glm::vec3(rightCoords.x - rightDownCoords.x, rightCoords.y - rightDownCoords.y, rightCoords.z - rightDownCoords.z),
                        glm::vec3(rightDownCoords.x - downCoords.x, rightDownCoords.y - downCoords.y, rightDownCoords.z - downCoords.z)
                    )
                );

                // Triangle  |‾/
                //           |/

                if(j != slices) {
                    ans.emplace_back(currCoords.x, currCoords.y, currCoords.z, cur_normal.x, cur_normal.y, cur_normal.z, j * angle / 360.0f, static_cast<float>(i) * stackStep / height);
                }
                else ans.emplace_back(radius - (static_cast<float>(i) * radiusStep) , static_cast<float>(i) * stackStep, 0.0f, cur_normal.x, cur_normal.y, cur_normal.z, 1.0f, static_cast<float>(i) * stackStep / height);

                ans.emplace_back(right_v.coords, cur_normal, right_v.tex_coord);
                ans.emplace_back(rightDown_v.coords, cur_normal, rightDown_v.tex_coord);

                // Triangle  /|
                //          /_|

                ans.emplace_back(rightDown_v.coords, cur_normal, rightDown_v.tex_coord);
                ans.emplace_back(down_v.coords, cur_normal, down_v.tex_coord);

                if(j != slices) {
                    ans.emplace_back(currCoords.x, currCoords.y, currCoords.z, cur_normal.x, cur_normal.y, cur_normal.z, j * angle / 360.0f, static_cast<float>(i) * stackStep / height);
                }

                else ans.emplace_back(radius - (static_cast<float>(i) * radiusStep) , static_cast<float>(i) * stackStep, 0.0f, cur_normal.x, cur_normal.y, cur_normal.z, 1.0f, static_cast<float>(i) * stackStep / height);

            }

            if(j != slices) basePoints.emplace_back(currCoords.x, currCoords.y, currCoords.z, cur_normal.x, cur_normal.y, cur_normal.z, j * angle / 360.0f, static_cast<float>(i) * stackStep / height);
            else basePoints.emplace_back(radius - (static_cast<float>(i) * radiusStep) , static_cast<float>(i) * stackStep, 0.0f, cur_normal.x, cur_normal.y, cur_normal.z, 1.0f, static_cast<float>(i) * stackStep / height); // Equivalent to basePoints[1] but with different texture coordinates
        }

        prevBasePoints.clear();
        prevBasePoints.insert(prevBasePoints.end(), basePoints.begin(), basePoints.end());

        basePoints.clear();
    }

    int size = prevBasePoints.size();

    for(unsigned int i = 2; i < size; i++){
        Vertex leftCoords = prevBasePoints[i];
        Vertex rightCoords = prevBasePoints[i-1];

        cur_normal = glm::normalize(
                    glm::cross(
                        glm::vec3(0.0f - rightCoords.coords.x, height - rightCoords.coords.y, 0.0f - rightCoords.coords.z),
                        glm::vec3(rightCoords.coords.x - leftCoords.coords.x, rightCoords.coords.y - leftCoords.coords.y, rightCoords.coords.z - leftCoords.coords.z)
                    )
                );

        ans.emplace_back(leftCoords.coords, cur_normal, leftCoords.tex_coord);
        ans.emplace_back(0.0f, height, 0.0f, 0.0f, 1.0f, 0.0f, static_cast<float>(i - 2) / static_cast<float>(size - 3), 1.0f);
        ans.emplace_back(rightCoords.coords, cur_normal, rightCoords.tex_coord);
    }

    return Consts::calcNormalAvg(ans);
}
