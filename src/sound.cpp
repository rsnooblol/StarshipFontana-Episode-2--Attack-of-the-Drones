#include <SDL/SDL.h>  // Pull in the SDL definitions
#include <SDL/SDL_mixer.h>
#include "sound.h"
using namespace std;
sound::sound()
{
  /// open the sound for the file
  Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT,2,4096);   
  //Stores the music in a Mix varible
  Mix_Music* music;
  //loading the music to the game
  music=Mix_LoadMUS("assets/music.wav");
  //loops the music
  Mix_PlayMusic(music, -1);

}


void laser()
{

    Mix_Chunk* shot; // where to store the loaded sound file
    shot = Mix_LoadWAV("assets/shot.WAV");//brings in the sound file
    Mix_PlayChannel( -1, shot, 0);//play sound

}


void coin()
{
    Mix_Chunk* coin;
    coin = Mix_LoadWAV("assets/coin.wav");
    Mix_PlayChannel( -1, coin, 0);
}


void clearAudio()
{
   Mix_CloseAudio();//Close all the of th audio componeents open
}

