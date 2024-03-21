#include <iostream>

#include "Plane.h"

std::vector<Vertex> Plane::createPlanePoints_TextSquareBySquare(const float length, const int divisions){
    std::vector<Vertex> ans = std::vector<Vertex>();
    std::vector<Vertex> ans2 = std::vector<Vertex>();

    const float fdivisions = static_cast<float>(divisions);

    const float step = length / fdivisions;

    for(int i = 0; i <= divisions; i++) { // lines (x)
        for(int j = 0; j <= divisions; j++) { // columns (z)
            ans.emplace_back(static_cast<float>(i) * step - length / 2.0f, 0.0f, static_cast<float>(j) * step - length / 2.0f, 0.0f, 1.0f, 0.0f);
        }
    }

    for(int i = 0; i < divisions * (divisions +1) ; i += (divisions + 1)) {
        for (int j = i; j < i + divisions; j += 1) {
            int right = j + divisions + 1;
            int down = j + 1;
            int right_down = j + divisions + 2;

            Vertex cur_v = ans.at(j);
            Vertex down_v = ans.at(down);
            Vertex right_down_v = ans.at(right_down);
            Vertex right_v = ans.at(right);

            /* Triangle |\
                        |_\ */

            // Texture applied square by square
            // Emplace v.coords, v.normal, v.texture
            ans2.emplace_back(cur_v.coords.x, cur_v.coords.y, cur_v.coords.z, cur_v.normal.x, cur_v.normal.y, cur_v.normal.z, 0.0f, 1.0f);
            ans2.emplace_back(down_v.coords.x, down_v.coords.y, down_v.coords.z, down_v.normal.x, down_v.normal.y, down_v.normal.z, 0.0f, 0.0f);
            ans2.emplace_back(right_down_v.coords.x, right_down_v.coords.y, right_down_v.coords.z, right_down_v.normal.x, right_down_v.normal.y, right_down_v.normal.z, 1.0f, 0.0f);

            // Triangle \‾|
            //           \|

            // Emplace v.coords, v.normal, v.texture
            ans2.emplace_back(cur_v.coords.x, cur_v.coords.y, cur_v.coords.z, cur_v.normal.x, cur_v.normal.y, cur_v.normal.z, 0.0f, 1.0f);
            ans2.emplace_back(right_down_v.coords.x, right_down_v.coords.y, right_down_v.coords.z, right_down_v.normal.x, right_down_v.normal.y, right_down_v.normal.z, 1.0f, 0.0f);
            ans2.emplace_back(right_v.coords.x, right_v.coords.y, right_v.coords.z, right_v.normal.x, right_v.normal.y, right_v.normal.z, 1.0f, 1.0f);
        }
    }

    return ans2;
}

std::vector<Vertex> Plane::createPlanePoints_TextWholeObj(const float length, const int divisions){
    std::vector<Vertex> ans = std::vector<Vertex>();
    std::vector<Vertex> ans2 = std::vector<Vertex>();

    const float fdivisions = static_cast<float>(divisions);

    const float step = length / fdivisions;

    for(int i = 0; i <= divisions; i++) { // lines (x)
        for(int j = 0; j <= divisions; j++) { // columns (z)
            ans.emplace_back(static_cast<float>(i) * step - length / 2.0f, 0.0f, static_cast<float>(j) * step - length / 2.0f, 0.0f, 1.0f, 0.0f, 1.0f - (step * static_cast<float>(i) / length), 1.0f - (step * static_cast<float>(j) / length));
        }
    }

    for(int i = 0; i < divisions * (divisions +1) ; i += (divisions + 1)) {
        for (int j = i; j < i + divisions; j += 1) {
            int right = j + divisions + 1;
            int down = j + 1;
            int right_down = j + divisions + 2;

            Vertex cur_v = ans.at(j);
            Vertex down_v = ans.at(down);
            Vertex right_down_v = ans.at(right_down);
            Vertex right_v = ans.at(right);

            /* Triangle |\
                        |_\ */

            // Texture applied square by square
            // Emplace v.coords, v.normal, v.texture
            ans2.emplace_back(cur_v);
            ans2.emplace_back(down_v);
            ans2.emplace_back(right_down_v);

            // Triangle \‾|
            //           \|

            // Emplace v.coords, v.normal, v.texture
            ans2.emplace_back(cur_v);
            ans2.emplace_back(right_down_v);
            ans2.emplace_back(right_v);
        }
    }

    return ans2;
}

std::vector<Vertex> Plane::createPlanePoints(const float length, const int divisions) {
    return Plane::createPlanePoints_TextWholeObj(length, divisions);
}

std::vector<Vertex> Plane::createPlanePointsNoTranslate(const float length, const int divisions) {
    std::vector<Vertex> ans = std::vector<Vertex>();
    std::vector<Vertex> ans2 = std::vector<Vertex>();

	const float fdivisions = static_cast<float>(divisions);

    const float step = length / fdivisions;

    for(int i = 0; i <= divisions; i++) { // lines (x)
        for(int j = 0; j <= divisions; j++) { // columns (z)
            ans.emplace_back(static_cast<float>(i) * step, 0.0f, static_cast<float>(j) * step, 0.0f, 1.0f, 0.0f, 1.0f - (step * static_cast<float>(i) / length), 1.0f - (step * static_cast<float>(j) / length));
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

    return ans2;
}

