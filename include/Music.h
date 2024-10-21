#define INCLUDE_SDL_MIXER
#include <string>
#include "SDL_include.h"
class Music {
    public:
        Music ();
        Music (std::string file);
        ~Music ();
        void Play (int times);
        void Stop (int ms);
        void Open (std::string file);
        bool IsOpen ();
    private:
        Mix_Music* music;
};