#include <iostream>

#include "Plane.h"

std::vector<Vertex> Plane::createPlanePoints(const int length, const int divisions) {
    std::vector<Vertex> ans = std::vector<Vertex>();
    std::vector<Vertex> ans2 = std::vector<Vertex>();

	const float flength = static_cast<float>(length);
	const float fdivisions = static_cast<float>(divisions);

    const float step = flength / fdivisions;

    for(int i = 0; i <= divisions; i++) { // linhas (x)
        for(int j = 0; j <= divisions; j++) { // colunas (z)
            ans.emplace_back(static_cast<float>(i) * step - flength / 2.0f, 0.0f, static_cast<float>(j) * step - flength / 2.0f);
        }
    }

    int size = ans.size();

    for(int i = 0; i < divisions * (divisions +1) ; i += (divisions + 1)) {
        for (int j = i; j < i + divisions; j += 1) {
            int right = j + divisions + 1;
            int down = j + 1;
            int right_down = j + divisions + 2;
            // triangulo que desce
            ans2.emplace_back(ans.at(j));
            ans2.emplace_back(ans.at(down));
            ans2.emplace_back(ans.at(right_down));
            // triangulo que sobe um bocado
            ans2.emplace_back(ans.at(j));
            ans2.emplace_back(ans.at(right_down));
            ans2.emplace_back(ans.at(right));
        }
    }

    return ans2;
}

std::vector<Vertex> Plane::createPlanePointsNoTranslate(const int length, const int divisions) {
    std::vector<Vertex> ans = std::vector<Vertex>();
    std::vector<Vertex> ans2 = std::vector<Vertex>();

	const float flength = static_cast<float>(length);
	const float fdivisions = static_cast<float>(divisions);

    const float step = flength / fdivisions;

    for(int i = 0; i <= divisions; i++) { // linhas (x)
        for(int j = 0; j <= divisions; j++) { // colunas (z)
            ans.emplace_back(static_cast<float>(i) * step, 0.0f, static_cast<float>(j) * step);
        }
    }

    int size = ans.size();

    for(int i = 0; i < divisions * (divisions +1) ; i += (divisions + 1)) {
        for (int j = i; j < i + divisions; j += 1) {
            int right = j + divisions + 1;
            int down = j + 1;
            int right_down = j + divisions + 2;
            // triangulo que desce
            ans2.emplace_back(ans.at(j));
            ans2.emplace_back(ans.at(down));
            ans2.emplace_back(ans.at(right_down));
            // triangulo que sobe um bocado
            ans2.emplace_back(ans.at(j));
            ans2.emplace_back(ans.at(right_down));
            ans2.emplace_back(ans.at(right));
        }
    }

    return ans2;
}

