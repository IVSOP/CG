#include <iostream>

#include "Sphere.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>
// stacks -> n de linhas horizontais
// slices -> n de colunas verticais

// x = (r * cos(phi)) * cos(theta)
// y = (r * cos(phi)) * sin(theta)
// z = r * sin(theta)

std::vector<Vertex> Sphere::createSpherePoints(const float radius, const int slices, const int stacks) {
    std::vector<Vertex> ans = std::vector<Vertex>();
    std::vector<Vertex> ans2 = std::vector<Vertex>();
    float fslices = static_cast<float>(slices);
    float fstacks = static_cast<float>(stacks);

    float sectorStep = 2 * M_PI / slices; // theta vai de 0 a 360º aka 0 a 2pi radianos
    float stackStep = M_PI / stacks; // phi vai de 90º a -90º aka pi/2 a -pi/2

    float currPhi;
    float currTheta;

    float currX;
    float currY;
    float currZ;
    float midXYCalc;

    //obter os pontos que constituem a esfera
    for (int currStack = 1; currStack < stacks; currStack++) {
        currPhi = (M_PI / 2) - stackStep * currStack; // partindo do phi no topo, aka 90º ir descendo até chegar a -90º, segundo stackStep
        midXYCalc = radius * cos(currPhi); // calcular o (r * cos(phi)), cálculo idêntico em x e y
        currZ = radius * sin(currPhi); 

        for(int currSlice = 0; currSlice < slices; currSlice++) {
            currTheta = sectorStep * currSlice;
            currX = midXYCalc * cos(currTheta);
            currY = midXYCalc * sin(currTheta);

            ans.emplace_back(currX,currY,currZ);
        }
    }

    for (auto point: ans) {
        std::cout << point << std::endl;
        std::cout << glm::length(point.getCoords()) << std::endl;
    }
    printf("Total points:%lu",ans.size());

    int currLinePoints, nextLinePoints;

    // Criar triângulos
    // for (int i=0; i < (stacks -1); i += 1) { // iterar sobre todas as stacks
    //     currLinePoints = i * (slices +1); // início da linha de pontos atual
    //     nextLinePoints = (i+1) * (slices+1); // início da próxima linha de pontos
        
    //     for (int j=0; i < slices; i++) {
    //         // para cada quadrado do círculo fazem-se dois triangulos:
    //         // |.
    //         // |_.
    //         ans2.emplace_back(ans[currLinePoints + j]);
    //         ans2.emplace_back(ans[nextLinePoints + j]);
    //         ans2.emplace_back(ans[nextLinePoints + j + 1]);
    //         //  .|
    //         // ._|
    //         ans2.emplace_back(ans[currLinePoints + j]);
    //         ans2.emplace_back(ans[nextLinePoints + j + 1]);
    //         ans2.emplace_back(ans[currLinePoints + j + 1]);
    //     }
    // }

    for(auto point: ans) {
        std::cout << point << std::endl;
    }

    return ans;
}
