#include "SFApp.h"
#include "sound.h"
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_ttf.h>
#include "SFEvent.h"
#include <iostream>
#include <fstream>
#include <string>
#include "highscore.h"




//sound functions for laser shot/coin collection from sound.h
void theScore(int highScore); // calling from the highscore.cpp
void laser();//Laser sound
void coin();//Coin collection sound.
void GetLastCode();
//Score for total aliens killed.
 int totalKilled = 0; // holds players current score to be displayed on screen



SFApp::SFApp() : fire(0), is_running(true) {
  
float alienPos = 500; // starting alien position

  surface = SDL_GetVideoSurface();
  app_box = make_shared<SFBoundingBox>(Vector2(surface->w/2, surface->h/2), surface->w/2, surface->h/2);
  player  = make_shared<SFAsset>(SFASSET_PLAYER);
  auto player_pos = Point2(surface->w/2, 100.0f);
  player->SetPosition(player_pos);

//Code for pacing aliens of opposite sides of the screen////
  const int number_of_aliens = 8; // new number of aliens i picked

  int row = 1; // To place the aliens on the map i will check where the
  int y = 150; // where the alien will be placed
  int width = 0;
  for(int i=0; i<number_of_aliens; i++) {
	if(row % 2 == 1){//using modulus to get remainder
	 alienPos = y; // set current position to y, if row is odd number
	 width = 50;//reset width
	} 
        else if( row % 2 == 0){// set position of alien to , if row is even number
	 alienPos = y;
	 width = 600;//set width to 600 if the row is an even number
           }
    auto alien = make_shared<SFAsset>(SFASSET_ALIEN);
    auto pos   = Point2(width, alienPos);
    alien->SetPosition(pos);
    aliens.push_back(alien);
  row++;//adding to row for next check of odd or even
  y = y + 70;//add to y to position the next row

  }

const int COIN = 10;

// Collectable coin's code
srand(time(0));// call to randomise spawn
for(int c = 0; c < COIN; c++){

int coinWidth = rand()% 500;//random spawn for coin, selects random number between 0 and 400
int coinHeight =rand()% 351 +100;//random spawn for coin, between 1 and 350 then adds 100 to ensure not on static barriers.
  auto coin = make_shared<SFAsset>(SFASSET_COIN);
  auto pos  = Point2(coinWidth, coinHeight);
  coin->SetPosition(pos);
  coins.push_back(coin);
}


const int numberHouse = 2; // variable for 2 static barriers on the screen
int blockWidth = 100; // width on block on screen
int blockHeight= 100; // height of block on screen
for(int j=0; j<numberHouse;j++){

auto house1 = make_shared<SFAsset>(SFASSET_HOUSE);
auto pos = Point2(blockWidth,blockHeight);
house1->SetPosition(pos);
house.push_back(house1);
blockWidth = blockWidth + 400;} //space the blocks out
}



SFApp::~SFApp() {
}

/**
 * Handle all events that come from SDL.
 * These are timer or keyboard events.
 */
int lastCode;
int up = 1;
int west = 2;
int east = 3;
int south = 4;
void SFApp::OnEvent(SFEvent& event) {
  SFEVENT the_event = event.GetCode();
  switch (the_event) {
  case SFEVENT_QUIT:
    is_running = false;
    break;
  case SFEVENT_UPDATE:
    OnUpdateWorld();
    OnRender();
    break;
  case SFEVENT_PLAYER_LEFT:
    player->GoWestFast();
    lastCode = west;
    break;
  case SFEVENT_PLAYER_RIGHT:
    player->GoEastFast();
    lastCode = east;
    break;
  case SFEVENT_PLAYER_UP:
    lastCode = up;
    player->GoNorthFast();
    break; 
   case SFEVENT_PLAYER_DOWN:
    lastCode = south;
    player->GoSouthFast();
    break;
  }
}

int SFApp::OnExecute() {
  // Execute the app

  SDL_Event event;
  while (SDL_WaitEvent(&event) && is_running) {

    // if this is an update event, then handle it in SFApp,
    // otherwise punt it to the SFEventDispacher.
    SFEvent sfevent((const SDL_Event) event);
    OnEvent(sfevent);
  }
}


void SFApp::OnUpdateWorld() {
  // Update projectile positions
  for(auto p: projectiles) {
   p->GoNorthFast();
   }
  for(auto c: coins) {
   // c->GoSouth();
  }
  // Update enemy positions

bool isEven = true;//Bool to check if row is even for moving the aliens left or right.

// way of moving aliens east and west of the screen by checking if even or odd.
for(auto a : aliens) {
     isEven = !isEven; // set even to the opposite, AKA false
	    if(isEven){ // if the row is even the go west
	     a->GoWestSlow();
	   } 
	    else {//else if it is not even then aliens must go east
	     a->GoEastSlow();
	}
 }

for(auto c : coins){
  if(c->CollidesWith(player)){
   c->HandleCollision();//if collisionbetween player and coins this will happen
   totalKilled = totalKilled + 100;//adding to the main high score
  coin();//add coin sound here///
 }
}




   //Blockades 
for(auto h : house){
 if(h->CollidesWith(player))
{
  switch(lastCode){
  case 1:
  player->GoSouthFast();
  break;
  case 2:
  player->GoEastFast();
  break;
  case 3:
  player->GoWestFast();
  break;
  case 4:
  player->GoNorthFast();
  break;

}
 }    
  }

       for(auto a : aliens) {
         int hitCount = 0;
           for(auto p : projectiles ) {
                     if(p->CollidesWith(a)) {
                        p->HandleCollision();
			a->HandleCollision();
                        totalKilled = totalKilled + 100;//Adding to the high score
                      }
 		}
       }  
       
    
//Collision between Barriers/Aliens if true game is ended !
 



if(totalKilled == 1000){ // if player has reached the max score of 1000 then highscore feature enabled to end game
theScore(totalKilled);


}




for( auto a : aliens){
	if(player->CollidesWith(a)){
        cout << "*******************************************************"<< endl;
	cout << "Sorry you have died please play again your score was: " << totalKilled << endl;
	theScore(totalKilled);
	exit(0);//exit the game once the endgame message/high score has been shown.
 }
}

// added to clear the coins of the screen once collision

list<shared_ptr<SFAsset>> tempC;
for(auto c : coins){
 if(c->IsAlive()){
 tempC.push_back(c);
}
}
coins.clear();
coins = list<shared_ptr<SFAsset>>(tempC);




  // remove dead aliens (the long way)
  list<shared_ptr<SFAsset>> tmp;
  for(auto a : aliens) {
    if(a->IsAlive()) {
      tmp.push_back(a);
    }
  }
  aliens.clear();
  aliens = list<shared_ptr<SFAsset>>(tmp);




}

bool initTTF()
{
    
    //Initialize SDL_ttf
    if( TTF_Init() == -1 )
    {
        return false;    
    }
    
 
    
    //If everything initialized fine
    return true;
}


SDL_Surface *message = NULL;
SDL_Surface *image; 
SDL_Surface *temp;


void SFApp::OnRender() {
SDL_Surface* background = NULL;
background = SDL_LoadBMP("assets/background.bmp"); // bring in the 
SDL_BlitSurface(background,NULL, surface, NULL);



SDL_Color backgroundColor = { 0, 0, 0 }; // background color for the TTF on screen
temp = SDL_LoadBMP("assets/background.bmp"); // load the temp image on surface
image = SDL_DisplayFormat(temp);
SDL_Rect textLocation = { 0, 0, 0, 0 }; // position of the TTF displayed on screen.
TTF_Font* font = TTF_OpenFont("assets/FreeSans.ttf", 20); // font that was used for the text
SDL_Color foregroundColor = {255,255, 255};

std::stringstream totalScore;
totalScore << "Score: " << totalKilled; // display for the total score using string stream to create a string from varible + string



SDL_Surface* textSurface = TTF_RenderText_Shaded(font, totalScore.str().c_str() , foregroundColor, backgroundColor);
SDL_BlitSurface(textSurface, NULL, surface, &textLocation); // load the surface on the screen
SDL_FreeSurface(temp);


SDL_FreeSurface(textSurface); //free the surface
 TTF_CloseFont(font); // close the fonts


    player->OnRender(surface);
    





  for(auto p: projectiles) {
    if(p->IsAlive()) {p->OnRender(surface);}
  }



  for(auto a: aliens) {
    if(a->IsAlive()) {a->OnRender(surface);}
  }


  for(auto c: coins) {
    if(c->IsAlive()) {c->OnRender(surface);}
  }

  for(auto h: house){
  if(h->IsAlive()) {h->OnRender(surface);}// added for the static barrier
}

  // Switch the off-screen buffer to be on-screen
  SDL_Flip(surface);
}

