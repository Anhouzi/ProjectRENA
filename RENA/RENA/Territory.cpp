#include "Territory.h"

/*- Territory Object: The primary structure of data that will hold multitudes of 
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
			- Label: Integer value of the territory for the game to reference.
			
			- Turn Actions:
		- Turn Actions are divided into Military, Diplomacy, and Economics,
		to reduce the number of options presented to the player at any one time.
			- This also gives some indicatation about what the action will do.
			- Military:
				Move, War, Hire, Train, View
			- Diplomacy: 
				Pact, Bribe, Rest, Grant
			- Economic:
				Tax, Send, Dam, Grow, Trade, Build, Give
			- At any point in the options a player should be able to go back to the main menu. 
			- A player should be able to add an 'h' to the end of a command
			to receive information about it.

		Actions marked with a '/' may be cut from the final version due to redundancy
		1. Move		- Moves troops to a different location.
		2. War		- Attacks an adjacent location owned by not you.
		3. Tax		- Increases income based on wealth. Higher values lower loyalty.
		4. Send		- Sends resources to another place.
		5. Dam		- Builds dams which are protections against disasters.
		6. Pact		- Make an alliance with someone else.
		7. Grow		- Grow Rice to not starve.
		/8. Marry	- Better alliance. 
		9. Trade	- Sell or purchase resources at the market for gold.
		10. Hire	- Obtain more soldiers.
		11. Train	- Make all soldiers stronger.
		12. View	- Look at the map. Does not end turn.
		13. Build	- Increase the town value.
		14. Give	- Give stuff to soldiers or towns folk to increase loyalty.
		15. Bribe	- Give stuff to someone else to make them weaker.
		/16. Assign	- Change soldier proportions for tactical combat.
		17. Rest	- Restore health. Skips the player's next turn (including all their other territories)
		/18. Map	- Look at the map.
		19. Grant	- Give a territory to someone. 
		20. Other	- Game Options*/

Territory::Territory()
{
	//Randomly roll the Territory's starting stats.
	Gold = rand() % 55 + 100;
	Debt = 0;
	Town = rand() % 30 + 45;
	Food = rand() % 35 + 40;
	Output = rand() % 30 + 75;
	Infrastructure = rand() % 20 + 60;
	Loyalty = rand() % 30 + 50;
	Wealth = rand() % 30 + 65;
	Soldiers = rand() % 25 + 30;
	Morale = rand() % 50 + 30;
	Skill = rand() % 50 + 50;
	Arms = rand() % 20 + 20;
	TaxRate = .2f;

	isAI = true;
}

/* Action Notes: 
	- If an action returns 1, it will not end the territory's turn. */

/* Military Actions */

/* Move() - Moves troops to a different, adjacent location. The target location must
be owned by the player. */
int Territory::Move()
{
	
	Territory *target = nullptr;
	int TroopMovement = 0;
	if (leader->ControlledTerritories.size() < 2)
	{
		if (!isAI)
		{
			std::cout << "No viable territories to move troops to." << std::endl;
		}
		return 1;
	}
	if (isAI)
	{
		target = Adjacency[0];
	}
	
	std::cout << "Territory " << TerritoryNumber << " Move -- " << std::endl; //Placeholder text.
	return 0;

}

/* War() */

int Territory::War(Territory *Target, int SoldierInvestment)
{
	
	int EnemySoldierInvestment;
	//If the enemy has no soldiers to fight with, we win automatically.

	EnemySoldierInvestment = std::min(Target->Gold, Target->Food);
	EnemySoldierInvestment = std::min(EnemySoldierInvestment, Target->Soldiers);

	float CombatPowerA = SoldierInvestment  * (1 + (Skill / 100.0)) * (.2 + .8 * (SoldierInvestment / Arms));
	float CombatPowerB = EnemySoldierInvestment * (1 + (Target->Skill / 100.0)) *
		(.2 + .8 * (EnemySoldierInvestment / Target->Arms));

	CombatPowerB *= 1.25f;

	if (EnemySoldierInvestment <= 0)
	{
		CombatPowerB = 1;
	}

	int Survivors = SoldierInvestment * (1 - (CombatPowerB / CombatPowerA));

	Soldiers -= SoldierInvestment;
	if (Soldiers < 0) { Soldiers = 0; }
	Target->Soldiers = Target->Soldiers * (1 - (CombatPowerA / CombatPowerB));
	if (Target->Soldiers < 0) { Target->Soldiers = 0; }

	Arms = Arms * (1 - ((CombatPowerB / CombatPowerB) / 2));
	if (Arms < 0) { Arms = 0; }
	Target->Arms = Target->Arms * (1 - ((CombatPowerA / CombatPowerB) / 2));
	if (Target->Arms < 0) { Target->Arms = 0; }

	Gold -= SoldierInvestment;
	Food -= SoldierInvestment;

	Target->Gold -= EnemySoldierInvestment;
	Target->Food -= EnemySoldierInvestment;
	Target->Soldiers = Survivors;
	Target->Skill = Skill;
	//target->leader->SetOwner(leader);

	std::cout << std::endl;
	//View();

	return 1;
	//War simulation and resolution will go here. Probably utility functions.
	std::cout << "Territory " << TerritoryNumber << " War -- " << std::endl;
	return 0;
}


/* SimulateBattle() */
/* Utility function for the war command. Calculates the combat power of this Territory
and the target territory, then do the following: 
	Territory A Soldiers = sA * (1-(cpB/cpA))
	Territory A Arms = aA * (1-((cpB/cpA) / 2)) 
	Territory B Soldiers = sB * (1-(cpA/cpB))
	Territory B Arms = aB * (1-((cpA/cpB) / 2))
	
	- A territory cannot have less than zero of any value. 
	*/

int Territory::SimulateBattle(Territory* target, int soldierInvestment)
{
	
}

/* Hire(int) - Subtract Gold from the Territory and add a proportional number of Soldiers.*/
int Territory::Hire(int NumberToHire)
{

	//TODO soldiers cost a certain amount of gold, and Territories can input the number they hire.
		//If we have more soldiers than gold, we do not want to hire more. 
	if (NumberToHire * 3 > Gold)
	{
		return 1; 
	}
	Gold -= NumberToHire * 3;
	int SoldierIncrease = NumberToHire;
	std::cout << "Territory " << TerritoryNumber << " Hire -- " << std::endl;

	//We also have to adjust the skill level of the territory since they are getting an influx of new recruits.
		//Skill = Skill*Soldiers + ((rand() % 30 + 40) * SoldierIncrease) / (Soldiers + SoldierIncrease)
		//Averages out the skill between the current skill level and the incoming new soldiers.

	std::cout << "Skill: " << Skill;
	Skill = ((Skill * Soldiers) + ((rand() % 30 + 40) * SoldierIncrease)) / (Soldiers + SoldierIncrease);
	std::cout << " -> " << Skill << std::endl;
	
	//Soldier increase
	std::cout << "Soldiers: " << Soldiers;
	Soldiers += SoldierIncrease;
	std::cout << " -> " << Soldiers << std::endl;

	

	return 0;
}

/* Train() - Increases the Territory's skill by a random amount. */
int Territory::Train()
{
	int SkillIncrease = rand() % 80 + 40;
	std::cout << "Territory " << TerritoryNumber << " Train -- ";
	std::cout << "Skill: " << Skill;
	Skill += SkillIncrease;
	std::cout << " -> " << Skill << std::endl;
	return 0;
}

/* View() */
/* Immediately prints the state of the chosen Territory to the player.
	- Costs gold to use and will not end the turn of any player using it.
	- AI Players using it will only have their gold deducted, since they will already
	have a reference to any territory that is relevant through their adjacency list.*/
int Territory::View()
{
	//Check if the user has enough gold to use the command.

	DisplayState();
	std::cout << "Territory " << TerritoryNumber << " View -- " << std::endl;
	return 1;
}

//Does the same as view, but for another territory.
int Territory::View(Territory* target)
{
	target->DisplayState();
	std::cout << "Territory " << TerritoryNumber << " View -- " << std::endl;
	return 1;
}

/* Diplomatic Actions */
int Territory::Pact()
{
	std::cout << "Territory " << TerritoryNumber << " Pact -- " << std::endl;
	return 1;
}

int Territory::Bribe()
{
	std::cout << "Territory " << TerritoryNumber << " Bribe -- " << std::endl;
	return 1;
}

/* Rest() - Increases the leader's health by a random amount. */
int Territory::Rest()
{
	int HealthIncrease = rand() % 4 + 2;
	
	std::cout << "Territory " << TerritoryNumber << " Rest -- ";
	std::cout << "Health " << leader->Health;
	leader->Health += HealthIncrease;
	std::cout << " -> " << leader->Health << std::endl;

	return 0;
}

int Territory::Grant()
{
	std::cout << "Territory " << TerritoryNumber << " Grant -- " << std::endl;
	return 1;
}

/* Economic Actions */
/* Tax() - Changes the percentile value of the Territory's taxes. Higher values decrease loyalty,
lower values increase loyalty. */
int Territory::Tax(float TaxValue)
{
	//Change the Territory's Loyalty value proportionately to the change in the tax plan. 
	//Going lower means an increase in loyalty, going higher means a decrease. 
	Loyalty *= (int)floor(TaxRate / TaxValue);
	TaxRate = TaxValue;
	std::cout << "Territory " << TerritoryNumber << " Tax -- " << std::endl;
	return 1;
}

int Territory::Send()
{
	std::cout << "Territory " << TerritoryNumber << " Send -- " << std::endl;
	return 1;
}

/* Dam(int) - Spend Gold on increasing the Infrastructure of the Territory.*/
int Territory::Dam(int Investment)
{
	//Increase the infrastructure value by 1 per 100 Gold invested.
	int InfrastructureIncrease = (int) floor(Investment*.01);
	Gold -= Investment;
	std::cout << "Territory " << TerritoryNumber << " Dam -- ";
	std::cout << "Infrastructure: " << Infrastructure;
	Infrastructure += InfrastructureIncrease;

	//Capping Infrastructure at 100.
	if (Infrastructure > 100)
	{
		Infrastructure = 100;
	}
	std::cout << " -> " << Infrastructure << std::endl;
	return 0;
}

/* Grow(int) - Spend Gold on increasing the Output of the Territory.*/
int Territory::Grow(int Investment)
{
	//Increase the Output value by some ratio based on the investment multiplied by the inverse of the current output. 
	int OutputIncrease = (int)floor(Investment*(50.0/Output));
	Gold -= Investment;
	std::cout << "Territory " << TerritoryNumber << " Grow -- ";
	std::cout << "Output: " << Output;
	Output += OutputIncrease;
	std::cout << " -> " << Output << std::endl;
	return 0;
}

/* Trade() - Allows the Territory to interact with the market in the following ways:
	- Buy Food: Trade Gold for Food at some loss.
	- Sell Food: Trade Food for Gold at some loss.
	- Loan*: Receive a loan of Gold as well as an increasing amount of debt per turn.
	- Repay*: Pay off loan of Gold. 
	- Arms: Trade Gold for Arms. 
*/
int Territory::Trade(int Command, int Input)
{
	switch (Command)
	{
	case 0: //Buy Food
		// Input: Desired food amount to buy
		if (Input * 1.2 > Gold)
		{
			return 1;
		}
		else
		{
			Food += Input;
			Gold -= (int)floor(Input * 1.2);
		}
		break;
	case 1: //Sell Food
		// Input: Desired amount of food to sell
		if (Input > Food)
		{
			return 1;
		}
		else
		{
			Food -= Input;
			Gold += (int)floor(Input * .8);
		}
		break;
	case 2: //Buy Arms
		// Input: Desired amount of arms to buy
		if (Input * 5 > Gold)
		{
			return 1;
		}
		else
		{
			Gold -= Input * 5;
			Arms += Input;
		}
		break;
	}
	std::cout << "Territory " << TerritoryNumber << " Trade -- " << std::endl;
	return 1;
}

/* Dam(int) - Spend Gold on increasing the Town value of the Territory.*/
int Territory::Build(int Investment)
{
	//DEBUGGED: Without the decimal 50/Town was giving me zero because the compiler thought it was an int.
	int TownIncrease = (int)floor(Investment*(50.0 / Town));
	Gold -= Investment;
	std::cout << "Territory " << TerritoryNumber << " Build -- ";
	std::cout << "Town " << Town;
	Town += TownIncrease;
	std::cout << " -> " << Town << std::endl;

	return 0;
}

/* Give(int, int) - Spend Food on increasing the Wealth and Loyalty by throwing a feast for citizens
or increase the Soldiers morale by throwing a feast for the soldiers.*/
int Territory::Give(int Command, int Input)
{
	int LoyaltyIncrease, MoraleIncrease, WealthIncrease;
	std::cout << "Territory " << TerritoryNumber << " Give -- " << std::endl;
	switch (Command)
	{
	case 0: //Give to Soldiers
		break;
	case 1: //Give to Citizens
		WealthIncrease = (int)floor(Input*(10.0 / Wealth));
		LoyaltyIncrease = (int)floor(Input*(15.0 / Loyalty));

		Food -= Input; 
		std::cout << "Wealth " << Wealth;
		Wealth += WealthIncrease;
		std::cout << " -> " << Wealth << std::endl;

		std::cout << "Loyalty " << Loyalty;
		Loyalty += LoyaltyIncrease;
		std::cout << " -> " << Loyalty << std::endl;
		break;
	}
	
	return 1;
}

/* Territory Utility Functions */

/* TakeTurn() */
/* AI utility method that randomly rolls an action based on the tendencies of the owner. */
// DEPRECATED
int Territory::TakeTurn()
{
	int ActionResult = 0; 
	
	return ActionResult;
}

/* TerritoryUpdate() */
/* Called after every territory's action. 
Updates the territory's state for various upkeeps and income.
	Upkeeps: 
		- Troop Upkeep: Gold -= Soldiers *.25, Food -= Soldiers *.25
		- Town Upkeep: Food -= Town *.5
		- Natural Erosion (based on infrastructure): Gold *= Infrastructure/100, Food *= Infrastructure/100
	Incomes:
		- Taxes: Gold += Town * (Wealth/100) * TaxRate
		- Farms: Food += Output
	Other: 
		- Check for troop skill levels. (TODO)
			- If there are no troops set skill to 0.
			- If there are troops and skill is 0, randomize skill based on Territory constructor. */
void Territory::TerritoryUpdate()
{
	const float SoldierUpkeep = .25;
	const float TownUpkeep = .50;
	/* Upkeep */
	//Soldier Upkeep
	Gold -= (int) ceil(Soldiers * SoldierUpkeep);
	Food -= (int) ceil(Soldiers * SoldierUpkeep);
	//std::cout << "Soldier Upkeep: " << Soldiers * SoldierUpkeep << std::endl;
	//Town Upkeep
	Food -= (int) ceil(Town * TownUpkeep);

	//Natural Upkeep
	//Gold *= (Infrastructure / 100);
	//Food *= (Infrastructure / 100);
	leader->Health--;

	/* Income */
	Gold += (int) ceil(Town * ((Wealth / 100) * TaxRate));
	Food += Output;
	//std::cout << "Town: " << Town << " Wealth: " << Wealth << " Tax Rate: " << TaxRate << std::endl;
	//std::cout << "Gold Income: " << (int) ceil(Town * (Wealth/100) * TaxRate) << std::endl;
	//View();
	/*
	if (Gold < 0)
	{
		std::cout << "Territory: " << TerritoryNumber << " is in debt." << std::endl;
	}
	if (Food < 0)
	{
		std::cout << "Territory: " << TerritoryNumber << " is starving." << std::endl;
	}
	*/
}


/* Display State() */
/* Prints out the state of the territory to the console. Including the leader statistics. */
void Territory::DisplayState()
{
	std::cout << "-- Territory " << TerritoryNumber << " --" << std::endl;
	std::cout << "Gold: " << Gold << std::endl;
	std::cout << "Town: " << Town << std::endl;
	std::cout << "Food: " << Food << std::endl;
	std::cout << "Output: " << Output << std::endl;
	std::cout << "Infrastructure: " << Infrastructure << std::endl;
	std::cout << "Loyalty: " << Loyalty << std::endl;
	std::cout << "Wealth: " << Wealth << std::endl;
	std::cout << "Soldiers: " << Soldiers << std::endl;
	std::cout << "Morale: " << Morale << std::endl;
	std::cout << "Skill: " << Skill << std::endl;
	std::cout << "Arms: " << Arms << std::endl;
	std::cout << std::endl;
	std::cout << "Owner: " << leader->Player << std::endl;
	std::cout << "Health: " << leader->Health << std::endl;
	std::cout << "Drive: " << leader->Drive << std::endl;
	std::cout << "Intelligence: " << leader->Intelligence << std::endl;
	std::cout << "Charisma: " << leader->Charisma << std::endl;
	std::cout << "Luck: " << leader->Luck << std::endl;

}

