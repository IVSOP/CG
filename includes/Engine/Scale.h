#ifndef CG_SCALE_H
#define CG_SCALE_H


struct Scale {
    int x, y, z;

    Scale(int x, int y, int z){
        this->x = x;
        this->y = y;
        this->z = z;
    }

    Scale(){
        this->x = 0;
        this->y = 0;
        this->z = 0;
    }
};


#endif //CG_SCALE_H
