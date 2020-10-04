/**
**This is three vector class
**/
#include <iostream>
#include <math.h>

#ifndef HEADER_FILE
#define HEADER_FILE

using namespace std;

class Vector3{
private:
    double v[3];
public:
    Vector3(){
        this->setValue(0,0,0);
    }
    Vector3(const Vector3 &v){
        this->v[0]=v.v[0];
        this->v[1]=v.v[1];
        this->v[2]=v.v[2];
    }

    //Setting vector's component
    void setValue(double v1,double v2,double v3){
        this->v[0]=v1;
        this->v[1]=v2;
        this->v[2]=v3;
    }
    void setValue(double v[3]){
        this->v[0]=v[0];
        this->v[1]=v[1];
        this->v[2]=v[2];
    }
    void setValue(Vector3 v){
        this->v[0]=v.v[0];
        this->v[1]=v.v[1];
        this->v[2]=v.v[2];
    }

    //Getting vector's Component
    void getValue(double v[3]){
        v[0]=this->v[0];
        v[1]=this->v[1];
        v[2]=this->v[2];
    }
    double getValue1(){
        return v[0];
    }
    double getValue2(){
        return v[1];
    }
    double getValue3(){
        return v[2];
    }
    Vector3 getVector(){
        Vector3 temp;
        temp.setValue(v);
        return temp;
    }
    void operator = (const Vector3 &v ) {
        this->v[0]=v.v[0];
        this->v[1]=v.v[1];
        this->v[2]=v.v[2];
    }
    static Vector3 getVector(double v1,double v2,double v3){
        Vector3 temp;
        temp.setValue(v1,v2,v3);
        return temp;
    }
    static Vector3 getVector(double v[3]){
        Vector3 temp;
        temp.setValue(v);
        return temp;
    }
    double getMagnitude(){
        double m = sqrt(this->v[0]*this->v[0]+this->v[1]*this->v[1]+this->v[2]*this->v[2]);
        return m;
    }

    //mathematics
    Vector3 operator+(const Vector3 &v){
        Vector3 temp;
        temp.v[0]=this->v[0]+v.v[0];
        temp.v[1]=this->v[1]+v.v[1];
        temp.v[2]=this->v[2]+v.v[2];
        return temp;
    }
    Vector3 operator-(const Vector3 &v){
        Vector3 temp;
        temp.v[0]=this->v[0]-v.v[0];
        temp.v[1]=this->v[1]-v.v[1];
        temp.v[2]=this->v[2]-v.v[2];
        return temp;
    }
    Vector3 operator*(const Vector3 &v){
        Vector3 temp;
        temp.v[0]=this->v[1]*v.v[2]-this->v[2]*v.v[1];
        temp.v[1]=this->v[2]*v.v[0]-this->v[0]*v.v[2];
        temp.v[2]=this->v[0]*v.v[1]-this->v[1]*v.v[0];
        return temp;
    }
    double dot(const Vector3 &v){
        double d=0;
        d=this->v[0]*v.v[0]+this->v[1]*v.v[1]+this->v[2]*v.v[2];
        return d;
    }
    Vector3 scale(Vector3 v){
        Vector3 temp;
        temp.v[0]=this->v[0]*v.v[0];
        temp.v[1]=this->v[1]*v.v[1];
        temp.v[2]=this->v[2]*v.v[2];
        return temp;
    }
    Vector3 scale(double v){
        Vector3 temp;
        temp.v[0]=this->v[0]*v;
        temp.v[1]=this->v[1]*v;
        temp.v[2]=this->v[2]*v;
        return temp;
    }

    //Print Value
    void print(){
        cout<<"("<<v[0]<<","<<v[1]<<","<<v[2]<<","<<")"<<endl;
    }
    friend ostream& operator <<(ostream &out,Vector3 v){
         out<<"("<<v.v[0]<<","<<v.v[1]<<","<<v.v[2]<<")";
         return out;
    }
};
#endif


