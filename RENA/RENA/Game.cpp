
#include "Game.h"

/* GameOptions() */
/* Called at the start of any new game and asks the player the following questions:
	- Number of civilizations.
	- Which civilization they wish to play.
	- Their leader's starting stats.
	- The game difficulty. */

/* To keep the size as square as possible when generating the map, we will have
a number of rows equal to the square root of the number of leaders (floor). (5 rows for 25-35 territories)
and a number of columns equal to NumberOfLeaders/NumberOfRows (ceil)*/

/* Accessibility Notes */
/* Map is a vector containing an array of Territory array References.
	- To access a row, use Map[x]. 
	- To access a specific territory, use Map[x][y].*/

/* Create Map */
/* Creates a 2D array of Territories for the game map. Gives each Territory a reference number and
sets up the adjacency list for all territories after the map is set up.*/
void Game::CreateMap()
{
	
	double RowsD = floor(sqrt(NumberOfLeaders));
	double ColsD= ceil(NumberOfLeaders / RowsD);
	int rows = (int)RowsD;
	int cols = (int)ColsD; //Using rows instead of reusing sqrt(NumberOfLeaders) gave better results.

	//int TwoDee[rows][cols];
	//Territory Map2[rows][cols];
	//cout << "Rows: " << rows << " Cols: " << cols << endl;
	//Initialize map to an array of Territory pointers.
	Map = new Territory**[rows];
	for (int i = 0; i < rows; i++)
	{
		//Initialize a new row of Territories.
		Territory **row = new Territory*[cols];
		for (int j = 0; j < cols; j++)
		{
			if ((i * cols) + j < NumberOfLeaders)
			{
				row[j] = new Territory();
				Territory* Current = row[j];
				//Set the Territory Number seperately from the constructor.
				Current->TerritoryNumber = (i * cols) + j;

				//Setup a new leader for the territory.
				Current->leader = new AI(i * cols + j);
				Current->leader->ControlledTerritories.push_back(Current);
				//cout << Current->TerritoryNumber << endl;

				
			}
			else
			{
				row[j] = nullptr;
			}
		}
		//Set the row with all the columns.
		Map[i] = row;
	}
	//cout << "Map test: " << Map[0][0]->TerritoryNumber << endl;
	//std::cout << "Connecting Map" << std::endl;
	ConnectMap(rows, cols);
	//std::cout << "Map Connections finished" << std::endl;
}

//Utility function for CreateMap() to establish the adjacency lists for all the territories.
void Game::ConnectMap(int rows, int cols)
{
	/* We are going to cycle through the whole list and fill the adjacency list of
	each element.
		Adjacency Restraints: 
		- i - 1 < 0 == Trying to access a row above the top row.
		- i + 1 > rows == Trying to access a row below the bottom row.
		- j - 1 < 0 == Trying to access a column to the left of the left most column.
		- j + 1 > cols == Trying to access a column to the right of the right most column.
	*/
	//int count = 0;
	//Territory *temp = nullptr;
	for (int i = 0; i < rows; ++i)
	{
		for (int j = 0; j < cols; ++j)
		{
			
			if ((i*cols + j) >= NumberOfLeaders)
			{
				//Map[i][j] = NULL;
				break;
			}
			
			
			//Initialize the adjacency list. Since no territory can have
			//more than four adjacent territories, an array of 4 is all that is needed.
			/* Adjacency 
			- [0]: Northern Territory (i - 1)
			- [1]: Eastern Territory  (j + 1)
			- [2]: Southern Territory (i + 1)
			- [3]: Western Territory  (j - 1)
			- If any value is set to NULL there is no territory adjacent to it in that direction.
			*/

			//Map[i][j].Adjacency = new Territory*[4];	

			//Validate the slot above this one. 
			//If there is a row above this one...
			
			if ((i - 1) >= 0)
			{
				//and if the territory above this one isn't empty.
				if (GetTerritory(((i - 1) * cols) + j) != nullptr)
				{
					Map[i][j]->Adjacency.push_back(GetTerritory(((i - 1) * cols) + j));
				}
				else
				{
					Map[i][j]->Adjacency.push_back(nullptr);
				}
			}
			else
			{
				Map[i][j]->Adjacency.push_back(nullptr);
			}
			//Validate the slot below this one.
			if ((i + 1) < rows)
			{
				if (GetTerritory(((i + 1) * cols) + j) != nullptr)
				{
					Map[i][j]->Adjacency.push_back(GetTerritory(((i + 1) * cols) + j));
				}
				else
				{
					Map[i][j]->Adjacency.push_back(nullptr);
				}
			}
			else
			{
				Map[i][j]->Adjacency.push_back(nullptr);
			}
			//Validate the slot to the left of this one.
			if ((j - 1) >= 0)
			{
				if (GetTerritory((i * cols) + (j - 1)) != nullptr)
				{
					Map[i][j]->Adjacency.push_back(GetTerritory((i * cols) + (j - 1)));
				}
				else
				{
					Map[i][j]->Adjacency.push_back(nullptr);
				}
			}
			else
			{
				Map[i][j]->Adjacency.push_back(nullptr);
			}
			//Validate the slot to the right of this one.
			if ((j + 1) < cols)
			{
				if (GetTerritory((i * cols) + (j + 1)) != nullptr)
				{
					Map[i][j]->Adjacency.push_back(GetTerritory((i * cols) + (j + 1)));
				}
				else
				{
					Map[i][j]->Adjacency.push_back(nullptr);
				}
			}
			else
			{
				Map[i][j]->Adjacency.push_back(nullptr);
			}
			//DEBUG Loop to check the adjacency lists. 
			//cout << Map[i][j]->Adjacency.size() << endl;
			/*
			for (int k = 0; k < Map[i][j]->Adjacency.size(); ++k)
			{
				if (Map[i][j]->Adjacency[k] != nullptr)
				{
					cout << "Territory " << Map[i][j]->Adjacency[k]->TerritoryNumber << endl;
				}
			}
			cout << endl;
			*/
		}
	}
}

void Game::PrintMap()
{
	double RowsD = floor(sqrt(NumberOfLeaders));
	double ColsD = ceil(NumberOfLeaders / RowsD);
	int rows = (int)RowsD;
	int cols = (int)ColsD;
	int TerritoryCount = 0;

	for (int i = 0; i < rows; ++i)
	{
		//We need a series of for-loops for each line to be displayed.
		//A single for loop can only print a single column of territories. 
		for (int j = 0; j < cols; ++j)
		{
			if (i * cols + j < NumberOfLeaders)
			{
				cout << "#####  ";
			}
		}
		cout << endl;
		for (int j = 0; j < cols; ++j)
		{
			if (i * cols + j < NumberOfLeaders)
			{
				cout << "# " << (i * cols) + j << " #  ";
			}
		}
		cout << endl;
		for (int j = 0; j < cols; ++j)
		{
			if (i * cols + j < NumberOfLeaders)
			{
				cout << "#####  ";
			}
		}
		cout << endl << endl;
	}
}

void Game::GameOptions()
{
	string input = "";
	size_t sz;
	cout << "Welcome to RENA!" << endl;
	cout << "Enter the number of Civilizations in the game." << endl;

	//TODO: Input validation.
	cin >> input;
	NumberOfLeaders = stoi(input, &sz);
	cout << endl;
	//DEBUG: cout << NumberOfLeaders << endl;

	cout << "Enter the game difficulty (1-5)." << endl;

	cin >> input;
	GameDifficulty = stoi(input, &sz);
	cout << endl;
	//DEBUG: cout << GameDifficulty << endl;

	//LeaderCreation(true, 100);
	
}

/* Called after the Game Options are confirmed and sets up the game.
	- Builds the game graph. 
	- Creates territories. */
void Game::GameSetup()
{
	CreateMap();
}

/* GetTerritory */
/* Returns a reference to the territory in the map given by the reference number.
 x value is the column. ref % cols
 y value is the row. ref / cols
 */
Territory * Game::GetTerritory(int ref)
{

	double RowsD = floor(sqrt(NumberOfLeaders));
	double ColsD = ceil(NumberOfLeaders / RowsD);
	int rows = (int)RowsD;
	int cols = (int)ColsD;
	int x = ref / cols;
	int y = ref % cols;

	return Map[x][y];
}

Territory *** Game::GetMap()
{
	return Map;
}

int Game::GetNumberOfLeaders()
{
	return NumberOfLeaders;
}

//Utility function that will run the leader creation process.
void LeaderCreation(bool isAI, int pointTotal)
{
	int PointTotal = pointTotal;
	int v1;//, v2, v3;
	if (isAI)
	{
		//srand(time(NULL));	//Required for actual randomness. Sets a new random seed.
		for (int i = 0; i < 5; i++)
		{
			// rand() % 'x' limits the number to numbers between 0 - (n-1)
			v1 = rand() % 100 + 1; 
			cout << v1 << " ";
		}
		cout << endl;
	}
}
