#include <iostream>

#include "Sphere.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/norm.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

// #define M_PI 3.14159265358979323846f

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

    float currX, currY, currZ; // coords pontos
    float normX, normY, normZ; // coords normal
    float texU, texV; // coords texturas
    float normFactor = 1.0f / radius; // basta dividir por raio os pontos, "resto" da ormalização já é obtida das coordenadas esféricas
    float midXZCalc;

    //obter os pontos que constituem a esfera
    for (int currStack = 1; currStack < stacks; currStack++) {
        currPhi = (M_PI / 2) - stackStep * currStack; // partindo do phi no topo, aka 90º ir descendo até chegar a -90º, segundo stackStep
        midXZCalc = radius * std::cos(currPhi); // calcular o (r * cos(phi)), cálculo idêntico em x e y
        currY = radius * std::sin(currPhi); 

        for(int currSlice = 0; currSlice < slices; currSlice++) {
            currTheta = sectorStep * currSlice;
            currZ = midXZCalc * std::cos(currTheta);
            currX = midXZCalc * std::sin(currTheta);

            normX = currX * normFactor;
            normY = currY * normFactor;
            normZ = currZ * normFactor;

            texU = static_cast<float>(currSlice) / slices;
            texV = 1 - static_cast<float>(currStack) / stacks; // em openGL, origem de imagem é canto esq. inferior, logo ir de 1 até 0 na coord V!

            ans.emplace_back(currX,currY,currZ,normX,normY,normZ,texU,texV);
        }
    }

    int currLinePoints, nextLinePoints;
    
    // ligar o ponto no topo da esfera aos pontos da primeira camada
    for (int i=0; i < slices; i++) { 

        texU = static_cast<float>(i) / slices;
        texV = 1.0f;
        ans2.emplace_back(0.0f,radius,0.0f,0.0f,1.0f,0.0f, texU, texV); // normal do ponto do topo é vetor vertical virado para cima
        ans2.emplace_back(ans[i]);
        
        Vertex lastVertex = ans[(i+1) % slices];
        if (i + 1 == slices) {
            lastVertex.tex_coord.x = 1.0f;
        }
        ans2.emplace_back(lastVertex);
    }

    // Criar triângulos
    for (int i=0; i < (stacks -2); i += 1) { // iterar sobre todas as stacks
        currLinePoints = i * slices; // início da linha de pontos atual
        nextLinePoints = (i+1) * slices; // início da próxima linha de pontos
        
        for (int j=0; j < slices; j++) {
            Vertex lastVertexCurrLine = ans[currLinePoints + ((j + 1) % slices)];
            Vertex lastVertexNextLine = ans[nextLinePoints + ((j + 1) % slices)];

            if (j + 1 == slices) { // preciso de explicitamente meter o text_V da última coordenada da linha a 1, para nao dar texture seams!
                lastVertexCurrLine.tex_coord.x = 1.0f;
                lastVertexNextLine.tex_coord.x = 1.0f;
            }
            // printf("%d %d\n",currLinePoints, nextLinePoints);
            // para cada quadrado do círculo fazem-se dois triangulos:
            /* |\
               |_\ */
            ans2.emplace_back(ans[currLinePoints + j]);
            ans2.emplace_back(ans[nextLinePoints + j]);
            ans2.emplace_back(lastVertexNextLine);



            /*  /|
               /_| */
            ans2.emplace_back(ans[currLinePoints + j]);
            ans2.emplace_back(lastVertexNextLine);
            ans2.emplace_back(lastVertexCurrLine);
        }
    }

    int lastPos = ans.size() - 1;
    for (int i = 0; i < slices; i++) { // ligar o ponto no fundo da esfera aos pontos da última camada

        Vertex lastVertex = ans[lastPos - ((i+1) % slices)];
        if (i + 1 == slices) {
            lastVertex.tex_coord.x = 1.0f;
        }
        ans2.emplace_back(ans[lastPos - i]);
        ans2.emplace_back(lastVertex);

        texU = static_cast<float>(i)/slices;
        texV = 0.0f; // última linha da textura
        ans2.emplace_back(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, texU , texV);
    }

    return ans2;
}
