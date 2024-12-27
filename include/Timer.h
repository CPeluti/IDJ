#pragma once
class Timer{
    public:
        Timer(float amount);
        void Update(float dt);
        void Restart();
        bool Expired();
        float GetTime();

    private:
        float amount;
        float time;
};