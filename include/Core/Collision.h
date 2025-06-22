#pragma once
#include "Core/Rect.h"
#include "Core/Vec2.h"

#include "Core/Log.h"

#include <algorithm>
#include <cmath>

struct vec2d
{
	float x;
	float y;
};


class polygon
{
	public:
		std::string tag;
		std::vector<vec2d> p;	// Transformed Points
		vec2d pos;				// Position of shape
		float angle;			// Direction of shape
		std::vector<vec2d> o;	// "Model" of shape							
		bool overlap = false;	// Flag to indicate if overlap has occurred
};

class Collision {

	public:
		// Observação: IsColliding espera ângulos em radianos!
		// Para usar graus, forneça a sua própria implementação de Rotate,
		// ou transforme os ângulos no corpo de IsColliding.
		static inline bool IsColliding(Collider& rA, Collider& rB, Vec2 offsetA={0,0}, Vec2 offsetB={0,0}) {
			double angleOfA = rA.getAngleDeg();
			double angleOfB = rB.getAngleDeg();
			float overlap = INFINITY;
			Vec2  mtvAxis{};  
			Rect a = Rect::AddVector(rA.box,offsetA);
			Rect b = Rect::AddVector(rB.box,offsetB);
			
			Vec2 A[] = { Vec2( a.GetPos().x, a.GetPos().y + a.GetSize().y ),
						  Vec2( a.GetPos().x + a.GetSize().x, a.GetPos().y + a.GetSize().y ),
						  Vec2( a.GetPos().x + a.GetSize().x, a.GetPos().y ),
						  Vec2( a.GetPos().x, a.GetPos().y )
						};
			Vec2 B[] = { Vec2( b.GetPos().x, b.GetPos().y + b.GetSize().y ),
						  Vec2( b.GetPos().x + b.GetSize().x, b.GetPos().y + b.GetSize().y ),
						  Vec2( b.GetPos().x + b.GetSize().x, b.GetPos().y ),
						  Vec2( b.GetPos().x, b.GetPos().y )
						};

			for (auto& v : A) {
				v = Vec2::Rotate(v - a.center(), angleOfA) + a.center();
			}

			for (auto& v : B) {
				v = Vec2::Rotate(v - b.center(), angleOfB) + b.center();
			}

			Vec2 axes[] = { Norm(A[0] - A[1]), Norm(A[1] - A[2]), Norm(B[0] - B[1]), Norm(B[1] - B[2]) };

			for (auto& axis : axes) {
				float P[4];


				for (int i = 0; i < 4; ++i) P[i] = Dot(A[i], axis);

				float minA = *std::min_element(P, P + 4);
				float maxA = *std::max_element(P, P + 4);


				float min_r2 = INFINITY, max_r2 = -INFINITY;
				for (int i = 0; i < 4; ++i) P[i] = Dot(B[i], axis);

				float minB = *std::min_element(P, P + 4);
				float maxB = *std::max_element(P, P + 4);

				if (maxA < minB || minA > maxB)
					return false;

				float o = std::min(maxA, maxB) - std::max(minA, minB);
				if (o < overlap) {                    // guardo o MENOR overlap
					overlap = o;
					mtvAxis = axis;
				}	
			}

			if (Dot(rB.box.center() - rA.box.center(), mtvAxis) < 0)
				mtvAxis = {-mtvAxis.x, -mtvAxis.y};

			float slop = 0.01f;
			float percent = 0.8f;
			float correctedOverlap = std::max(overlap - slop, 0.0f) * percent;
			Vec2 d = (mtvAxis * overlap);
			LOG_INFO("overlap: {}",overlap);
			LOG_INFO("ra: {}",rA.box.center());
			LOG_INFO("rB: {}",rB.box.center());
			LOG_INFO("d: {}",d);
			// float s = std::sqrtf(d.x*d.x+d.y*d.y);
			rA.box.Move(rA.box.center()-d);
			rA.getAssociated()->box.Move(rA.box.center());
			
			return true;
		}


		// static inline bool ShapeOverlap_SAT_STATIC(Collider &c1, Collider &c2)
		// {
		// 	if(c1.GetTag() == "entity" && c2.GetTag() == "phys12" || c2.GetTag() == "entity" && c1.GetTag() == "phys12"){
		// 		LOG_INFO("phys12");
		// 	}

		// 	float fTheta = 3.14159f * 2.0f / 5.0f;
		// 	polygon s1;
		// 	s1.pos = {(float)c1.box.center().x, (float)c1.box.center().y};
		// 	s1.angle = c1.getAngleDeg();
		// 	s1.o.push_back({ (float)-c1.box.GetSize().x/2, (float)-c1.box.GetSize().y/2 });
		// 	s1.o.push_back({ (float)-c1.box.GetSize().x/2, (float)+c1.box.GetSize().y/2 });
		// 	s1.o.push_back({ (float)+c1.box.GetSize().x/2, (float)+c1.box.GetSize().y/2 });
		// 	s1.o.push_back({ (float)+c1.box.GetSize().x/2, (float)-c1.box.GetSize().y/2 });

		// 	s1.p.resize(4);
		// 	for (int i = 0; i < s1.o.size(); i++)
		// 		s1.p[i] =
		// 		{	// 2D Rotation Transform + 2D Translation
		// 			(s1.o[i].x * cosf(s1.angle)) - (s1.o[i].y * sinf(s1.angle)) + s1.pos.x,
		// 			(s1.o[i].x * sinf(s1.angle)) + (s1.o[i].y * cosf(s1.angle)) + s1.pos.y,
		// 		};

			
		// 	s1.tag = c1.GetTag();

		// 	polygon s2;
		// 	s2.pos = {(float)c2.box.center().x, (float)c2.box.center().y};;
		// 	s2.angle = c2.getAngleDeg();
		// 	s2.o.push_back({ (float)-c2.box.GetSize().x/2, (float)-c2.box.GetSize().y/2 });
		// 	s2.o.push_back({ (float)-c2.box.GetSize().x/2, (float)+c2.box.GetSize().y/2 });
		// 	s2.o.push_back({ (float)+c2.box.GetSize().x/2, (float)+c2.box.GetSize().y/2 });
		// 	s2.o.push_back({ (float)+c2.box.GetSize().x/2, (float)-c2.box.GetSize().y/2 });
			
		// 	s2.p.resize(4);
		// 	for (int i = 0; i < s2.o.size(); i++)
		// 		s2.p[i] =
		// 		{	// 2D Rotation Transform + 2D Translation
		// 			(s2.o[i].x * cosf(s2.angle)) - (s2.o[i].y * sinf(s2.angle)) + s2.pos.x,
		// 			(s2.o[i].x * sinf(s2.angle)) + (s2.o[i].y * cosf(s2.angle)) + s2.pos.y,
		// 		};
		// 	s2.tag = c2.GetTag();
			
		// 	polygon *poly1 = &s1;
		// 	polygon *poly2 = &s2;

		// 	float overlap = INFINITY;
			
		// 	for (int shape = 0; shape < 2; shape++)
		// 	{
		// 		if (shape == 1)
		// 		{
		// 			poly1 = &s2;
		// 			poly2 = &s1;
		// 		}
		// 		for (int a = 0; a < poly1->p.size(); a++)
		// 		{
		// 			int b = (a + 1) % poly1->p.size();
		// 			vec2d axisProj = { -(poly1->p[b].y - poly1->p[a].y), poly1->p[b].x - poly1->p[a].x };
					
		// 			// Optional normalisation of projection axis enhances stability slightly
		// 			//float d = sqrtf(axisProj.x * axisProj.x + axisProj.y * axisProj.y);
		// 			//axisProj = { axisProj.x / d, axisProj.y / d };

		// 			// Work out min and max 1D points for r1
		// 			float min_r1 = INFINITY, max_r1 = -INFINITY;
		// 			for (int p = 0; p < poly1->p.size(); p++)
		// 			{
		// 				float q = (poly1->p[p].x * axisProj.x + poly1->p[p].y * axisProj.y);
		// 				min_r1 = std::min(min_r1, q);
		// 				max_r1 = std::max(max_r1, q);
		// 			}

		// 			// Work out min and max 1D points for r2
	
		// 			float min_r2 = INFINITY, max_r2 = -INFINITY;
		// 			for (int p = 0; p < poly2->p.size(); p++)
		// 			{
		// 				float q = (poly2->p[p].x * axisProj.x + poly2->p[p].y * axisProj.y);
		// 				min_r2 = std::min(min_r2, q);
		// 				max_r2 = std::max(max_r2, q);
		// 			}

		// 			// Calculate actual overlap along projected axis, and store the minimum
		// 			overlap = std::min(std::min(max_r1, max_r2) - std::max(min_r1, min_r2), overlap);
		// 			if (!(max_r2 >= min_r1 && max_r1 >= min_r2)){
		// 				return false;
		// 			}
		// 		}
		// 	}

		// 	// If we got here, the objects have collided, we will displace r1
		// 	// by overlap along the vector between the two object centers
		// 	LOG_INFO("current: x:{} y:{}", s1.pos.x, s1.pos.y);
		// 	vec2d d = { s2.pos.x - s1.pos.x, s2.pos.y - s1.pos.y };
		// 	float s = sqrtf(d.x*d.x + d.y*d.y);

		// 	s1.pos.x -= overlap * d.x / s;
		// 	s1.pos.y -= overlap * d.y / s;
		// 	LOG_INFO("overlap:{} ", overlap);
		// 	c1.getAssociated()->box.Move({s1.pos.x, s1.pos.y});
		// 	LOG_INFO("newpos: x:{} y:{}", s1.pos.x, s1.pos.y);
		// 	return true;
		// }

	private:

		static inline float Mag(const Vec2& p) {
			return std::sqrt(p.x * p.x + p.y * p.y);
		}

		static inline Vec2 Norm(const Vec2& p) {
			return p * (1.f / Mag(p));
		}

		static inline float Dot(const Vec2& a, const Vec2& b) {
			return a.x * b.x + a.y * b.y;
		}
};

// Aqui estão três operadores que sua classe Vec2 deve precisar, se já não tiver. 
// Se sua classe tiver métodos para Mag, Norm, Dot e/ou Rotate, você pode substituir
// os usos desses métodos por usos dos seus, mas garanta que deem resultados corretos.

// Vec2 operator+(const Vec2& rhs) const {
//    return Vec2(x + rhs.x, y + rhs.y);
// }

// Vec2 operator-(const Vec2& rhs) const {
//    return Vec2(x - rhs.x, y - rhs.y);
// }

// Vec2 operator*(const float rhs) const {
//    return Vec2(x * rhs, y * rhs);
// }

inline std::string format_as(const polygon &c)
{
    return fmt::format("{}: x:{} y:{}", c.tag, c.pos.x, c.pos.y);
}