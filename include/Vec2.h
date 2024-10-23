#pragma once
class Vec2{
    public:
        float x,y;
        Vec2();
        Vec2(int x,int y);
        Vec2 normalized();
        float magnitude();
        float inclination();
        static Vec2 Rotate(Vec2 a,float angle);
        static float Angle(Vec2 a,Vec2 b);
        static float Dot(Vec2 a,Vec2 b);
        static Vec2 Add(Vec2 a,Vec2 b);
        static Vec2 Sub(Vec2 a,Vec2 b);
        static Vec2 MultiplyByScalar(Vec2 a,int b);
        static Vec2 DivideByScalar(Vec2 a,int b);
        static float Distance(Vec2 a,Vec2 b);
        // Vec2 operator=(const Vec2& a);
};