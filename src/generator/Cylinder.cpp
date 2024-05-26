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

    float down_coords_tex_u;
    float curr_coords_tex_u;

    glm::mat4 rotYMatrix = Consts::rotYMatrix(-angle);

    for(int i = 0; i <= stacks; i++) {
        basePoints.emplace_back(0.0f , static_cast<float>(i) * stackStep - height / 2.0f, 0.0f);
        basePoints.emplace_back(radius, static_cast<float>(i) * stackStep - height / 2.0f, 0.0f);

        for (int j = 1; j <= slices; j++) {
            baseCoords = basePoints[0].getCoords();
            prevCoords = basePoints[j].getCoords();

            if(j != slices) {
                currCoords = rotYMatrix * prevCoords;
                //coords texturas última linha
                down_coords_tex_u = 1 - static_cast<float>(j) / slices;
                curr_coords_tex_u = 1 - static_cast<float>(j) / slices;
            }
            else {
                currCoords = basePoints[1].getCoords();

                //coords texturas última linha
                down_coords_tex_u = 0.0f;
                curr_coords_tex_u = 0.0f;
            }

            if(i == 0){
                ans.emplace_back(baseCoords.x, baseCoords.y, baseCoords.z,0.0f,-1.0f,0.0f,0.5f,0.0f); // pegar no y do topo da imagem e x do meio (aka 0.5)
                ans.emplace_back(prevCoords.x, prevCoords.y, prevCoords.z,0.0f,-1.0f,0.0f,1.0f,1.0f); // pegar no x mais à esq, no y de baixo de todo
                ans.emplace_back(currCoords.x, currCoords.y, currCoords.z,0.0f,-1.0f,0.0f,0.0f,1.0f); // pegar no x mais à dir, no y de baixo de todo
            }

            if(i == stacks){
                ans.emplace_back(currCoords.x, currCoords.y, currCoords.z,0.0f,1.0f,0.0f,1.0f,1.0f); // pegar no x mais à esq, no y de baixo de todo
                ans.emplace_back(prevCoords.x, prevCoords.y, prevCoords.z,0.0f,1.0f,0.0f,0.0f,1.0f); // pegar no x mais à dir, no y de baixo de todo
                ans.emplace_back(baseCoords.x, baseCoords.y, baseCoords.z,0.0f,1.0f,0.0f,0.5f,0.0f); // pegar no y do topo da imagem e x do meio (aka 0.5)
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

                ans.emplace_back(rightDownCoords.x, rightDownCoords.y, rightDownCoords.z,rightDownCoordsNormal.x,0.0f,rightDownCoordsNormal.z, 1 - static_cast<float>(j-1) / slices, static_cast<float>(i-1) / stacks);
                ans.emplace_back(rightCoords.x, rightCoords.y, rightCoords.z,rightCoordsNormal.x,0.0f,rightCoordsNormal.z, 1 - static_cast<float>(j-1) / slices, static_cast<float>(i) / stacks);
                ans.emplace_back(currCoords.x, currCoords.y, currCoords.z,currCoordsNormal.x,0.0f,currCoordsNormal.z, curr_coords_tex_u, static_cast<float>(i) / stacks);

                // Triangle  /|
                //          /_|

                ans.emplace_back(currCoords.x, currCoords.y, currCoords.z,currCoordsNormal.x,0.0f,currCoordsNormal.z, curr_coords_tex_u, static_cast<float>(i) / stacks);
                ans.emplace_back(downCoords.x, downCoords.y, downCoords.z,downCoordsNormal.x,0.0f,downCoordsNormal.z, down_coords_tex_u, static_cast<float>(i-1) / stacks);
                ans.emplace_back(rightDownCoords.x, rightDownCoords.y, rightDownCoords.z, rightDownCoordsNormal.x,0.0f,rightDownCoordsNormal.z, 1 - static_cast<float>(j-1) / slices, static_cast<float>(i-1) / stacks);
            }

            basePoints.emplace_back(currCoords.x, currCoords.y, currCoords.z);
        }

        prevBasePoints.clear();
        prevBasePoints.insert(prevBasePoints.end(), basePoints.begin(), basePoints.end());

        basePoints.clear();
    }

    return ans;
}
