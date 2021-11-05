// Michael Corona
// Assignment 12 - FINAL PROJECT
// CS 302 - Section 1003

//Compile as: g++ 2DInteractiveMap.cpp -lncurses

//#include<iostream>    //cin/cout NOT USED. DO NOT ENABLE!
#include<fstream>       //File Streams
#include<ncurses.h>     //N Curses Library
#include<stdlib.h>      //srand
#include<time.h>        //time
#include<vector>

//using namespace std;

void initCurses();
void endCurses();

class CharMap {
  public:
    CharMap(char *arg);
    CharMap(char** c, std::string m, int w, int h) :
        map(c), mapName(m), width(w), height(h){}
    ~CharMap();
    void searchMap(char,int&,int&);
    char getChr(int, int);
    void print();
    char ** map;
    std::string mapName;
    int width;
    int height;
};

class ent_t{
public:
  int x;
  int y;
  ent_t(int xx = 0, int yy = 0){
    x = xx;
    y = yy;
  }
  void setCoordinates(int xx, int yy){
    x = xx;
    y = yy;
  }
  virtual char whatAmI() = 0;
};

class bomb_t : public ent_t{
public:
  bool isPlanted;
  bool isCarried;
  bool isDefused;
  bomb_t(int xx, int yy) : ent_t(xx,yy){
    isPlanted = false;
    isCarried = false;
    isDefused = false;
  }
  char whatAmI(){
    return 'B';
  }
};

class player_t : public ent_t{
public:
  bool isHuman;
  bomb_t *bomb;
  char lastDirection;
  enum Status {dead,alive};
  Status stat;
  char Team;

  player_t(int xx, int yy, char typeOfPlayer, bool h)
    : ent_t(xx,yy){
    if (typeOfPlayer == 'T'){
      Team = 'T';
    }
    else{
      Team = 'C';
    }
    isHuman = h;
    stat = alive;
  }
  bool isAlive(){
    if (stat == alive){
      return true;
    }
    else{
      return false;
    }
  }
  char whatAmI(){
    if (isHuman){
      return '@';
    }
    else if (Team == 'T'){
      return 'T';
    }
    else{
      return 'C';
    }
  }
  void RIP(std::vector<ent_t*>& p_entList){
    for (int i = 0; i < p_entList.size(); i++){
      if (p_entList.at(i) == this){
	p_entList.at(i) = NULL;
	if (bomb != nullptr){
	  p_entList.at(i) = bomb;
	  bomb->isCarried = false;
	}
      }
    }
  }
};

class point_t{
public:
  char baseType;
  int x;
  int y;
  std::vector<ent_t*>entList;
  bool isBombSite;
  bool isObstacle;
  bool isWall;
  bool isBridgeTunnel;
  point_t(CharMap *pnt, int xx, int yy){
    baseType = pnt->getChr(xx,yy);
    x = xx;
    y = yy;
    if (baseType == 'x'){
      isWall = true;
    }
    if (baseType == '#'){
      isBridgeTunnel = true;
    }
    if (baseType == 'B'){
      baseType = ' ';
      entList.push_back(new bomb_t(x,y));
    }
    if (baseType == 'P' || baseType == '1' || baseType == '2'
	|| baseType == '3'){
      baseType = ' ';
      isBombSite = true;
    }
    if (baseType == 'C'){
      for (int i = 0; i < 5; i++){
	entList.push_back(new player_t(x,y,'C', false));
      }
      baseType = ' ';
    }
    if (baseType == 'T'){
      for (int i = 0; i < 5; i++){
	entList.push_back(new player_t(x,y,'T', false));
      }
      baseType = ' ';
    }

  }
  void renderPoint(){

    char finalType = 'P';
    finalType = baseType;
    if (isBombSite){
      finalType = 'P';
    }
    if (entList.size() != 0){
      for (int i = 0; i < entList.size(); i++){
	if (entList.at(i)->whatAmI() == 'B'){
	  finalType = 'B';
	}
      }
      for (int i = 0; i < entList.size(); i++){
	if (entList.at(i)->whatAmI() == 'C'){
	  finalType = 'C';
	}
	if (entList.at(i)->whatAmI() == 'T'){
	  finalType = 'T';
	}
      }
      for (int i = 0; i < entList.size(); i++){
        if (static_cast<player_t*>(entList.at(i))->isHuman){
	  finalType = '@';
	}
      }
      printw("%c",finalType);
    }
    else{
      printw("%c", finalType);
    }
  }
};

class Level{
public:
  point_t*** point;
  int height;
  int width;
  CharMap* mapref;
  int roundTimer;
  int bombTimer;
  bool bombPlanted;
  int Talive;
  int Calive;
  int endCondtion;
  Level(CharMap *map){

    mapref = map;
    height = map->height;
    width = map->width;

    //create 2-D array map
    point = new point_t **[height];
    for (int i = 0; i < height; i++){
      point[i] = new point_t *[width];
    }

    //initialize 2-D array map
    for (int i = 0; i < height; i++){
      for (int j = 0; j < width; j++){
	point[i][j] = new point_t(map,i,j);
      }
    }

  }

  void renderLevel(){
    for (int i = 0; i < height; i++){
      for (int j = 0; j < width; j++){
	point[i][j]->renderPoint();
      }
    }
  }

  player_t* openNteamSelect(){
    player_t *player;
    char temp;
    int xx;
    int yy;

    do{
      printw("\nChoose your team \n");
      printw("---- Press T for Terrorist Team\n");
      printw("---- Press C for Counter Terrorist Team\n");
      temp = getch();
      clear();
    }while(temp != 'C' && temp!= 'T');

    if (temp == 'C'){
      mapref->searchMap('C',xx,yy);
      player = new player_t(xx,yy,'C',true);
      point[xx][yy]->entList.pop_back();
      point[xx][yy]->entList.push_back(player);
    }
    else if (temp == 'T'){
      mapref->searchMap('T',xx,yy);
      player = new player_t(xx,yy,'T',true);
      point[xx][yy]->entList.pop_back();
      point[xx][yy]->entList.push_back(player);
    }

    clearScreen();
    renderLevel();

    return player;
  }

  void clearScreen(){
    clear();
    refresh();
  }

};

class projectile_t : public ent_t{
public:
  char direction;
  ent_t *owner;
  projectile_t(int xx, int yy) : ent_t(xx,yy){
    //direction = owner->lastDirection;
  }
  char whatAmI(){
    return '*';
  }
};

class BallisticDispatcher{
public:
  std::vector<projectile_t*> projList;
  Level* levelref;
  BallisticDispatcher(Level* lvl){
    levelref = lvl;
  }

  void addProjectile(projectile_t* proj){
    projList.push_back(proj);
  }

  void updateAll(){
  }
};

class AIDispatcher{
public:
  std::vector<player_t*> botList;
  player_t* human;
  bomb_t* bomb;
  Level* levelref;
  int totalBots;
  BallisticDispatcher* ballref;

  AIDispatcher(Level *map, BallisticDispatcher *ball){
    levelref = map;
    ballref = ball;
    int height = levelref->height;
    int width = levelref->width;
    for (int i = 0; i < height; i++){
      for (int j = 0; j < width; j++){
        for (int w = 0; w < levelref->point[i][j]->entList.size(); w++){
	  if (levelref->point[i][j]->entList.at(w)->whatAmI() == 'C'){

	    addBot(static_cast<player_t*>(levelref->point[i][j]->entList.at(w)));

	  }
	  if (levelref->point[i][j]->entList.at(w)->whatAmI() == 'T'){

	    addBot(static_cast<player_t*>(levelref->point[i][j]->entList.at(w)));

	  }
	  if (static_cast<player_t*>(levelref->point[i][j]->entList.at(w))->isHuman == true){
	    addHuman(static_cast<player_t*>(levelref->point[i][j]->entList.at(w)));
	  }
	  if (static_cast<bomb_t*>(levelref->point[i][j]->entList.at(w))->whatAmI() == 'B'){
	    addBomb(static_cast<bomb_t*>(levelref->point[i][j]->entList.at(w)));
	  }
	}
      }
    }
  }
  void addHuman(player_t* h){
    human = h;
  }
  void addBot(player_t* bot){
    botList.push_back(bot);
  }
  void addBomb(bomb_t* b){
    bomb = b;
  }

  void printStatus(){
  }

  void checkForNewDead(){
  }

  void updateAll(){
  }

};

class MovementDispatcher{
public:
  static char readkeyinput(){

    int ch = getch();

    switch(ch){
    case 'W':
    case 'w':
    case KEY_UP:
      return 'U';
      break;
    case 'A':
    case 'a':
    case KEY_LEFT:
      return 'L';
      break;
    case 'S':
    case 's':
    case KEY_DOWN:
      return 'D';
      break;
    case 'D':
    case 'd':
    case KEY_RIGHT:
      return 'R';
      break;
    case 'Q':
    case 'q':
      return 'Q';
      break;
    case 'C':
    case 'c':
      return 'C';
      break;
    Default:
      return 'I';
    }
    return 0;
  }

  static void makeMove(Level* lvl, player_t* p, char direction,
		       BallisticDispatcher* ball){
    int xx = p->x;
    int yy = p->y;
    point_t *to = lvl->point[xx][yy];
    if (lvl->point[xx][yy]->isBridgeTunnel){
      if ((direction == 'U') || (direction == 'D')){
	if ((p->lastDirection != 'U') && (p->lastDirection != 'D')){
	  return;
	}
      }
      else if ((direction == 'R') || (direction == 'L')){
	if ((p->lastDirection != 'R') && (p->lastDirection != 'L')){
	  return;
	}
      }
    }
    if (direction == 'Q'){
      return;
    }
    else{
      if (direction == 'U'){
	if ((p->isAlive()) && !(lvl->point[--xx][yy]->isWall)
	    && !(lvl->point[--xx][yy]->isObstacle)){
	  xx = p->x;
	  yy = p->y;
	  for (int i = 0; i < lvl->point[xx][yy]->entList.size(); i++){
	    if (static_cast<player_t*>(lvl->point[xx][yy]->entList.at(i))->isHuman){
	      to->entList.erase(to->entList.begin()+i);
	    }
	  }

	  xx = xx-1;
	  lvl->point[xx][yy]->entList.push_back(p);
	  p->setCoordinates(xx,yy);
	  p->lastDirection = 'U';

	}
      }
      else if (direction == 'L'){
        if ((p->isAlive()) && !(lvl->point[xx][--yy]->isWall)
	  && !(lvl->point[xx][--yy]->isObstacle)){
	  xx = p->x;
	  yy = p->y;
	  for (int i = 0; i < lvl->point[xx][yy]->entList.size(); i++){
	    if (static_cast<player_t*>(lvl->point[xx][yy]->entList.at(i))->isHuman){
	      to->entList.erase(to->entList.begin()+i);

	    }
	  }

	  yy = yy-1;
	  lvl->point[xx][yy]->entList.push_back(p);
	  p->setCoordinates(xx,yy);
	  p->lastDirection = 'L';
        }
      }
      else if (direction == 'D'){
	if ((p->isAlive()) && !(lvl->point[++xx][yy]->isWall)
	  && !(lvl->point[++xx][yy]->isObstacle)){
	  xx = p->x;
	  yy = p->y;
	  for (int i = 0; i < lvl->point[xx][yy]->entList.size(); i++){
	    if (static_cast<player_t*>(lvl->point[xx][yy]->entList.at(i))->isHuman){
	      to->entList.erase(to->entList.begin()+i);

	    }
	  }

	  xx = xx+1;
	  lvl->point[xx][yy]->entList.push_back(p);
	  p->setCoordinates(xx,yy);
	  p->lastDirection = 'D';
        }
      }
      else if (direction == 'R'){
	if ((p->isAlive()) && !(lvl->point[xx][++yy]->isWall)
	  && !(lvl->point[xx][++yy]->isObstacle)){
	  xx = p->x;
	  yy = p->y;
	  for (int i = 0; i < lvl->point[xx][yy]->entList.size(); i++){
	    if (static_cast<player_t*>(lvl->point[xx][yy]->entList.at(i))->isHuman){
	      to->entList.erase(to->entList.begin()+i);

	    }
	  }

	  yy = yy+1;
	  lvl->point[xx][yy]->entList.push_back(p);
	  p->setCoordinates(xx,yy);
	  p->lastDirection = 'R';
        }
      }
      else if (direction == 'C'){
	//clear screen
	lvl->clearScreen();
      }
      else{
	//idle
      }
    }

  }

};

int main(int argc, char **argv){
//	srand(time(NULL)); //Comment out to always have the same RNG for debugging
    CharMap *map = (argc == 2) ? new CharMap(argv[1]) : NULL; //Read in input file
    if(map == NULL){printf("Invalid Map File\n"); return 1;}  //close if no file given
    initCurses(); // Curses Initialisations
    //map->print();
    //Your Code Here
    Level *dust2 = new Level(map);
    //dust2->renderLevel();
    player_t* player1 = dust2->openNteamSelect();
    BallisticDispatcher* ballistics = new BallisticDispatcher(dust2);
    AIDispatcher* AI = new AIDispatcher(dust2, ballistics);

    //MAIN LOOP
    char ch;
    while ((ch = MovementDispatcher::readkeyinput()) != 'Q'){

      MovementDispatcher::makeMove(dust2,player1,ch,ballistics);

      dust2->clearScreen();
      dust2->renderLevel();

    }

    delete map; map = NULL;
    delete dust2, dust2 = NULL;
    printw("\ngg ez\n");
    endCurses(); //END CURSES
    return 0;
}

void initCurses(){
    // Curses Initialisations
	initscr();
	raw();
	keypad(stdscr, TRUE);
	noecho();
	printw("Welcome - Press Q to Exit\n");
}
void endCurses(){
	refresh();
	getch(); //Make user press any key to close
	endwin();
}


//CharMap Functions
CharMap::CharMap(char *arg){
    char temp;
    std::ifstream fin(arg);
    fin >> mapName;
    fin >> height;
    fin >> temp;
    fin >> width;
    map = new char*[height]; //Allocate our 2D array
    for(int i=0; i<height; i++){
        map[i] = new char[width];
        for(int j=0; j<width; j++) //Read into our array while we're at it!
            fin >> (map[i][j]) >> std::noskipws; //dont skip whitespace
        fin >> std::skipws; //skip it again just so we skip linefeed
    }
    //for(int i=0; i<height; i++){ //Uncomment if you have issues reading
    //    for(int j=0; j<width; j++) printf("%c", map[i][j]); printf("\n");};
}

void CharMap::searchMap(char item, int& x, int& y){
  for (int i = 0; i < height; i++){
    for (int j = 0; j < width; j++){
      if (item == map[i][j]){
	x = i;
	y = j;
	return;
      }
    }
  }
}

char CharMap::getChr(int x, int y){
  return map[x][y];
}

CharMap::~CharMap(){
    if(map == NULL) return;
    for(int i=0; i<height; i++)
        delete [] map[i];
    delete [] map;
}

void CharMap::print(){ //call only after curses is initialized!
    printw("Read Map: '%s' with dimensions %dx%d!\n",
            mapName.c_str(), height, width);
    //Note the c_str is required for C++ Strings to print with printw
    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++)
            printw("%c", map[i][j]);
        printw("\n");
    }
}
