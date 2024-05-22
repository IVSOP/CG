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
    for (int currStack = 0; currStack <= stacks; currStack++) {
        currPhi = (M_PI / 2) - stackStep * currStack; // partindo do phi no topo, aka 90º ir descendo até chegar a -90º, segundo stackStep
        midXZCalc = radius * std::cos(currPhi); // calcular o (r * cos(phi)), cálculo idêntico em x e y
        currY = radius * std::sin(currPhi); 

        texV = 1.0f - (static_cast<float>(currStack) / stacks); // em openGL, origem de imagem é canto esq. inferior, logo ir de 1 até 0 na coord V!
            
        for(int currSlice = 0; currSlice < slices; currSlice++) {

            texU = static_cast<float>(currSlice) / slices;

            if (currStack == 0) {
                ans.emplace_back(0.0f,radius,0.0f,0.0f,1.0f,0.0f,texU,1.0f); // pontos correspondentes ao ponto mais alto da esfera
                // ans.emplace_back(0.0f,radius,0.0f,0.0f,1.0f,0.0f,texU + 1.0f / (slices * 2),1.0f); // alternativa para os triângulos parecerem centrados, meter que pontos do topo estão em texU = media entre texU do pontos da linha seguinte na esfera
            }

            else if (currStack == stacks) {
                ans.emplace_back(0.0f,-radius,0.0f,0.0f,-1.0f,0.0f,texU,0.0f); // pontos correspondentes ao ponto mais baixo da esfera
                // ans.emplace_back(0.0f,-radius,0.0f,0.0f,-1.0f,0.0f,texU + 1.0f / (slices * 2),0.0f); // alternativa para os triângulos parecerem centrados
            }

            else {
                currTheta = sectorStep * currSlice;
                currZ = midXZCalc * std::cos(currTheta);
                currX = midXZCalc * std::sin(currTheta);

                normX = currX * normFactor;
                normY = currY * normFactor;
                normZ = currZ * normFactor;

                ans.emplace_back(currX,currY,currZ,normX,normY,normZ,texU,texV); // outros pontos que hao o mais alto/ o mais baixo da esfera
            }
        }
    }

    int currLinePoints, nextLinePoints;

    // Criar triângulos
    for (int currStack = 0; currStack  < stacks; currStack ++) { // iterar sobre todas as stacks
        currLinePoints = currStack  * slices; // início da linha de pontos atual
        nextLinePoints = (currStack +1) * slices; // início da próxima linha de pontos
        
        for (int currSlice = 0; currSlice < slices; currSlice ++) {
            Vertex lastVertexCurrLine = ans[currLinePoints + ((currSlice + 1) % slices)];
            Vertex lastVertexNextLine = ans[nextLinePoints + ((currSlice + 1) % slices)];

            if (currSlice + 1 == slices) { // preciso de explicitamente meter o text_U da última coordenada da linha a 1, para nao dar texture seams!
                lastVertexCurrLine.tex_coord.x = 1.0f;
                lastVertexNextLine.tex_coord.x = 1.0f;
            }
            // printf("%d %d\n",currLinePoints, nextLinePoints);
            // para cada quadrado do círculo fazem-se dois triangulos:
            /* |\
               |_\ */
            ans2.emplace_back(ans[currLinePoints + currSlice]);
            ans2.emplace_back(ans[nextLinePoints + currSlice]);
            ans2.emplace_back(lastVertexNextLine);

            /*  /|
               /_| */
            ans2.emplace_back(ans[currLinePoints + currSlice]);
            ans2.emplace_back(lastVertexNextLine);
            ans2.emplace_back(lastVertexCurrLine);
        }
    }

    return ans2;
}

