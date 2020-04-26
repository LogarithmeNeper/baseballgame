/*************************************************************************
Baseball game
-------------------
begin                : 26 April 2020
copyright            : (C) 2019 by Pierre Sibut-Bourde
e-mail               : pierre.sibut-bourde@insa-lyon.fr, pierre.sibut.bourde@gmail.com
*************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

// define color
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

// Structures
typedef struct Innings
{
  unsigned int occupiedBases[3];
  unsigned int currentOuts;
} Inning;

typedef struct Boards
{
  unsigned int currentInning;
  unsigned int currentHalfInning; // 0 is top, 1 is bottom
  unsigned int currentScore[2];
  Inning currentSituation;
} Board;

// Functions
Board initializeBoard(Board _board); // initializes the scoreboard
Inning initializeInning(Inning _inning); // initializes the current inning

void printBoard(Board _board); // prints the current scoreboard
void printInning(Inning _inning); // prints the current situation of the current inning

Board switchHalfInning(Board _board); // switches half-inning

Board rules(Board _board, unsigned int _dice1, unsigned int _dice2); // step, returns correct board

Board advance(Board _board, unsigned int numberOfBases); // 1<=numberOfBases<=4 (1:single, ..., 4:HR)
Board throwOut(Board _board, unsigned int numberOfOuts); // throw numberOfOuts people out
Board walk(Board _board); // simulates walk
Board doublePlay(Board _board); // simulates double play
Board sacFly(Board _board); // simulates sacrifice fly

Board updateScore(Board _board, unsigned int scoredRuns); // updates the score for the team at-bat
unsigned int howManyOnBase(Board _board); // number of occupied Bases
unsigned int isInningDone(Board _board);
unsigned int throwDice(); // Randomizer %6 +1

// Main programm
int main()
{
  srand(time(NULL)); // initialize seed for randomizer

  Board scoreboard;
  scoreboard=initializeBoard(scoreboard); // initialize the scoreboard
  unsigned int dice1;
  unsigned int dice2;

  unsigned int i;
  for(i=0; i<18; ++i)
  {
    while(!isInningDone(scoreboard))
    {
      dice1=throwDice();
      dice2=throwDice();
      printf("Dices : %d, %d\r\n", dice1, dice2);
      scoreboard=rules(scoreboard,dice1,dice2);
      sleep(1);
    }
    printBoard(scoreboard);

    sleep(3);
    scoreboard=switchHalfInning(scoreboard);
  }

  return 0;
}

// Realisation of Functions
Board initializeBoard(Board _board)
{
  _board.currentInning=1; // starts first inning
  _board.currentHalfInning=0; // starts top
  _board.currentScore[0]=0;
  _board.currentScore[1]=0;
  _board.currentSituation=initializeInning(_board.currentSituation);
  return _board;
}

Inning initializeInning(Inning _inning)
{
  _inning.currentOuts=0; // no out
  _inning.occupiedBases[0]=0;
  _inning.occupiedBases[1]=0;
  _inning.occupiedBases[2]=0;
  return _inning;
}

void printBoard(Board _board)
{
  printf("Current inning : ");
  _board.currentHalfInning ? printf("Bottom of the %d\r\n",_board.currentInning) : printf("Top of the %d\r\n",_board.currentInning);
  printf("Top team :    %d\r\n",_board.currentScore[0]);
  printf("Bottom team : %d\r\n",_board.currentScore[1]);
  printf("\r\n");
  printInning(_board.currentSituation);
}

void printInning(Inning _inning)
{
  printf("Current outs : %d\r\n",_inning.currentOuts);
  printf("Bases :\r\n");

  unsigned int i;
  for(i=0; i<3; ++i)
  {
    printf("%d" "B : ",i+1);
    _inning.occupiedBases[i] ? printf(ANSI_COLOR_RED "occupied" ANSI_COLOR_RESET "\r\n") : printf("empty\r\n");
  }
  printf("\r\n");
}

Board switchHalfInning(Board _board)
{
  if(_board.currentHalfInning)
  {
    _board.currentHalfInning=0;
    ++_board.currentInning;
  }
  else
  {
    _board.currentHalfInning=1;
  }
  _board.currentSituation=initializeInning(_board.currentSituation);
  return _board;
}

Board rules(Board _board, unsigned int _dice1, unsigned int _dice2)
{
  if(_dice1==1 && _dice2==1)
  {
    printf(ANSI_COLOR_CYAN "Home Run : " ANSI_COLOR_RESET);
    _board=advance(_board,4); // HR
  }

  if(_dice1==2 && _dice2==2)
  {
    _board=doublePlay(_board); // double play
  }

  if(_dice1==3 && _dice2==3)
  {
    _board=walk(_board); // walk
  }

  if(_dice1==4 && _dice2==4)
  {
    _board=walk(_board); // walk
  }

  if(_dice1==5 && _dice2==5)
  {
    printf(ANSI_COLOR_CYAN "Double : " ANSI_COLOR_RESET);
    _board=advance(_board,2); //double
  }

  if(_dice1==6 && _dice2==6)
  {
    printf(ANSI_COLOR_CYAN "Home Run : " ANSI_COLOR_RESET);
    _board=advance(_board,4); //HR
  }

  if((_dice1==1 && _dice2==2) || (_dice1==2 && _dice2==1))
  {
    printf(ANSI_COLOR_CYAN "Double : " ANSI_COLOR_RESET);
    _board=advance(_board,2); // double
  }

  if((_dice1==1 && _dice2==3) || (_dice1==3 && _dice2==1))
  {
    printf(ANSI_COLOR_MAGENTA "Flyout" ANSI_COLOR_RESET "\r\n");
    _board=throwOut(_board,1); // Flyout
  }

  if((_dice1==1 && _dice2==4) || (_dice1==4 && _dice2==1))
  {
    _board=walk(_board); //walk
  }

  if((_dice1==1 && _dice2==5) || (_dice1==5 && _dice2==1))
  {
    printf(ANSI_COLOR_MAGENTA "Popout" ANSI_COLOR_RESET "\r\n");
    _board=throwOut(_board,1); // Popout
  }

  if((_dice1==1 && _dice2==6) || (_dice1==6 && _dice2==1))
  {
    printf(ANSI_COLOR_CYAN "Single : " ANSI_COLOR_RESET);
    _board=advance(_board,1); // single
  }

  if((_dice1==2 && _dice2==3) || (_dice1==3 && _dice2==2))
  {
    printf(ANSI_COLOR_MAGENTA "Groundout" ANSI_COLOR_RESET "\r\n");
    _board=throwOut(_board,1); // Groundout
  }

  if((_dice1==2 && _dice2==4) || (_dice1==4 && _dice2==2))
  {
    printf(ANSI_COLOR_MAGENTA "Strikeout" ANSI_COLOR_RESET "\r\n");
    _board=throwOut(_board,1); // K
  }

  if((_dice1==2 && _dice2==3) || (_dice1==3 && _dice2==2))
  {
    printf(ANSI_COLOR_CYAN "Single : " ANSI_COLOR_RESET);
    _board=advance(_board,1); // Single
  }

  if((_dice1==2 && _dice2==6) || (_dice1==6 && _dice2==2))
  {
    printf(ANSI_COLOR_MAGENTA "Strikeout" ANSI_COLOR_RESET "\r\n");
    _board=throwOut(_board,1); // K
  }

  if((_dice1==3 && _dice2==4) || (_dice1==4 && _dice2==3))
  {
    printf(ANSI_COLOR_CYAN "Triple : " ANSI_COLOR_RESET);
    _board=advance(_board,3); // Triple
  }

  if((_dice1==3 && _dice2==5) || (_dice1==5 && _dice2==3))
  {
    printf(ANSI_COLOR_MAGENTA "Groundout" ANSI_COLOR_RESET "\r\n");
    _board=throwOut(_board,1); // Groundout
  }

  if((_dice1==3 && _dice2==6) || (_dice1==6 && _dice2==3))
  {
    printf(ANSI_COLOR_MAGENTA "Flyout" ANSI_COLOR_RESET "\r\n");
    _board=throwOut(_board,1); // Flyout
  }

  if((_dice1==4 && _dice2==5) || (_dice1==5 && _dice2==4))
  {
    printf(ANSI_COLOR_MAGENTA "Popout" ANSI_COLOR_RESET "\r\n");
    _board=throwOut(_board,1); // Popout
  }

  if((_dice1==4 && _dice2==6) || (_dice1==6 && _dice2==4))
  {
    printf(ANSI_COLOR_MAGENTA "Strikeout" ANSI_COLOR_RESET "\r\n");
    _board=throwOut(_board,1); // Strikeout
  }

  if((_dice1==5 && _dice2==6) || (_dice1==6 && _dice2==5))
  {
    _board=sacFly(_board);
  }

  return _board;
}

Board advance(Board _board, unsigned int numberOfBases)
{
  unsigned int runsToScore=0;
  switch (numberOfBases)
  {
    case 1:
      _board.currentSituation.occupiedBases[2] ? ++runsToScore : runsToScore; // 3B -> Home
      _board.currentSituation.occupiedBases[2]=_board.currentSituation.occupiedBases[1]; // 2B -> 3B
      _board.currentSituation.occupiedBases[1]=_board.currentSituation.occupiedBases[0]; // 1B -> 2B
      _board.currentSituation.occupiedBases[0]=1; // Home -> 1B
      break;
    case 2:
      _board.currentSituation.occupiedBases[2] ? ++runsToScore : runsToScore; // 3B -> Home
      _board.currentSituation.occupiedBases[1] ? ++runsToScore : runsToScore; // 2B -> Home
      _board.currentSituation.occupiedBases[2]=_board.currentSituation.occupiedBases[0]; // 1B -> 3B
      _board.currentSituation.occupiedBases[1]=1; // Home -> 2B
      _board.currentSituation.occupiedBases[0]=0; // 1B is empty
      break;
    case 3:
      _board.currentSituation.occupiedBases[2] ? ++runsToScore : runsToScore; // 3B -> Home
      _board.currentSituation.occupiedBases[1] ? ++runsToScore : runsToScore; // 2B -> Home
      _board.currentSituation.occupiedBases[0] ? ++runsToScore : runsToScore; // 1B -> Home
      _board.currentSituation.occupiedBases[2]=1; // Home -> 3B
      _board.currentSituation.occupiedBases[1]=0; // 2B is empty
      _board.currentSituation.occupiedBases[0]=0; // 1B is empty
      break;
    case 4:
      ++runsToScore;
      unsigned int i;
      for(i=0; i<3; ++i)
      {
        _board.currentSituation.occupiedBases[i] ? ++runsToScore : runsToScore; // (i+1)B -> Home
        _board.currentSituation.occupiedBases[i]=0; // (i+1)B is empty
      }
      break;

    default: break;
  }
  _board=updateScore(_board, runsToScore);
  printf(ANSI_COLOR_BLUE "%d" ANSI_COLOR_RESET, runsToScore);
  printf(ANSI_COLOR_CYAN " scores" ANSI_COLOR_RESET "\r\n");
  return _board;
}

Board throwOut(Board _board, unsigned int numberOfOuts)
{
  _board.currentSituation.currentOuts+=numberOfOuts;
  return _board;
}

Board walk(Board _board)
{
  printf(ANSI_COLOR_CYAN "Walk : " ANSI_COLOR_RESET);
  if(!_board.currentSituation.occupiedBases[0]) // empty 1B
  {
    _board.currentSituation.occupiedBases[0]=1;
  }
  else
  {
    if(!_board.currentSituation.occupiedBases[1]) // 1B occ, 2B empty
    {
      _board.currentSituation.occupiedBases[1]=1;
    }
    else
    {
      if(!_board.currentSituation.occupiedBases[2]) // 1B,2B occ, 3B empty
      {
        _board.currentSituation.occupiedBases[2]=1;
      }
      else
      {
        _board=advance(_board,1); //1B,2B,3B occ
      }
    }
  }
  printf("\r\n");
  return _board;
}

Board doublePlay(Board _board)
{
  if(_board.currentSituation.occupiedBases[0])
  {
    printf(ANSI_COLOR_MAGENTA "Double play" ANSI_COLOR_RESET "\r\n");
    _board=throwOut(_board,2);
  }
  else
  {
    printf(ANSI_COLOR_MAGENTA "Out at first" ANSI_COLOR_RESET "\r\n");
    _board=throwOut(_board,1);
  }

  if(!isInningDone(_board))
  {
    _board=advance(_board,1); // if inning isn't finished, everyone advance
    _board.currentSituation.occupiedBases[0]=0; // except 1B
    _board.currentSituation.occupiedBases[1]=0; // and 2B
  }

  return _board;
}

Board sacFly(Board _board)
{
  printf(ANSI_COLOR_CYAN "Sacrifice Fly : " ANSI_COLOR_RESET);
  _board=throwOut(_board,1); // 1 is out
  if(!isInningDone(_board))
  {
    _board=advance(_board,1); // If inning not finished, everyone advance
    _board.currentSituation.occupiedBases[0]=0; // except 1B
  }

  return _board;
}

Board updateScore(Board _board, unsigned int scoredRuns)
{
  _board.currentScore[_board.currentHalfInning]+=scoredRuns;
  return _board;
}

unsigned int howManyOnBase(Board _board)
{
  unsigned int result=0;
  unsigned int i;
  for(i=0; i<3; ++i)
  {
    _board.currentSituation.occupiedBases[i] ? ++result : result; // is on base
  }
  return result;
}

unsigned int isInningDone(Board _board)
{
  unsigned int result;
  if(_board.currentSituation.currentOuts>=3)
  {
    result=1;
  }
  else
  {
    result=0;
  }
  return result;
}

unsigned int throwDice()
{
  return ((rand()%6)+1);
}
