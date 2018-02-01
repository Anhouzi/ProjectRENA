/* Project: RENA by Matthew Freeman */
/* RENA (Re Explored Nobunaga's Ambition) will be my attempt
	at recreating beloved strategy game Nobunaga's Ambition.

	Detailed Strategy Guide for the game on gamefaqs.com:
	https://www.gamefaqs.com/nes/587494-nobunagas-ambition/faqs/56980

	Project Notes: 
		- I will be attempting to create this game from the perspective of 
		an operational AI before attempting to implement player controls. 
		- This project, while based on Nobunaga's Ambition, will see some thematic
		changes to make the game a little more interesting to produce. 

	Architecture: 
		- Leader Object : Health, Drive, Luck, Charisma, IQ
			- Health: The time limit value. Decreases a little bit every turn and if it reaches 0, game over.
			- Drive: Modifier for military activities like training and combat modifiers.
			- Luck: Variable to increase success chance in many actions.
			- Charisma: Attribute to determine the success rate of diplomatic actions.
			- IQ: Improved efficacy with economic actions.
			HIDDEN:
			- Owner: Player, Aggro, Diplo, Econo, Allies
				- Player: Integer of the player that owns this territory.
				- Aggro: Value that determines the probability that the owner will make militaristic moves.
				- Diplo: Value that determines the probability that the owner will make diplomatic/defensive.
				- Econo: Value that determines the probability that the owner will make economic moves.
				- Allies: List of this players allies and thus cannot be attacked.


		- Territory Object: The primary structure of data that will hold multitudes of 
		"territorial" data for the player and AI to make decisions on. 

			- Territory: Leader, Gold, Debt, Town, Food, Output, Infrastructure, Loyalty, 
						 Wealth, Soldiers, Morale, Skill, Arms
				- Leader: Reference to the leader that owns the territory.
				- Gold: Money with which to buy things.
				- Debt: Money owed from loans.
				- Town: Value of the territory. Modifies income. Increased by building, decreased by storms.
				- Food: Amount of stored food for the territory. Food is required to feed troops and people. 
					Low values modify loyalty and lose battles.
				- Output: How much food can be produced.
				- Infrastructure: Reduces damage from natural disasters.
				- Loyalty: Value that determines rebellions
				- Wealth: The average wealth of the citizens. Higher values mean more income. 
				- Soldiers: Army size within the territory.
				- Morale: Value that determines soldiers' susceptibility to the charisma of other territories.
				- Skill: How well trained the soldiers are.
				- Arms: How well armed the soldiers are. 
				HIDDEN:
				- Adjacent: List of adjacent territories.

	Rules:
	- Every round each territory will get a turn to take an action (see below). Once every territory has taken
	an action, the round restarts. +
	*/
#include <iostream>
#include "Game.h"

using namespace std;

int main()
{
	srand(time(NULL)); //Required for actual randomness. Sets a new random seed. ONLY USE THIS ONCE!
	Game newGame;
	newGame.GameOptions();
	newGame.GameSetup();
	//newGame.CreateMap();
	
	newGame.PrintMap();
	int rounds = 0;
	int gameState = 1;
	Territory * playerTerritory = newGame.GetTerritory(0);
	playerTerritory->leader = new User(0, &newGame);
	while (true)
	{
		for (int i = 0; i < newGame.GetNumberOfTerritories(); ++i)
		{
			//cout << "Test" << endl;
			int TurnComplete = 1;
			Territory *territory = newGame.GetTerritory(i);
			do
			{
				TurnComplete = territory->leader->TakeTurn(territory);
			} while (TurnComplete != 0);
			territory->TerritoryUpdate();
		}
		gameState = newGame.CheckState();
		if (gameState < 1)
		{
			std::cout << "Game over!" << std::endl;
			break;
		}
		cout << endl;
		rounds++;
	}
	return 0;
}