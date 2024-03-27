#ifndef CG_PATCH_H
#define CG_PATCH_H


#include <string>
#include <vector>

#include "Vertex.h"

class Patch {
public:
    static std::vector<Vertex> generatePatch(std::string& patchFilename, int tesselation);
    static std::vector<Vertex> tessellatePatches(std::vector<std::vector<Vertex>>& patches, int divisions);
    static std::vector<Vertex> dividePatch(std::vector<Vertex>& patch, int divisions);
    static Vertex interpolateVertex(Vertex& p0, Vertex& p1, Vertex& p2, Vertex& p3, float t,float u, float v);
    static Vertex interpolatePatch(std::vector<Vertex>& patch, float u, float v);

    static void writePatchesToFile(std::vector<std::vector<Vertex>>& patches, std::string& filename);
};


#endif //CG_PATCH_H
