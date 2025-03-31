#pragma once
#define _USE_MATH_DEFINES

class Vec2{
    public:
        static Vec2 Zero;
        float x,y;
        Vec2();
        Vec2(float x,float y);
        Vec2(int x,int y);
        Vec2 normalized();
        float magnitude();
        float inclination();
        //receive angle in degrees
        static Vec2 Rotate(Vec2 a,float angle);
        //return angle in degrees
        static float Angle(Vec2 a,Vec2 b);
        static float Dot(Vec2 a,Vec2 b);
        static Vec2 Add(Vec2 a,Vec2 b);
        static Vec2 Sub(Vec2 a,Vec2 b);
        static Vec2 MultiplyByScalar(Vec2 a,int b);
        static Vec2 MultiplyByScalar(Vec2 a,float b);
        static Vec2 MultiplyByScalar(Vec2 a,double b);
        static Vec2 DivideByScalar(Vec2 a,int b);
        static Vec2 DivideByScalar(Vec2 a,float b);
        static float Distance(Vec2 a,Vec2 b);
        static float lerp(float a,float b, float current);
};


Vec2 operator+(const Vec2& a, const Vec2& b);
Vec2 operator-(const Vec2& a, const Vec2& b);
Vec2 operator*(const Vec2& a, int b);
Vec2 operator*(const Vec2 a, float b);
Vec2 operator*(const Vec2 a, double b);
float operator*(const Vec2& a, const Vec2& b);
Vec2 operator/(const Vec2& a, int b);
Vec2 operator/(const Vec2& a, float b);
bool operator==(const Vec2 a, const Vec2 b);
bool operator!=(const Vec2 a, const Vec2 b);