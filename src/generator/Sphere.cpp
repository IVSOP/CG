#include <iostream>

#include "Sphere.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>
// stacks -> n de linhas horizontais
// slices -> n de colunas verticais

// considerando y como o eixo que sobe
// z = (r * cos(phi)) * cos(theta)
// x = (r * cos(phi)) * sin(theta)
// y = r * sin(theta)

std::vector<Vertex> Sphere::createSpherePoints(const float radius, const int slices, const int stacks) {
    std::vector<Vertex> ans = std::vector<Vertex>();
    std::vector<Vertex> ans2 = std::vector<Vertex>();

    float sectorStep = 2 * M_PI / slices; // theta vai de 0 a 360º aka 0 a 2pi radianos
    float stackStep = M_PI / stacks; // phi vai de 90º a -90º aka pi/2 a -pi/2

    float currPhi;
    float currTheta;

    float currX;
    float currY;
    float currZ;
    float midXZCalc;

    //obter os pontos que constituem a esfera
    for (int currStack = 1; currStack < stacks; currStack++) {
        currPhi = (M_PI / 2) - stackStep * currStack; // partindo do phi no topo, aka 90º ir descendo até chegar a -90º, segundo stackStep
        midXZCalc = radius * cos(currPhi); // calcular o (r * cos(phi)), cálculo idêntico em x e y
        currY = radius * sin(currPhi); 

        for(int currSlice = 0; currSlice < slices; currSlice++) {
            currTheta = sectorStep * currSlice;
            currZ = midXZCalc * cos(currTheta);
            currX = midXZCalc * sin(currTheta);

            ans.emplace_back(currX,currY,currZ);
        }
    }

    // for (auto point: ans) {
    //     std::cout << glm::length(point.getCoords()) << std::endl;
    //     std::cout << point << std::endl;
    // }
    // printf("Initial points:%lu\n",ans.size());

    int currLinePoints, nextLinePoints;

    for (int i=0; i < slices; i++) { // ligar o ponto no topo da esfera aos pontos da primeira camada
        ans2.emplace_back(0,radius,0);
        ans2.emplace_back(ans[i]);
        ans2.emplace_back(ans[(i+1) % slices]);
    }

    // Criar triângulos
    for (int i=0; i < (stacks -2); i += 1) { // iterar sobre todas as stacks
        currLinePoints = i * slices; // início da linha de pontos atual
        nextLinePoints = (i+1) * slices; // início da próxima linha de pontos
        
        for (int j=0; j < slices; j++) {
            // printf("%d %d\n",currLinePoints, nextLinePoints);
            // para cada quadrado do círculo fazem-se dois triangulos:
            /* |\
               |_\ */
            ans2.emplace_back(ans[currLinePoints + j]);
            ans2.emplace_back(ans[nextLinePoints + j]);
            ans2.emplace_back(ans[nextLinePoints + ((j + 1) % slices)]);
            /*  /|
               /_| */
            ans2.emplace_back(ans[currLinePoints + j]);
            ans2.emplace_back(ans[nextLinePoints + ((j + 1) % slices)]);
            ans2.emplace_back(ans[currLinePoints + ((j + 1) % slices)]);
        }
    }

    int lastPos = ans.size() - 1;
    for (int i=0; i < slices; i++) { // ligar o ponto no topo da esfera aos pontos da primeira camada
        ans2.emplace_back(ans[lastPos - i]);
        ans2.emplace_back(ans[lastPos - ((i+1) % slices)]);
        ans2.emplace_back(0,-radius,0);
        // printf("iter: %d\n",i);
        // std::cout << ans[lastPos - ((i+1) % slices)] << std::endl;
        // std::cout << ans[lastPos - i] << std::endl;
    }

    // for(auto point: ans) {
    //     std::cout << point << std::endl;
    // }
    // printf("Total points:%lu\n",ans2.size());
    return ans2;
}
