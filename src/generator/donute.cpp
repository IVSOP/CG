#include "Torus.h"

#include <iostream>
#include "Consts.h"

#include <math.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// criar circunferencia no meio e dar translate para a posição certa
// Stack é o número de circulos, slices é as fatias
std::vector<Vertex> Torus::createTorusPoints(float intRadius, float extRadius, int slices, int stacks) {
    std::vector<Vertex> circuloPoints;  // circulo no eixo xz para colar pelas slices
    std::vector<Vertex> circuloPoints2;
    std::vector<Vertex> circuloPoints3;
    std::vector<Vertex> circuloRotTransPoints;
    std::vector<Vertex> ans;

    float torusRadius = (extRadius - intRadius) / 2.0f;
    float torusRadiusPos = intRadius + (extRadius - intRadius) / 2.0f; // distancia ponto medio r1 e r2
    float angleSlices = 360.0f / static_cast<float>(slices);  // angulo para criar as fatias do donut
    float angleStacks = 360.0f / static_cast<float>(stacks);  // angulo para criar as circunferencias à volta de cada ponto

    glm::vec4 currCoords;
    glm::vec4 prevCoords;


    glm::mat4 rotYMatrix = Consts::rotYMatrix(-angleSlices);
    glm::mat4 rotZMatrix = Consts::rotZMatrix(90.0f);
    glm::mat4 rotYMatrix2 = Consts::rotYMatrix(180.0f);

    circuloPoints.emplace_back(torusRadius, 0.0f, 0.0f);

    // Criar o circulo no plano xz
    for (int j = 0; j < slices; j++) {
        prevCoords = j == 0 ? circuloPoints[j].getCoords() : circuloPoints2[j-1].getCoords();

        currCoords = rotYMatrix * prevCoords;

        circuloPoints2.emplace_back(currCoords.x, currCoords.y, currCoords.z);
    }

    // rodar o circulo 90 graus
    for (auto point: circuloPoints2) {
        currCoords = rotYMatrix * rotZMatrix * point.getCoords();
        circuloPoints3.emplace_back(currCoords.x, currCoords.y, currCoords.z);
    }

    // Criar os circulos nas posições certas
    for(int i = 0; i < stacks; i++) {
        for (auto point : circuloPoints3) {
            currCoords = point.getCoords();

            glm::mat4 rotMatrix = Consts::rotYMatrix(angleStacks * i);
            glm::mat4 translateMatrix = Consts::translateMatrix(torusRadiusPos * sin(glm::radians(angleStacks * i)), 0.0f, torusRadiusPos * cos(glm::radians(angleStacks * i))); 
            glm::vec4 rotatedPosition = translateMatrix * rotMatrix * currCoords;
            
            circuloRotTransPoints.emplace_back(rotatedPosition.x, rotatedPosition.y, rotatedPosition.z);
            // std::cout << "Posição: " << i <<  " | x: " << rotatedPosition.x << " |  y: " << rotatedPosition.y << " | z: " <<  rotatedPosition.z << std::endl;
        }
    }

    // // Criar triângulos
    int currLinePoints, nextLinePoints;
    for (int i = 0; i < stacks; i += 1){ // iterar sobre todas as stacks
        currLinePoints = i * slices; // início da linha de pontos atual
        nextLinePoints = i != stacks-1 ? (i+1) * slices : 0; // início da próxima linha de pontos
        
        for (int j = 0; j < slices; j++) {
            // printf("%d %d\n",currLinePoints, nextLinePoints);
            // para cada quadrado do círculo fazem-se dois triangulos:
            /* |\
               |_\ */
            ans.emplace_back(circuloRotTransPoints[currLinePoints + j]);
            ans.emplace_back(circuloRotTransPoints[nextLinePoints + j]);
            ans.emplace_back(circuloRotTransPoints[nextLinePoints + ((j + 1) % slices)]);
            /*  /|
               /_| */
            ans.emplace_back(circuloRotTransPoints[currLinePoints + j]);
            ans.emplace_back(circuloRotTransPoints[nextLinePoints + ((j + 1) % slices)]);
            ans.emplace_back(circuloRotTransPoints[currLinePoints + ((j + 1) % slices)]);
        }
    }

    return ans;
}
