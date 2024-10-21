#include "Sprite.h"
#include "Music.h"
class State{
    public:
        State (); 
        bool QuitRequested ();
        void LoadAssets ();
        void Update (float dt);
        void Render ();
        void Run ();
    private: 
        Sprite* bg;
        Music music;
        bool quitRequested;
};