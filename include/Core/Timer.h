#pragma once
class Timer{
    public:
        Timer(float amount);
        void Update(float dt);
        void Restart();
        bool Expired();
        float GetTime();
        float GetAmount();

    private:
        float amount;
        float time;
};