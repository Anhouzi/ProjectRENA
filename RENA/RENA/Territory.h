#pragma once
#include <iostream>
#include <string>
#include <time.h>
#include <vector>
#include <algorithm>
#include "Leader.h"
/* - Military:
				Move, War, Hire, Train, View
			- Diplomacy: 
				Pact, Bribe, Rest, Grant
			- Economic:
				Tax, Send, Dam, Grow, Trade, Build, Give*/
class Leader; 
//Forward Declaration Resource: https://stackoverflow.com/questions/4757565/what-are-forward-declarations-in-c
class Territory
{
private:
	int SimulateBattle(Territory* target, int soldierInvestment);

public:
	bool isAI; 
	Leader *leader; 
	std::vector<Territory*> Adjacency;
	int Gold, Debt, Town, Food, Output, Infrastructure, Loyalty,
		Soldiers, Morale, Skill, Arms, TerritoryNumber;
	float TaxRate, Wealth;
	Territory();
	//Military Actions
	int Move();
	int War(Territory *Target, int SoldierInvestment);
	int Hire(int Investment);
	int Train();
	int View();
	int View(Territory*);

	//Diplomatic Actions
	int Pact();
	int Bribe();
	int Rest();
	int Grant();

	//Economic Actions
	int Tax(float TaxValue);
	int Send();
	int Dam(int Investment);
	int Grow(int Investment);
	int Trade(int Command, int Input);
	int Build(int Investment);
	int Give(int Command, int Input);

	//Territory Utility Functions
	int TakeTurn();
	void TerritoryUpdate();
	void DisplayState();
};