#include <iostream>

#include "Plane.h"

std::vector<Vertex> Plane::createPlanePoints(const float length, const int divisions) {
    std::vector<Vertex> ans = std::vector<Vertex>();
    std::vector<Vertex> ans2 = std::vector<Vertex>();

	const float fdivisions = static_cast<float>(divisions);

    const float step = length / fdivisions;

    for(int i = 0; i <= divisions; i++) { // lines (x)
        for(int j = 0; j <= divisions; j++) { // columns (z)
            ans.emplace_back(static_cast<float>(i) * step - length / 2.0f, 0.0f, static_cast<float>(j) * step - length / 2.0f);
        }
    }

    for(int i = 0; i < divisions * (divisions +1) ; i += (divisions + 1)) {
        for (int j = i; j < i + divisions; j += 1) {
            int right = j + divisions + 1;
            int down = j + 1;
            int right_down = j + divisions + 2;

            /* Triangle |\
                        |_\ */

            ans2.emplace_back(ans.at(j));
            ans2.emplace_back(ans.at(down));
            ans2.emplace_back(ans.at(right_down));

            // Triangle \‾|
            //           \|

            ans2.emplace_back(ans.at(j));
            ans2.emplace_back(ans.at(right_down));
            ans2.emplace_back(ans.at(right));
        }
    }

	// LOOP TEMPORARIO PARA ADICIONAR AS NORMAIS
	for (Vertex &vertex : ans2) {
		vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	}

    return ans2;
}

std::vector<Vertex> Plane::createPlanePointsNoTranslate(const float length, const int divisions) {
    std::vector<Vertex> ans = std::vector<Vertex>();
    std::vector<Vertex> ans2 = std::vector<Vertex>();

	const float fdivisions = static_cast<float>(divisions);

    const float step = length / fdivisions;

    for(int i = 0; i <= divisions; i++) { // lines (x)
        for(int j = 0; j <= divisions; j++) { // columns (z)
            ans.emplace_back(static_cast<float>(i) * step, 0.0f, static_cast<float>(j) * step);
        }
    }

    for(int i = 0; i < divisions * (divisions +1) ; i += (divisions + 1)) {
        for (int j = i; j < i + divisions; j += 1) {
            int right = j + divisions + 1;
            int down = j + 1;
            int right_down = j + divisions + 2;

            /* Triangle |\
                        |_\ */

            ans2.emplace_back(ans.at(j));
            ans2.emplace_back(ans.at(down));
            ans2.emplace_back(ans.at(right_down));

            // Triangle \‾|
            //           \|

            ans2.emplace_back(ans.at(j));
            ans2.emplace_back(ans.at(right_down));
            ans2.emplace_back(ans.at(right));
        }
    }

		// LOOP TEMPORARIO PARA ADICIONAR AS NORMAIS
	for (Vertex &vertex : ans2) {
		vertex.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	}

    return ans2;
}

