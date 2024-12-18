#include "Vec2.h"
#include <cmath>
Vec2 Vec2::Zero = Vec2();
Vec2::Vec2() {
    this->x = 0;
    this->y = 0;
}

Vec2::Vec2(float x,float y) {
    this->x = x;
    this->y = y;
}

Vec2 Vec2::Add(Vec2 a, Vec2 b) {
    return Vec2(a.x + b.x, a.y + b.y);
}

Vec2 Vec2::Sub(Vec2 a, Vec2 b) {
    return Vec2(a.x - b.x, a.y - b.y);
}

float Vec2::Dot(Vec2 a, Vec2 b) {
    return a.x * b.x + a.y * b.y;
}

Vec2 Vec2::MultiplyByScalar(Vec2 a, int b) {
    return Vec2(a.x * b, a.y * b);
}
Vec2 Vec2::MultiplyByScalar(Vec2 a, float b) {
    return Vec2(a.x * b, a.y * b);
}
Vec2 Vec2::DivideByScalar(Vec2 a, int b) {
    return Vec2(a.x / b, a.y / b);
}
Vec2 Vec2::DivideByScalar(Vec2 a, float b) {
    return Vec2(a.x / b, a.y / b);
}
Vec2 operator+(const Vec2& a, const Vec2& b) {
    return Vec2::Add(a, b);
}

Vec2 operator-(const Vec2& a, const Vec2& b) {
    return Vec2::Sub(a, b);
}

Vec2 operator*(const Vec2& a, int b) {
    return Vec2::MultiplyByScalar(a, b);
}
Vec2 operator*(const Vec2 a, float b) {
    return Vec2::MultiplyByScalar(a, b);
}
float operator*(const Vec2& a, const Vec2& b) {
    return Vec2::Dot(a,b);
}

Vec2 operator/(const Vec2& a, int b) {
    return Vec2::DivideByScalar(a, b);
}
Vec2 operator/(const Vec2& a, float b) {
    return Vec2::DivideByScalar(a, b);
}
bool operator==(const Vec2 a, const Vec2 b) {
    return a.x == b.x && a.y == b.y;
}

bool operator!=(const Vec2 a, const Vec2 b) {
    return a.x != b.x || a.y != b.y;
}

float Vec2::magnitude() {
    return sqrt(x * x + y * y);
}

Vec2 Vec2::normalized(){
    float magnitude = this->magnitude();
    return *this/magnitude;
}

float Vec2::Distance(Vec2 a, Vec2 b) {
    return (a-b).magnitude();
}

float Vec2::inclination(){
    return atan2(y,x);
}

float Vec2::Angle(Vec2 a, Vec2 b){
    return acos((a*b)/(a.magnitude()*b.magnitude()));
}

Vec2 Vec2::Rotate(Vec2 a, float angle){
    return Vec2(a.x*cos(angle) - a.y*sin(angle), a.x*sin(angle) + a.y*cos(angle));
}
// Vec2 operator=(const Vec2& a){
//     return Vec2(a.x, a.y);
// }