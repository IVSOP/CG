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
    float normFactor = 1.0f / torusRadius;

    glm::vec4 currCoords;
    glm::vec4 prevCoords;


    glm::mat4 rotYMatrix = Consts::rotYMatrix(-angleSlices);
    glm::mat4 rotZMatrix = Consts::rotZMatrix(90.0f);
    glm::mat4 rotYMatrix2 = Consts::rotYMatrix(180.0f);

    //aux vars
    glm::mat4 rotMatrix;
    glm::mat4 translateMatrix;
    glm::vec4 rotatedPosition;
    glm::vec3 centerSpherePoint;
    glm::vec3 normal;

    float texU;
    float texV;

    circuloPoints.emplace_back(torusRadius, 0.0f, 0.0f);

    // Criar o circulo no plano xz
    for (int j = 0; j < slices; j++) {
        prevCoords = j == 0 ? circuloPoints[j].getCoords() : circuloPoints2[j-1].getCoords();

        currCoords = rotYMatrix * prevCoords;

        circuloPoints2.emplace_back(currCoords.x, currCoords.y, currCoords.z);
    }

    // rodar o circulo 90 graus
    for (auto point: circuloPoints2) {
        currCoords = rotYMatrix2 * rotZMatrix * point.getCoords();
        circuloPoints3.emplace_back(currCoords.x, currCoords.y, currCoords.z);
    }

    // Criar os circulos nas posições certas
    for(int i = 0; i < stacks; i++) {
        for (int j = 0; j < slices; j++) {
            currCoords = circuloPoints3[j].getCoords();
            // calcular coordenadas
            rotMatrix = Consts::rotYMatrix(angleStacks * i);
            translateMatrix = Consts::translateMatrix(torusRadiusPos * sin(glm::radians(angleStacks * i)), 0.0f, torusRadiusPos * cos(glm::radians(angleStacks * i))); 
            rotatedPosition = translateMatrix * rotMatrix * currCoords;
            //calcular normais
            centerSpherePoint = glm::vec3(translateMatrix[3]); // A ultima linha da matriz de transformação corresponde ao vetor de transformação, que correpsonde ao ponto final se se aplicar a transformação à origem!
            normal = (glm::vec3(rotatedPosition) - centerSpherePoint) * normFactor; // normal de cada ponto -> vetor entre centro da circunferência e respetivo ponto

            texU = static_cast<float>(i) / stacks;
            texV = static_cast<float>(j) / slices; // Não é 1 - porque ordem de percorrer os pontos de cada circunferência já é contrária

            circuloRotTransPoints.emplace_back(rotatedPosition.x, rotatedPosition.y, rotatedPosition.z, normal.x,normal.y,normal.z,texU,texV);
        }
    }

    // // Criar triângulos
    int currLinePoints, nextLinePoints;
    for (int i = 0; i < stacks; i += 1){ // iterar sobre todas as stacks
        currLinePoints = i * slices; // início da linha de pontos atual
        nextLinePoints = i != stacks-1 ? (i+1) * slices : 0; // início da próxima linha de pontos

        for (int j = 0; j < slices; j++) {

            Vertex currVertexCurrCirc = circuloRotTransPoints[currLinePoints + j];
            Vertex lastVertexCurrCirc = circuloRotTransPoints[currLinePoints + ((j + 1) % slices)];

            Vertex currVertexNextCirc = circuloRotTransPoints[nextLinePoints + j];
            Vertex lastVertexNextCirc = circuloRotTransPoints[nextLinePoints + ((j + 1) % slices)];
                    
            if (i + 1 == stacks) {
                // currVertexCurrCirc.tex_coord.x = 1.0f;
                // lastVertexCurrCirc.tex_coord.x = 1.0f;
                currVertexNextCirc.tex_coord.x = 1.0f;
                lastVertexNextCirc.tex_coord.x = 1.0f;
            }

            if (j + 1 == slices) { // preciso de explicitamente meter o text_V da última coordenada da linha a 1, para nao dar texture seams!
                lastVertexCurrCirc.tex_coord.y = 1.0f;
                lastVertexNextCirc.tex_coord.y = 1.0f;
            }
                // para cada quadrado do círculo fazem-se dois triangulos:
                /* |\
                |_\ */
                ans.emplace_back(currVertexCurrCirc);
                ans.emplace_back(currVertexNextCirc);
                ans.emplace_back(lastVertexNextCirc);
                /*  /|
                /_| */
                ans.emplace_back(currVertexCurrCirc);
                ans.emplace_back(lastVertexNextCirc);
                ans.emplace_back(lastVertexCurrCirc);
        }
    }
    return ans;
}
