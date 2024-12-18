class Timer{
    public:
        Timer(float amount);
        void Update(float dt);
        void Restart();
        bool Expired();

    private:
        float amount;
        float time;
};