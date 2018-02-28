#pragma once
#include <vector>
#include <iostream>
//#include <algorithm>
#include "Territory.h"
#include "Game.h"


/* Leader Object */

/*
- Leader Object : Health, Drive, Luck, Charisma, IQ
	- Health: The time limit value. Decreases a little bit every turn and if it reaches 0, game over.
	- Drive: Modifier for military activities like training and combat modifiers.
	- Luck: Variable to increase success chance in many actions.
	- Charisma: Attribute to determine the success rate of diplomatic actions.
	- Intelligence: Improved efficacy with economic actions.
	HIDDEN:
	- Owner: Player, Aggro, Diplo, Econo, Allies
		- Player: Integer of the player that owns this territory.
		- Aggro: Value that determines the probability that the owner will make militaristic moves.
		- Diplo: Value that determines the probability that the owner will make diplomatic/defensive.
		- Econo: Value that determines the probability that the owner will make economic moves.
		- Allies: List of this players allies and thus cannot be attacked.
*/
class Territory; //Forward declaration for Territory functionality.
class Game;
				 
// We are changing leader into a virtual class for AI and User it will only contain variables
// and methods that all AIs and Users should have.

/* Leader Class 
	- Player: The player number to differentiate the various leaders from each other.
	- Health: The Leader's health is the "turn counter" in that if it should hit zero the leader
	dies and the territories become unowned.
	- Drive: The Leader's drive is the variable that governs morale and loyalty. Leaders with
	higher drive will have thier territories loyalty and morale decline slower.
	- Luck: Luck effects random number generation for rolls like training troops. 
	- Charisma: Charisma effects diplomatic actions by increasing the odds of success with higher values. 
	- Intelligence: Intelligence slightly increases the effectiveness of all actions taken in game. */
class Leader
{
public:
	bool isAiPlayer;
	int Player, Health, Drive, Luck, Charisma, Intelligence;
	std::vector<Territory *> ControlledTerritories;

	Leader();
	//Leader(int player);
	//AI* GetOwner();
	//void SetOwner(Leader*);

	//Virtual Functions that all children MUST define.
	virtual int TakeTurn(Territory *CurrentTerritory) = 0;

	//Leader Functions

};

class AI : public Leader
{
public:
	//int Player;
	float Aggro, Diplo, Econo;
	//Owner *Allies;

public:
	AI(int player);

	int TakeTurn(Territory *CurrentTerritory);
	int GetStateSelf(Territory *CurrentTerritory);
	int SafeAction(Territory *CurrentTerritory);
	int MilitaryAction(Territory *CurrentTerritory, int Action);
	int mWarAction(Territory *CurrentTerritory);
	int CalculateInvestment(Territory * CurrentTerritory, Territory * target);
	int mHireAction(Territory *CurrentTerritory);
	int DiplomaticAction(Territory *CurrentTerritory, int Action);
	int EconomicAction(Territory*CurrentTerritory, int Action);
	int eGrowAction(Territory *CurrentTerritory);
	int eBuildAction(Territory *CurrentTerritory);
	int eGiveAction(Territory *CurrentTerritory);
	
	//DEBUG Method Declarations
	void AiStatistics();
	void TerritoryStatistics(Territory * CurrentTerritory);
};

class User : public Leader
{
	Game *GameReference;
public:
	
	User(int player, Game *gameReference);
	int TakeTurn(Territory *CurrentTerritory);

	void MenuDisplay(int ActionCategory);
	bool HandleActionMilitary(Territory *CurrentTerritory, int ActionNumber);
	bool MoveAction(Territory *CurrentTerritory);
	bool WarAction(Territory *CurrentTerritory);
	bool HireAction(Territory *CurrentTerritory);
	bool TrainAction(Territory *CurrentTerritory);
	bool ArmsAction(Territory *CurrentTerritory);
	bool RationsAction(Territory *CurrentTerritory);
	bool HandleActionDiplomatic(Territory *CurrentTerritory, int ActionNumber);
	bool RestAction(Territory * CurrentTerritory);
	bool HandleActionEconomic(Territory *CurrentTerritory, int ActionNumber);

	bool TaxAction(Territory * CurrentTerritory);

	bool SendAction(Territory * CurrentTerritory);

	bool DamAction(Territory * CurrentTerritory);

	bool GrowAction(Territory * CurrentTerritory);

	bool TradeAction(Territory * CurrentTerritory);

	bool BuildAction(Territory * CurrentTerritory);

	bool CelebrateAction(Territory * CurrentTerritory);

};




