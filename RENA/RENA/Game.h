#pragma once
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include "Leader.h"
#include "Territory.h"

using namespace std; 
class Leader;
class Territory;

/* Game Functions */
/* The game requires a series of functions in order to the game to progress.
	- Pre-Game Setup: A series of prompts that lets the player set the settings/parameters of the game.
	- Game Setup: An actual call to create the game and all of the players. 
	- Rounds and Turns: 
		- A round is only complete when all of the turns are complete.
		- Each Territory can make one move per turn. 
			- Effects of that turn are calculated as soon as the move is made, not between rounds. 
		- Some actions will not end the turn (see "Turn Actions" below) 
	
	
	- Win Condition: When a player owns every territory, the game will end. 
	I.e. when the number of leaders == 1.
*/

/* Map (May not need this since the map will be a 2D array anyway)*/
/* The map class will be a graph that contains all of the territories and their connections. 
	Variables:
		- TerritoryCount: Total number of territories. 
	Private Functions (Primarily for Map Setup):
		- AddNode(int): Adds a territory to the map using the argument as its reference number.
		- ConnectNodes(Territory, Territory): Adds T1 to the adjacency list of T2 and vice versa.
	Public Functions: 
		- Constructor(int): Creates an empty graph. 
		- GetTerritory(int): Returns a pointer to the territory whose reference matches the argument.
		- DisplayMap(): A long series of cout statements that will show the map*/

/* Game */
/* Game class will keep track of the high level elements of gameplay. 
	- Map handling
	- Win condition tracking
	- Game options management
*/
class Game
{
	int NumberOfLeaders, NumberOfTerritories, GameDifficulty;
	Territory ***Map;
	//Leader *LeaderList;
private:
	
public: 
	void CreateMap();
	void ConnectMap(int, int);
	void PrintMap();
	void GameOptions();
	void GameSetup();
	Territory *GetTerritory(int);
	Territory ***GetMap();
	int GetNumberOfLeaders();
	int GetNumberOfTerritories();
	int CheckState();

};
 
void LeaderCreation(bool AI, int points);


