#include <iostream>

#include "Plane.h"

std::vector<Vertex> Plane::createPlanePoints(int length, int divisions) {
    std::vector<Vertex> ans = std::vector<Vertex>();
    std::vector<Vertex> ans2 = std::vector<Vertex>();
    const int step = length / divisions;

    for(int i=0;i<divisions;i++) { // linhas (x)
        for(int j=0;j<divisions;j++) { // colunas (z)
            ans.emplace_back(i*step - length/2.0f,0.0f,j*step - length/2.0f);

        }
    }
    int size = ans.size();

    for(int i = 0; i < size; i++) {
        int right = i + divisions + 1;
        int down = i + 1;
        int right_down = i + divisions + 2;

        if (right < size && down < size && right_down < size) {
             // triangulo que desce
            ans2.emplace_back(ans.at(i));
            ans2.emplace_back(ans.at(down));
            ans2.emplace_back(ans.at(right_down));
            // triangulo que sobe um bocado
            ans2.emplace_back(ans.at(i));
            ans2.emplace_back(ans.at(right_down));
            ans2.emplace_back(ans.at(right));
        }
    }

    return ans2;
}
