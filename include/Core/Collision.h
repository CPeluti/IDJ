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
				float PA[4];
				float PB[4];

				for (int i = 0; i < 4; ++i) PA[i] = Dot(A[i], axis);

				float minA = *std::min_element(PA, PA + 4);	
				float maxA = *std::max_element(PA, PA + 4);


				float min_r2 = INFINITY, max_r2 = -INFINITY;
				for (int i = 0; i < 4; ++i) PB[i] = Dot(B[i], axis);

				float minB = *std::min_element(PB, PB + 4);
				float maxB = *std::max_element(PB, PB + 4);

				if (maxA < minB || minA > maxB)
					return false;

				float o = std::min(maxA, maxB) - std::max(minA, minB);
				if (o < overlap) {
					overlap = o;
					mtvAxis = axis;
				}	
			}

			if (Dot((b.center() - a.center()), mtvAxis) < 0)
				mtvAxis = {-mtvAxis.x, -mtvAxis.y};

			float slop = 0.01f;
			float percent = 0.8f;
			float correctedOverlap = std::max(overlap - slop, 0.0f) * percent;
			Vec2 d = (mtvAxis * correctedOverlap);
			rA.box.Move(rA.box.center()-d);
			rA.getAssociated()->box.Move(rA.box.center());
			
			return true;
		}

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