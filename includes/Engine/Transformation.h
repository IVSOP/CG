#ifndef CG_TRANSFORMATION_H
#define CG_TRANSFORMATION_H

#include "Vertex.h"
#include "Consts.h"
#include "common.h"

#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <algorithm>

class Transform {
public:
    virtual glm::mat4 getMatrix(float t) = 0;
};

class Translate : public Transform {
public:
    float time, x, y, z;
    bool align, curve;
    std::vector<Vertex> curvePoints;

    std::string alignAxis;

    glm::vec3 xVector;
    glm::vec3 yVector; // Assumir y inicial
    glm::vec3 zVector;

    // Como saber qual o eixo de orientação do objeto?
    Translate(float time, bool align, float x, float y, float z, bool curve, std::vector<Vertex>& curvePoints, const char* alignAxis):
            time(time), x(x), y(y), z(z), align(align), curve(curve), curvePoints(curvePoints),
            xVector(glm::vec3(1.0f,0.0f,0.0f)), yVector(glm::vec3(0.0f,1.0f,0.0f)), zVector(glm::vec3(0.0f,0.0f,-1.0f)){
        this->alignAxis = alignAxis;

        std::transform(this->alignAxis.begin(), this->alignAxis.end(), this->alignAxis.begin(), ::tolower);

    }

    glm::mat4 getMatrix(float t) override {
        if(!this->curve) return Consts::translateMatrix(this->x, this->y, this->z);

        t = fmod(t,this->time);

        t = t / this->time;

        glm::vec4 pos;
        glm::vec4 deriv;

        // obter posição e derivadas de ponto
        getGlobalCatmullRomPoint(t, pos, deriv);

        // translação para  posição correta na curva
        glm::mat4 finalMat = Consts::translateMatrix(pos.x,pos.y,pos.z); 

        // setup para matriz de rotação
        glm::mat4 rotMatrix = glm::mat4(1.0f);

        // glm::vec3 desiredOrientation = glm::vec3(0.0f,1.0f,0.0f);

        if(this->align){

            // Align Z
            if(this->alignAxis == "z"){
                this->zVector = glm::vec3(glm::normalize(deriv));

                this->xVector = glm::normalize(glm::cross(this->zVector,this->yVector));

                this->yVector = glm::normalize(glm::cross(this->xVector, this->zVector));
            }

            // Align Y
            // Se quiseres que o teapot fique virado para cima em vez de para baixo, no construtor do zVector mete -1.0f em vez de 1.0f

            else if(this->alignAxis == "y"){
                this->yVector = glm::vec3(glm::normalize(deriv));

                this->xVector = glm::normalize(glm::cross(this->yVector, this->zVector));

                this->zVector = glm::normalize(glm::cross(this->xVector,this->yVector));
            }

            // Default
            // Align X
            else {
                this->xVector = glm::vec3(glm::normalize(deriv));

                this->zVector = glm::normalize(glm::cross(this->xVector,this->yVector));

                this->yVector = glm::normalize(glm::cross(this->zVector, this->xVector));
            }

//            this->yVector = glm::normalize(deriv);
//
//            this->zVector = glm::normalize(glm::cross(this->yVector, glm::vec3(1.0f, 0.0f, 0.0f)));
//
//            this->xVector = glm::normalize(glm::cross(this->zVector, this->yVector));

            rotMatrix = buildRotMatrix(this->xVector,this->yVector,this->zVector);
        }

        // rodar objeto para ficar alinhado com a curva
        finalMat *= rotMatrix;

        return finalMat;
    }

    // Construída para matrix column major, daí não ser transposta
    glm::mat4 buildRotMatrix(glm::vec3& x, glm::vec3& y, glm::vec3& z) {
        glm::mat4 resultMatrix = glm::mat4(1.0f);
        resultMatrix[0][0] = x.x; resultMatrix[0][1] = x.y; resultMatrix[0][2] = x.z; resultMatrix[0][3] = 0.0f;
        resultMatrix[1][0] = y.x; resultMatrix[1][1] = y.y; resultMatrix[1][2] = y.z; resultMatrix[1][3] = 0.0f;
        resultMatrix[2][0] = z.x; resultMatrix[2][1] = z.y; resultMatrix[2][2] = z.z; resultMatrix[2][3] = 0.0f;
        resultMatrix[3][0] = 0.0f; resultMatrix[3][1] = 0.0f; resultMatrix[3][2] = 0.0f; resultMatrix[3][3] = 1.0f;

        return resultMatrix;
    }
    
    void getGlobalCatmullRomPoint(float gt, glm::vec4& pos, glm::vec4& deriv) {

        GLuint pointCount = curvePoints.size();
        float t = gt * static_cast<float>(pointCount);
		GLuint index = static_cast<GLuint>(floor(t));  // which segment
        t = t - static_cast<float>(index); // where within  the segment

        GLuint indices[4];

        indices[0] = (index + pointCount-1)%pointCount;	
        indices[1] = (indices[0]+1)%pointCount;
        indices[2] = (indices[1]+1)%pointCount; 
        indices[3] = (indices[2]+1)%pointCount;

        //pontos atuais da curva a ser considerados
        std::vector<Vertex> currPoints;

        for (GLuint index : indices) {
            currPoints.emplace_back(this->curvePoints[index]);
        }

        // catmull-rom matrix
        glm::mat4 mMatrix = glm::mat4(1.0f);	
            
        // glm é column major daí fazer assim
        mMatrix[0] = glm::vec4(-0.5f, 1.0f, -0.5f, 0.0f);
        mMatrix[1] = glm::vec4(1.5f, -2.5f, 0.0f, 1.0f);
        mMatrix[2] = glm::vec4(-1.5f, 2.0f, 0.5f, 0.0f);
        mMatrix[3] = glm::vec4(0.5f, -0.5f, 0.0f, 0.0f);

        glm::vec4 tVec = glm::vec4(powf(t,3.0f), powf(t,2.0f), t, 1.0f);
        glm::vec4 tTVec = glm::vec4(3.0f * powf(t,2), 2 * t, 1.0f, 0.0f);

        glm::mat4 pMatrix; 

        //construct pMatrix from currPoints coords
        for (GLuint i = 0; i < 4; ++i) {
            pMatrix[i] = currPoints[i].coords;
        }

        // glm é column major daí fazer assim
        pMatrix = glm::transpose(pMatrix);


        // Compute A = M * P // 4*4 * 4*4 = 4*4
        glm::mat4 aMatrix = mMatrix * pMatrix;

        // Compute pos = T * A // 1*4 * 4*4 = 1*4
        pos = tVec * aMatrix;

        // compute deriv = T' * A
        deriv = tTVec * aMatrix;
    }

    std::pair<std::vector<Vertex>, std::vector<Vertex>> getCurvePoints(int tesselation_level){
        if(!this->curve) return std::pair(std::vector<Vertex>(), std::vector<Vertex>());

        float gt;

        glm::vec4 pos;
        glm::vec4 deriv;

        std::vector<Vertex> points = std::vector<Vertex>();
        std::vector<Vertex> derivates = std::vector<Vertex>();

        for(int i = 0; i < tesselation_level; i++){

            gt = float(i) / tesselation_level;

            getGlobalCatmullRomPoint(gt, pos, deriv);

            points.emplace_back(pos[0], pos[1], pos[2]);

            derivates.emplace_back(pos[0], pos[1], pos[2]);
            derivates.emplace_back(pos[0] + deriv[0], pos[1] + deriv[1], pos[2] + deriv[2]);
        }

        return std::pair(points, derivates);
    }
};

class Rotate : public Transform {
public:
    float angle, time, x, y, z;
    bool curve;

    Rotate(float angle, float time, float x, float y, float z, bool curve) : angle(angle), time(time), x(x), y(y), z(z),
                                                                             curve(curve) 
        {

            // vetor de rotate é normalizado, para n afetar o scale
            glm::vec3 normalizedAxis = glm::normalize(glm::vec3(x,y,z));
            this->x = normalizedAxis.x;
            this->y = normalizedAxis.y;
            this->z = normalizedAxis.z;
        }

    glm::mat4 getMatrix(float t) override {
        if(this->curve){
            t = fmod(t,this->time); // rever
            this->angle = 360.0f * t / this->time;
        }

        return Consts::rotMatrixVector(angle, this->x, this->y, this->z);
    }
};

class Scale : public Transform {
public:
    glm::mat4 matrix;
    int x, y, z;

    Scale(float x, float y, float z){
        this->matrix = Consts::scaleMatrix(x, y, z);
    }

    Scale(){
        this->matrix = Consts::scaleMatrix(1, 1, 1);
    }

    glm::mat4 getMatrix(float t) override {
        return this->matrix;
    }
};

struct Transformation {
public:
    glm::mat4 transformMatrix;

    Transformation()
    : transformMatrix(Consts::idMatrix()) {}
    
    explicit Transformation(Transform& transform, float t){
        this->transformMatrix = transform.getMatrix(t);
    }

    void appendTransformation(glm::mat4& transformation);

    void appendTransformation(Transform& transform, float t);

    std::vector<Vertex> apply(std::vector<Vertex>& points);
};


#endif //CG_TRANSFORMATION_H
