#include "Leader.h"
/* Leader Object */

/*		
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
*/

/* TODO:

*/

//////////////////////////////////////////////////
//Leader Class Methods
//////////////////////////////////////////////////

//Constructor for Leader struct.
/*
Leader::Leader(int player)
{
	//TODO: Randomized stat generation.
	Player = player;
	Health = Drive = Luck = Charisma = Intelligence = 10;
	AiOwner = new AI(player);
	isAiPlayer = true;
	PlayerOwner = nullptr;
}
*/

Leader::Leader()
{

}

//////////////////////////////////////////////////
//AI Class Methods
//////////////////////////////////////////////////
AI::AI(int player)
{
	Player = player;
	Health = Drive = Luck = Charisma = Intelligence = 10;
	//AiOwner = new AI(player);
	isAiPlayer = true;
	//PlayerOwner = nullptr;
	//Constructor for an Owner struct. For starters we will set all of the AI stats to the same number.
	Aggro = Diplo = Econo = 0.f;
	for (int i = 0; i < 33; ++i)
	{
		int roll = rand() % 3;
		switch (roll)
		{
		case 0:
			Aggro += 3.f;
			break;
		case 1:
			Diplo += 3.f;
			break;
		case 2:
			Econo += 3.f;
			break;
		}
	}
}

/*TakeTurn() - The AI Turn taking method. 
	This will be the "main" method for the AI to decide what to do for their turn in the current territory. 
	We need the following things for a successful turn:
		1. A way to analyse the state of the territory.
			- Calculate the current standing for resources and troops.
				- If we don't have enough resources for our troops we need to get more.
			- Calculate the net income for Gold and Food.
				- If we are negative in either, we will take an appropriate economic action.
			- If we are stable on the above actions we will make a "Safe Action" which will be a randomly
			rolled action depending on the Aggro-Diplo-Econo (ADE) stats of the AI. 
		2. A way to analyse the state of the AI as a whole. (Stretch Goal)
			- The AI should be able to determine if any of their other holdings are in financial or military
			danger. 
			- The AI should be able to see potential weakpoints in the neighbors of their other holdings compared
			to their total military might.*/
int AI::TakeTurn(Territory *CurrentTerritory)
{
	int TurnResult = 1;
	while (TurnResult != 0)
	{
		TurnResult = GetStateSelf(CurrentTerritory);
		if (TurnResult != 0)
		{
			TurnResult = SafeAction(CurrentTerritory);
		}
		//TurnResult = 0;
	}
	return 0;
}

/* GetStateSelf - We will check the state of the current territory and make moves to try and remedy the state
if the state is poor. */
int AI::GetStateSelf(Territory *CurrentTerritory)
{
	float GoldIncome = 0, FoodIncome = 0;
	/*
	std::cout << "Territory: "<< CurrentTerritory->TerritoryNumber << std::endl;
	std::cout << "\tGold: " << CurrentTerritory->Gold << std::endl;
	std::cout << "\tFood: " << CurrentTerritory->Food << std::endl;
	std::cout << "\tSoldiers: " << CurrentTerritory->Soldiers << std::endl;
	*/
	// We check to see if the current state of the territory is unsatisfactory. 
	//Check the health of the leader. 
	if (CurrentTerritory->leader->Health - 3 <= 0)
	{
		DiplomaticAction(CurrentTerritory, 0); //Diplomatic Action: Rest
		return 0;
	}

	// Check to see if we have enough money to support the army. 
	if (CurrentTerritory->Gold < CurrentTerritory->Soldiers)
	{
		EconomicAction(CurrentTerritory, 5); //Economic Action: Build
		return 0;
	}

	//Check to see if we have enough food to support the army.
	if (CurrentTerritory->Food < CurrentTerritory->Soldiers / 2)
	{
		EconomicAction(CurrentTerritory, 3); //Economic Action: Grow
		return 0;
	}

	// We check to see if we have a positive net income for Gold and Food.
	GoldIncome -= (int)ceil(CurrentTerritory->Soldiers * .25);
	FoodIncome -= (int)ceil(CurrentTerritory->Soldiers * .25);
	//std::cout << "Soldier Upkeep: " << Soldiers * SoldierUpkeep << std::endl;
	//Town Upkeep
	FoodIncome -= (int)ceil(CurrentTerritory->Town * .50);

	//Natural Upkeep
	//Gold *= (Infrastructure / 100);
	//Food *= (Infrastructure / 100);
	//leader->Health--;

	/* Income */
	GoldIncome += (int)ceil(CurrentTerritory->Town * ((CurrentTerritory->Wealth / 100) * CurrentTerritory->TaxRate));
	FoodIncome += CurrentTerritory->Output;

	if (GoldIncome < 5)
	{
		EconomicAction(CurrentTerritory, 5); //Economic Action: Build
		return 0;
	}
	if (FoodIncome < 5)
	{
		EconomicAction(CurrentTerritory, 3); //Economic Action: Grow
		return 0;
	}
	return 1;
}

int AI::SafeAction(Territory *CurrentTerritory)
{
	const int MilitaryActionCount = 4; //Increase this number as more military directives arise. 
	const int DiplomaticActionCount = 4;
	const int EconomicActionCount = 7;
	int ActionResult = 1;
	if (isAiPlayer)
	{
		int ActionCategory = rand() % 99;
		
		//DEBUG: std::cout << "Action " << Action << std::endl;
		//If the action category roll is below the Aggro value we will attempt a military action.
		if (ActionCategory < Aggro)
		{
			int Action = floor(rand() % MilitaryActionCount);
			//std::cout << "Military Action Number: " << Action << std::endl;
			ActionResult = MilitaryAction(CurrentTerritory, Action);
			//std::cout << "Action Complete" << std::endl;
		}
		//If the action category is above Aggro but below Aggro+Diplo, we will attempt a diplomatic action.
		else if (ActionCategory < Aggro + Diplo)
		{
			int Action = floor(rand() % DiplomaticActionCount);
			//std::cout << "Diplomatic Action Number: " << Action << std::endl;
			ActionResult = DiplomaticAction(CurrentTerritory, Action);
			//std::cout << "Action Complete" << std::endl;
		}
		//Else we will attempt an economic action.
		else
		{
			int Action = floor(rand() % EconomicActionCount);
			//std::cout << "Economic Action Number: " << Action << std::endl;
			ActionResult = EconomicAction(CurrentTerritory, Action);
			//std::cout << "Action Complete" << std::endl;
		}
	}
	return ActionResult;
}

/* Military Actions: 
	0 - Move Troops: We can move troops from one territory to another.
	1 - Go to War: We can attack a neighboring territory to try and take control of it.
	2 - Train Troops: We can increase the skill value of our troops in the territory 
		to make them more effective soldiers.
	3 - Hire Troops: We can hire more troops to add to the numbers of our armies. 
	4 - Purchase Weapons: We can purchase weapons from the market and increase our army's
		effectiveness.
	5 - Give Troops: We can give gold or food to our troops to increase their morale
		making them less susceptible to bribes or rebellion.
	*/

/* MilitaryAction (Territory*, int) - Enacts a given military action*/
int AI::MilitaryAction(Territory * CurrentTerritory, int Action)
{
	int ActionResult = 1;
	switch (Action)
	{
	case 0:
		//ActionResult = CurrentTerritory->Move();
		break;
	case 1:
		ActionResult = mWarAction(CurrentTerritory);
		break;
	case 2:
		ActionResult = CurrentTerritory->Train();
		break;
	case 3:
		ActionResult = mHireAction(CurrentTerritory);
		break;
	case 4:
		//View();
		ActionResult = 1;
		std::cout << "Rolled View" << std::endl;
		break;
	default:
		std::cout << "Passed Turn" << std::endl;
		break;
	}
	return ActionResult;
}

/* mWarAction(Territory*) */
/* For territory A to war with Territory B, we need:
- Territory A must have soldiers to fight with.
- Territory B must not be owned by the owner of territory A.
- Territory B must not be owned by an ally of the owner of territory A.
- Territory B must be adjacent to Territory A.
- War costs one gold and one food for each soldier sent into battle.

We first need to acquire a target to war against. While a player may be able to pick one by number,
the AI will need to select a territory from the adjacency list at random.

A territory's combat value is: (Solders * (1+(Skill/100))) * (Soldiers/Arms)
With this formula, an army can only fight at full power if each soldier has a weapon.
After that, the army's combat value is modified by skill.

Variables:
- Iterator: A reference to the beginning of the adjacency vector.
- target: Pointer to the Territory being selected as the war target.
- AdjacentIndices: List of indices of the viable adjacent territories.
- IndexTracker: A counter to keep track of where in the adjacency list the iterator is.

TODO: Currently we are only selecting a random viable territory. The AI can be improved
by making view checks of the viable targets and selecting the weaker one. The search
criteria can also be limited based on these checks such as if there are no targets that
are sufficiently weak.

TL:DR War command for the AI is in alpha and will be improved later.*/
int AI::mWarAction(Territory * CurrentTerritory)
{
	//std::cout << "War Action Enter" << std::endl;
	//Declaring an iterator for the adjacency list. 
	std::vector<Territory *>::iterator Iterator = CurrentTerritory->Adjacency.begin();
	Territory * Target = nullptr;
	std::vector<int> AdjacentIndices;
	int IndexTracker = -1;
	int ActionResult = 1;
	int TerritorySelection = -1;
	//If there are no soldiers, we can't have war.
	if (CurrentTerritory->Soldiers < 1) { return 1; }

	//Iterate through the adjacency list and add the indices of viable territories to AdjacentIndices
	for (Iterator; Iterator != CurrentTerritory->Adjacency.end() - 1; ++Iterator)
	{
			//Increment the index tracker.
		IndexTracker++;
		Target = *Iterator;

		//If the current index has no Territory, go to next index.
		if (Target == nullptr)
		{
			continue;
		}

		//If the target is owned by the attacker, we cannot attack. Next index.
			//We are looking at the player number, not the Leader object specifically.
		if (Target->leader->Player == CurrentTerritory->leader->Player)
		{
			continue;
		}

		//If the target exists and is not owned by the current player, we will add it to the list of possible targets.
		AdjacentIndices.push_back(IndexTracker);
		//std::cout << "Targetting: " << target->TerritoryNumber << std::endl;
	}
	//std::cout << "War targets found..." << std::endl;
	/*Select a random territory from the list of viable ones to attack. If that territory is too strong,
	we will try and pick another one. If there are no Territories weak enough for us attack we will undo the action.*/
	while (AdjacentIndices.size() > 0 && ActionResult != 0)
	{
		TerritorySelection = rand() % AdjacentIndices.size();
		Target = CurrentTerritory->Adjacency[AdjacentIndices[TerritorySelection]];

		ActionResult = CalculateInvestment(CurrentTerritory, Target);
		if (ActionResult != 0)
		{
			AdjacentIndices.erase(AdjacentIndices.begin() + TerritorySelection);
		}
	}
	//std::cout << "War Action Exit" << std::endl;
	return 0;
}


/* CalculateInvestment(Territory *, Territory *) */
/* To calculate how much, if any, investment an AI Territory is sending into battle we must compare
our territory with the target and find a difference in strength.
Process:
	1. Compare the combat powers of the two territories (10 Gold Cost).
	2. If the target has a higher Combat Power, we will roll another action.
	3. If the target has a lower CP, we calculate how much stronger we are.
	3a. If the ratio is > 1.5, we don't need to devote all of our forces to win, so we will calculate the number of soldiers we need to win.
	4. Once we know our investment, we must see if we have enough money and food to support the war (1 Gold and 1 Food per soldier).
	5. If everything checks out, we kill them.

- A territory's combat value is: (Solders * (1+(Skill/100))) * (.2 + .8 *(Soldiers/Arms)
	- To define the formula: The number of soldiers multiplied by the Skill value of the territory
	further multiplied by the percentage of armed Soldiers. 
	-The optimal army setup then becomes Arms = Soldiers, then have as much skill as you can get.
- The defending territory gets a 25% boost to combat power due to static defenses (implied value).*/
int AI::CalculateInvestment(Territory *CurrentTerritory, Territory *Target)
{
	//std::cout << "Calculate Investment Enter" << std::endl;
	//Variable Declarations
	int SoldierInvestment, EnemySoldierInvestment, Survivors;
	float CombatRatio, ArmsRatio;

	//To view the target, we spend 10 Gold.
	CurrentTerritory->Gold -= 10;


	//We must keep At least 5 soldiers at home so nobody can immediately swoop in.
	SoldierInvestment = CurrentTerritory->Soldiers - 5;

	if (SoldierInvestment <= 0)
	{
		return 1;
	}

	//An enemy must fight with whatever their lowest resource is. 
	//Gold, Food, Soldiers, since the ratio is 1:1:1 the enemy can only fight with as much as their lowest resource.
	EnemySoldierInvestment = std::min(Target->Gold, Target->Food);
	EnemySoldierInvestment = std::min(EnemySoldierInvestment, Target->Soldiers);

	std::cout << std::endl;

	//If the enemy has no soldiers, we can win by default.
	if (EnemySoldierInvestment <= 0)
	{
		std::cout << "No enemies to fight." << std::endl;
		return 0;
	}

	//std::cout << "War calculations commencing..." << std::endl;
	//Calculate the combat power of the two territories.

	ArmsRatio = std::min(1, CurrentTerritory->Arms / CurrentTerritory->Soldiers);
	float CombatPowerA = SoldierInvestment  * (1 + (CurrentTerritory->Skill / 100.0)) * 
						(.2 + .8 * (ArmsRatio));

	ArmsRatio = std::min(1, Target->Arms / Target->Soldiers);
	float CombatPowerB = EnemySoldierInvestment * (1 + (Target->Skill / 100.0)) *
						(.2 + .8 * (ArmsRatio));

	CombatPowerB *= 1.25f;
	std::cout << "Battle" << std::endl;
	std::cout << "---------------------------------------------" << std::endl;
	//View();

	std::cout << std::endl;
	
	std::cout << "cpA: " << CombatPowerA << std::endl;
	std::cout << std::endl;

	std::cout << "cpB: " << CombatPowerB << std::endl;
	std::cout << "Battle Ratio: " << CombatPowerA / CombatPowerB << std::endl;

	std::cout << "---------------------------------------------" << std::endl;

	//If the enemy is stronger than us, undo action. 
	if (CombatPowerB > CombatPowerA)
	{
		std::cout << "The enemy is stronger than we are." << std::endl;
		return 1;
	}

	CombatRatio = CombatPowerA / CombatPowerB;
	//std::cout << "War calculations finished..." << std::endl;
	//If we are more than 50% stronger than the enemy, we don't have to send in our full army to win. So we will try and cut back. 
	if (CombatRatio > 1.5)
	{
		//We will send a little over just enough soldiers to win the encounter. 
		SoldierInvestment = ceil(SoldierInvestment / (CombatRatio - .2f));
	}

	//If we don't have the resources to send out these soldiers, then the risk is too great and we will try something else. 
	if (CurrentTerritory->Gold > SoldierInvestment && CurrentTerritory->Food > SoldierInvestment)
	{
		//std::cout << "Going to war..." << std::endl;
		CurrentTerritory->War(Target, SoldierInvestment);
		//std::cout << "Went to war..." << std::endl;
		return 0;
	}
	else
	{
		std::cout << "Not enough resources." << std::endl;
		return 1;
	}
	//std::cout << "Calculate Investment End" << std::endl;
	std::cout << std::endl;
	
}


/* HireAction(Territory *) - Figure out how many soldiers we can safely hire. */
int AI::mHireAction(Territory * CurrentTerritory)
{
	//std::cout << "Hire Action Enter" << std::endl;
	// First set aside enough Gold to support our current Soldier count
	int SpendableGold = CurrentTerritory->Gold - CurrentTerritory->Soldiers;
	int RandomHireAmount, HireResult;
	
	/* Divide remaining Gold by two (floor) since we will need at least an equal amount of Gold
	since we need to support the new troops as well. */
	SpendableGold = (int)floor(SpendableGold / 2);

	// For now, pick a random number between 0 - ((Gold/2)/3) and call the Territory's Hire function with the result.
		// ((Gold/2)/3) is half the remaining gold value divided by the cost of each soldier. 
	SpendableGold = (int)floor(SpendableGold / 3);
	//If we don't give ourselves enough gold to spend, we'll try something else. 
	if ((int)floor(SpendableGold / 2.0) <= 0)
	{
		return 1;
	}
	RandomHireAmount = floor(rand() % (int)floor(SpendableGold / 2.0) + (int)floor(SpendableGold / 2.0));


	HireResult = CurrentTerritory->Hire(RandomHireAmount);
	//std::cout << "Hire Action Exit" << std::endl;
	return HireResult;
}

/* Diplomatic Actions: 
	0 - Rest: Gives up the turn in this territory to allow the leader to regain health.
	1 - Grant (Stretch): Changes the priorities of the AI for this territory between military and economy.
	2 - Pact: Attempts to create a Non-Agression Pact between this territory and another. 
	3 - Bribe Territory: Bribes citizens in another territory to lower their
		morale and loyalty, but increasing their wealth.
*/

/* DiplomaticAction (Territory*, int) - Enacts a given diplomatic action*/
int AI::DiplomaticAction(Territory * CurrentTerritory, int Action)
{
	int ActionResult = 1;
	switch (Action)
	{
	case 0:
		ActionResult = CurrentTerritory->Rest();
		break;
	case 1:
		//ActionResult = CurrentTerritory->Grant();
		break;
	case 2:
		//ActionResult = CurrentTerritory->Pact();
		break;
	case 3:
		//ActionResult = CurrentTerritory->Bribe();
		break;
	default:
		std::cout << "Passed Turn" << std::endl;
	}
	return ActionResult;
}

/* Economic Actions:
	0 - Change Taxes: Adjust the tax value between 0.0 - 1.0. Higher values mean higher taxes.
	1 - Send Resources: Send resources to another territory under the same leadership. 
	2 - Build Infrastructure: Spend gold to improve infrastructure value to reduce upkeep values.
	3 - Grow Food: Spend gold to improve farms and increase food output. 
	4 - Sell Food: Sell surplus food for Gold. 
	5 - Build Town: Spend gold to improve the town and increase gold income. 
	6 - Give Citizens: Give surplus gold or food to citizens to increase Wealth or Loyalty respectively.*/

/* EconomicAction(Territory*, int) - Enacts a given economic action*/
int AI::EconomicAction(Territory * CurrentTerritory, int Action)
{
	int ActionResult = 1;
	switch (Action)
	{
	case 0:
		//ActionResult = CurrentTerritory->Tax();
		break;
	case 1:
		//ActionResult = CurrentTerritory->Send();
		break;
	case 2:
		//ActionResult = CurrentTerritory->Dam();
		break;
	case 3:
		ActionResult = eGrowAction(CurrentTerritory);
		break;
	case 4:
		//ActionResult = CurrentTerritory->Trade();
		break;
	case 5:
		ActionResult = eBuildAction(CurrentTerritory);
		break;
	case 6:
		ActionResult = eGiveAction(CurrentTerritory);
		break;
	default:
		std::cout << "Passed Turn" << std::endl;
	}
	return ActionResult;
}
int AI::eGrowAction(Territory * CurrentTerritory)
{
	//First subtract the number of Soldiers from the Gold amount so we don't weaken our military.
	int InvestmentAmount = CurrentTerritory->Gold - CurrentTerritory->Soldiers;
	int RandomInvestmentAmount, BuildResult;

	//If we don't have gold to invest in a Build action we will have to sell Food.
	if (InvestmentAmount <= 10)
	{
		//TODO finish Trade action.
		//Try an economic Give action.
		if (CurrentTerritory->Food - CurrentTerritory->Soldiers >= 10)
		{
			eGiveAction(CurrentTerritory);
			return 0;
		}
	}
	//For now, select a random amount between Half the InvestmentAmount - InvestmentAmount and call the Territory's Build action.
		//Update: Increased minimum since small numbers were painfully insignificant which came out to wasted turns overall.
	RandomInvestmentAmount = rand() % (int) floor(InvestmentAmount/2.0) + (int) floor(InvestmentAmount/2.0);

	//std::cout << "Investment Amount: " << RandomInvestmentAmount << std::endl;

	BuildResult = CurrentTerritory->Grow(RandomInvestmentAmount);
	return BuildResult;
}
/* BuildAction(Territory *) - AI Method to decide how much Gold to spend on a Build action.*/
int AI::eBuildAction(Territory * CurrentTerritory)
{
	//std::cout << "Build Action Enter" << std::endl;
	//First subtract the number of Soldiers from the Gold amount so we don't weaken our military.
	int InvestmentAmount = CurrentTerritory->Gold - CurrentTerritory->Soldiers;
	int RandomInvestmentAmount, BuildResult;

	//If we don't have gold to invest in a Build action we will have to sell Food.
	if (InvestmentAmount <= 10)
	{
		//TODO finish Trade action.
		//Try an economic Give action.
		if (CurrentTerritory->Food - CurrentTerritory->Soldiers >= 10)
		{
			eGiveAction(CurrentTerritory);
			return 0;
		}
	}
	//For now, select a random amount between 0 - InvestmentAmount and call the Territory's Build action.
	RandomInvestmentAmount = rand() % (int)floor(InvestmentAmount / 2.0) + (int)floor(InvestmentAmount / 2.0);

	//std::cout << "Investment Amount: " << RandomInvestmentAmount << std::endl;

	BuildResult = CurrentTerritory->Build(RandomInvestmentAmount);
	return BuildResult;
}

/* eGiveAction(Territory *) - AI Method to decide how much Food to spend on an economic Give action.*/
int AI::eGiveAction(Territory * CurrentTerritory)
{
	//First subtract the number of Soldiers from the Food amount so we don't weaken our military.
	int InvestmentAmount = CurrentTerritory->Food - CurrentTerritory->Soldiers;
	int RandomInvestmentAmount, BuildResult;

	if (InvestmentAmount <= 10)
	{
		//TODO finish Trade action.
		//Try a Build action.

		if (CurrentTerritory->Gold - CurrentTerritory->Soldiers >= 10)
		{
			eBuildAction(CurrentTerritory);
			return 0;
		}
		
	}
	//For now, select a random amount between 0 - InvestmentAmount and call the Territory's Build action.
	RandomInvestmentAmount = rand() % (int)floor(InvestmentAmount / 2.0) + (int)floor(InvestmentAmount / 2.0);

	//std::cout << "Investment Amount: " << RandomInvestmentAmount << std::endl;

	BuildResult = CurrentTerritory->Give(1, RandomInvestmentAmount);
	return BuildResult;

	return 0;
}

//DEBUG Methods
void AI::AiStatistics()
{
	std::cout << "Leader Attributes:" << std::endl;
	std::cout << "\tHealth " << Health << std::endl;
	std::cout << "\tDrive " << Drive << std::endl;
	std::cout << "\tLuck " << Luck << std::endl;
	std::cout << "\tCharisma " << Charisma << std::endl;
	std::cout << "\tIntelligence " << Intelligence << std::endl;

	std::cout << "AI Attributes:" << std::endl;
	std::cout << "\tAggro " << Aggro << std::endl;
	std::cout << "\tDiplo " << Diplo << std::endl;
	std::cout << "\tEcono " << Econo << std::endl;
	std::cout << std::endl;
}

void AI::TerritoryStatistics(Territory *CurrentTerritory)
{
	CurrentTerritory->DisplayState();
	std::cout << std::endl;
	std::cout << std::endl;
}


//////////////////////////////////////////////////
//Player Class Methods
//////////////////////////////////////////////////
int User::TakeTurn(Territory *CurrentTerritory)
{
	std::cout << "Player Turn" << std::endl;

	return 0;
}
