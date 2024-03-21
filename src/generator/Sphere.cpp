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

    float currX;
    float currY;
    float currZ;
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

            ans.emplace_back(currX,currY,currZ);
        }
    }

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
    for (int i=0; i < slices; i++) { // ligar o ponto no fundo da esfera aos pontos da última camada
        ans2.emplace_back(ans[lastPos - i]);
        ans2.emplace_back(ans[lastPos - ((i+1) % slices)]);
        ans2.emplace_back(0,-radius,0);
    }


	// LOOP VETORES NORMAIS
	// normal = angulo do centro ate ao vertice. nao deve estar 100% correto mas esta quase
	for (Vertex &vertex : ans2) {
		// assumo que centro e (0.0f, 0.0f, 0.0f)
		glm::vec3 normal = glm::vec3(vertex.coords) - glm::vec3(0.0f, 0.0f, 0.0f);
		vertex.normal = glm::normalize(normal);

        //LOOP TEXTURAS
        vertex.tex_coord[0] = asin(vertex.normal.x)/ M_PI + 0.5;
        vertex.tex_coord[1] = asin(vertex.normal.y) / M_PI + 0.5;
	}

    return ans2;
}
