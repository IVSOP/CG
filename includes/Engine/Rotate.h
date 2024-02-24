#ifndef CG_ROTATE_H
#define CG_ROTATE_H


struct Rotate {
public:
    float angle;
    int x, y, z;

    Rotate(float angle, int x, int y, int z){
        this->angle = angle;
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Rotate(){
        this->angle = 0;
        this->x = 0;
        this->y = 0;
        this->z = 0;
    }


};


#endif //CG_ROTATE_H
