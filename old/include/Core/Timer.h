#pragma once
#include <string>
#include <sstream>
class Timer{
    public:
        inline Timer(float amount):m_Amount(amount),m_Time(amount){};
        inline Timer(): m_Amount(0), m_Time (0) {}

        inline void Update(float dt) {
            bool already = Expired();
            m_Time += dt;
            if( Expired() && !already) {
                m_justExpired = true;
            } else if(Expired() && already) {
                m_justExpired = false;
			}
        };
        inline void Restart() {m_Time = 0;};
        inline bool Expired() {return m_Time >= m_Amount;};

        inline bool JustExpired() {
            return m_justExpired;
        }

        inline float GetTime() const{ return m_Time; };
        inline float GetAmount() const{ return m_Amount; };

        inline void SetTime(float time) { m_Time = time; };
        inline void SetAmount(float amount) { m_Amount = amount; };

        inline std::string ToString() {
            std::stringstream ss;
            ss << "Target_Time: " << m_Amount << " | " << "Current_Time: " << m_Time;
            return ss.str();
        }
    private:
        float m_Amount;
        float m_Time;
		bool m_justExpired = false;
};

inline std::string format_as(Timer& e){
    return e.ToString();
}