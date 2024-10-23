#include "GameObject.h"
#include "Animation.h"
#include "Sprite.h"
class SpriteRenderer {
    public:
        SpriteRenderer(GameObject& associated);
        SpriteRenderer(GameObject& associated, std::string file, int frameCountW = 1, int frameCountH = 1);
        void Open(std::string file);
        void SetFrameCount(int frameCountW, int frameCountH);
        void Update(float dt);
        void Render();
        bool Is(std::string type);
        void SetAnimation(Animation* animation);
    private:
        Sprite sprite;
        int frameStart;
        int frameEnd;
        float frameTime;
        int currentFrame;
        float timeElapsed;
};