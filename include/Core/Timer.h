#pragma once
#include <string>
#include <sstream>
class Timer{
    public:
        inline Timer(float amount):m_Amount(amount),m_Time(amount){};
        
        inline void Update(float dt) {m_Time += dt;};
        inline void Restart() {m_Time = 0;};
        inline bool Expired() {return m_Time >= m_Amount;};

        inline float GetTime() { return m_Time; };
        inline float GetAmount() { return m_Amount; };

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
};

inline std::string format_as(Timer& e){
    return e.ToString();
}