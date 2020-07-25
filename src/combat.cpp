/* #include "..\include\combat.h" */
#include "..\include\game.h"
#include "..\include\move_impl.h"
#include "..\include\embed\cassandra_combat_sprite.h"
#include "..\include\sprite.h"

UMap<Actor *, Vec<MoveEffect> > CombatData::effects;
UMap<Actor *, StatusEffects> CombatData::statusEffects;
MemPool CombatData::diffMemPool = CreateMemPool(DIFF_MEM_SIZE);

CombatPortraits CombatData::portraits;
Texture CombatData::halo;
Vec<bool> CombatData::hasMoveChosen;

UMap<Actor *, Sprite> CombatData::actorSprites;
bool CombatData::moveAnimPlaying = false;

Vec<Actor *> CombatData::playerAlive;
Vec<Actor *> CombatData::botAlive;
Vec<Actor *> CombatData::potTargets;
Vec<Actor *> CombatData::chosenTargets;

Vec<CasterTargetsPair> CombatData::ctps;
Vec<CasterTargetsPair *> CombatData::ctpsPtrs;

unsigned char CombatData::focus = 0;

/* bool CombatData::canAssign */

CasterTargetsPair::CasterTargetsPair(Actor *c, struct Move *m, const Vec<Actor *> &t){
	caster = c;
	move = m;
	targets = t;
}


void StartCombat(Game &game){
	CombatData *cbtData = game.cbtData;

	cbtData->focus = 0;

	CombatData::portraits.toggles.resize(4);
	CombatData::portraits.toggles[0].active = true;
	//FIXME: Use the texture manager instead of loading them with each state change
	CombatData::portraits.toggles[0].texture = LoadTexture("../img/cassandra_dev.png");
	CombatData::portraits.toggles[1].texture = LoadTexture("../img/gordon_dev.png");
	CombatData::portraits.toggles[2].texture = LoadTexture("../img/lou_dev.png");
	CombatData::portraits.toggles[3].texture = LoadTexture("../img/persy_dev.png");
	cbtData->halo = LoadTexture("../img/portrait_halo.png");

	for(size_t i = 0; i < 4; ++i){
		ImageToggle *toggle = &cbtData->portraits.toggles[i];
		toggle->bounds =
			(Rectangle){PORTRAIT_PADDING, (float)(i * PORTRAIT_SEPARATION + PORTRAIT_PADDING), PORTRAIT_WIDTH, PORTRAIT_WIDTH};
	}

	cbtData->playerTeam = &game.player.team;

	CombatData::actorSprites[cbtData->playerTeam->members[0]] = LoadSprite(cassandra_combat_anim); 
	//FIXME: kind of hacky, need to store looping in the anim file and embedded file
	auto &cass_spr = CombatData::actorSprites[cbtData->playerTeam->members[0]];
	TextureManager::textures["../img/cassandra_combat.png"] = LoadTexture("../img/cassandra_combat.png")	;
	cass_spr.texture = &TextureManager::textures["../img/cassandra_combat.png"];
	cass_spr.pos = {440, 360};
	cass_spr.anims["Combat Idle"].looping = true;
	cass_spr.playAnimation("Combat Idle");

	CreateGoons(*cbtData);
	TeamsSetup(*cbtData);

	CombatData::effects.clear();
	for(auto &it : CombatData::playerAlive)
		CombatData::effects.emplace(it, Vec<MoveEffect>());
	for(auto &it : CombatData::botAlive)
		CombatData::effects.emplace(it, Vec<MoveEffect>());

	CombatData::statusEffects.clear();
	for(auto &it : CombatData::playerAlive)
		CombatData::statusEffects.emplace(it, StatusEffects());
	for(auto &it : CombatData::botAlive)
		CombatData::statusEffects.emplace(it, StatusEffects());

	MoveButtonsSetup(*cbtData);

	cbtData->chosenMove = nullptr;
	cbtData->aiMakePairs = true;

	cbtData->hasMoveChosen.resize(CombatData::playerAlive.size());
	for(unsigned int i = 0; i < cbtData->hasMoveChosen.size(); ++i)
		cbtData->hasMoveChosen[i] = false;

	cbtData->targetSelectedList.bounds = (Rectangle){256, 64, 192, 192};
	cbtData->targetAliveList.bounds = (Rectangle){256 + 192 + 128, 64, 192, 192};

	//TODO: Also call this when returning to phase 1
	GetAffordableMoves(*cbtData->playerTeam->members[game.cbtData->focus], cbtData->affordable);

	game.justEnteredState = true;
}


void AIMakeCTPs(CombatData &cbtD){
	for(unsigned int i = 0; i < CombatData::botAlive.size(); ++i){ //FIXME: turn into for-range
		Vec<Actor *> targets;
		struct Move *move = CombatData::botAlive[i]->moves[0];

		if(move->maxTargets == TARGET_ALL_ACTORS){
			targets.reserve(CombatData::playerAlive.size() + CombatData::botAlive.size());

			for(auto &it : CombatData::playerAlive)
				targets.push_back(it);
			for(auto &it : CombatData::botAlive)
				targets.push_back(it);
		}
		else if(move->maxTargets == TARGET_SELF){
			targets.push_back(CombatData::botAlive[i]);
		}
		else if(move->isFriendly){
			for(int j = 0; j < move->maxTargets; ++j) //Take the first actors it can
				targets.push_back(CombatData::botAlive[j]);
		}
		else if(move->isHostile){
			for(int j = 0; j < move->maxTargets; ++j) //Take the first actors it can
				targets.push_back(CombatData::playerAlive[j]);
		}

		CombatData::ctps.push_back(CasterTargetsPair(CombatData::botAlive[i], move, targets));
	}
	cbtD.aiMakePairs = false;
}


void SelectMoves(CombatData &cbtD){
	/* Actor *actor = CombatData::playerAlive[cbtD.focus]; //FIXME: won't work if a character dies! */
	Actor *actor = cbtD.playerTeam->members[cbtD.focus];

	for(size_t i = 0; i < NUM_ACTOR_MOVES; ++i){
		Update(cbtD.moveButtons[i]);
		if(cbtD.moveButtons[i].state == GuiControlState::GUI_STATE_PRESSED && cbtD.affordable){
			bool found = false;
			for(size_t j = 0; j < NUM_ACTOR_MOVES; ++j){
				if(actor->moves[i] == cbtD.affordable[j]){ //If the selection is in the affordable move list
					found = true;
					break;
				}
			}
			if(found){
				cbtD.chosenMove = actor->moves[i];
				cbtD.canAssign = true;
			}
			break;
		}
	}
}


void GetAffordableMoves(Actor &actor, struct Move **affordable){
	for(size_t i = 0; i < NUM_ACTOR_MOVES; ++i)
		affordable[i] = nullptr;

	unsigned int fill = 0;
	for(size_t i = 0; i < NUM_ACTOR_MOVES; ++i){
		if(actor.moves[i]){
			if(actor.moves[i]->cost <= actor.remMP){
				affordable[fill] = actor.moves[i];
				fill++;
			}
		}
	}
}


void AssignTargets(CombatData &cbtD){
	//Make a macro-loop
	//TODO: Make this into a function, (AssignTargetsSetup())
	if(!cbtD.begunAssigning){
		//TODO: Add checks for maxTargets == all enemies
		//TODO: Add checks for maxTargets == all actors
		ResetSelectAvailList(cbtD);

		//Get potential targets
		if(cbtD.chosenMove->isFriendly){
			for(auto it : CombatData::playerAlive)
				CombatData::potTargets.push_back(it);
		}
		if(cbtD.chosenMove->isHostile){
			for(auto it : CombatData::botAlive)
				CombatData::potTargets.push_back(it);
		}
		
		for(auto &it : CombatData::potTargets) //TODO: Make this into a function, (GetAliveList())
			cbtD.targetAliveList.text.push_back(it->name);

		SetupListView(cbtD.targetAliveList);
		SetupListView(cbtD.targetSelectedList);

		cbtD.begunAssigning = true;
	}
	if((int)CombatData::chosenTargets.size() < cbtD.chosenMove->maxTargets){
		int prevActiveAlive = cbtD.targetAliveList.active;
		int prevActiveSelect = cbtD.targetSelectedList.active;
		Update(cbtD.targetAliveList);
		Update(cbtD.targetSelectedList);

		if(prevActiveAlive != -1 && cbtD.targetAliveList.active != prevActiveAlive)
			AddFromAliveList(cbtD, prevActiveAlive);
		if(prevActiveSelect != -1 && cbtD.targetSelectedList.active != prevActiveSelect)
			RemoveFromSelectedList(cbtD, prevActiveSelect);
	}
	else{
		cbtD.canMakePair = true;
		cbtD.canAssign = false;
	}
}


void AssignTargetsSetup(CombatData &cbtD){
	//TODO: Add checks for maxTargets == all enemies
	//TODO: Add checks for maxTargets == all actors
	if(cbtD.chosenMove->maxTargets == TARGET_ALL_ACTORS){ //
		for(auto &it : CombatData::playerAlive)
			CombatData::chosenTargets.push_back(it);
		for(auto &it : CombatData::botAlive)
			CombatData::chosenTargets.push_back(it);
		
		cbtD.targetAliveList.text.push_back("ALL");
	}
	ResetSelectAvailList(cbtD);

	//Get potential targets
	if(cbtD.chosenMove->isFriendly){
		for(auto it : CombatData::playerAlive)
			CombatData::potTargets.push_back(it);
	}
	if(cbtD.chosenMove->isHostile){
		for(auto it : CombatData::botAlive)
			CombatData::potTargets.push_back(it);
	}
	
	for(auto &it : CombatData::potTargets) //TODO: Make this into a function, (GetAliveList())
		cbtD.targetAliveList.text.push_back(it->name);

	//Populate list views with
	SetupListView(cbtD.targetAliveList);
	SetupListView(cbtD.targetSelectedList);

	cbtD.begunAssigning = true;
}


void MakeCTP(CombatData &cbtD){
	Actor *&actor = cbtD.playerTeam->members[cbtD.focus];
	CombatData::ctps.emplace( CombatData::ctps.begin(), CasterTargetsPair(actor, cbtD.chosenMove, CombatData::chosenTargets));
	cbtD.dispTargetLists = false;
	cbtD.begunAssigning = false;
	cbtD.hasMoveChosen[cbtD.focus] = true;

	cbtD.canMakePair = false;

}

//TODO: make me more elegant?
void AddFromAliveList(CombatData &cbtD, int &prevActive){
	CombatData::chosenTargets.push_back(CombatData::potTargets[prevActive]);
	cbtD.targetSelectedList.text.push_back(cbtD.targetAliveList.text[prevActive]);
	SetupListView(cbtD.targetSelectedList);
}


void RemoveFromSelectedList(CombatData &cbtD, int &prevActive){
	CombatData::chosenTargets.erase(CombatData::chosenTargets.begin() + prevActive);
	cbtD.targetSelectedList.text.erase(cbtD.targetSelectedList.text.begin() + prevActive);
	SetupListView(cbtD.targetSelectedList);
}


void HandleInventoryPortraits(CombatData &cbtD){ //TODO: rename HandleCombatPortraits()
	bool *isToggled = new bool[CombatData::playerAlive.size()]; //Keep track of previous activation

	for(size_t i = 0; i < CombatData::playerAlive.size(); ++i)
		isToggled[i] = CombatData::portraits.toggles[i].active;

	for(size_t i = 0; i < CombatData::playerAlive.size(); ++i){
		bool update = Update(CombatData::portraits.toggles[i]); //a group toggling

		if(isToggled[i] && CombatData::hasMoveChosen[i]){

			for(size_t j = 0; j < CombatData::playerAlive.size() && CombatData::hasMoveChosen[i]; ++j){ //Cycle through characters who have yet to select moves
				i = (i + 1) % CombatData::playerAlive.size();
				CombatData::portraits.toggles[i].active = true;
				CombatData::focus = i;
			}

			for(size_t j = 0; j < CombatData::playerAlive.size(); ++j){
				if(j != i) //Turn the others in the toggle group off
					CombatData::portraits.toggles[j].active = false;
			}
			break;
		}

		if(update != isToggled[i]){
			if(update){

				CombatData::focus = i;
				for(size_t j = 0; j < 4; ++j){
					if(j != i) //Turn the others in the toggle group off
						CombatData::portraits.toggles[j].active = false;
				}
				break;
			}
			else
				CombatData::portraits.toggles[i].active = true;
		}
	}
	delete[] isToggled;
}


void MoveButtonsSetup(CombatData &cbtD){
	Vector2 pos = {96, 448};
	for(size_t i = 0; i < NUM_ACTOR_MOVES; ++i){
		cbtD.moveButtons[i].bounds = (Rectangle){pos.x + (176 * (i % 4)), pos.y + (80 * (i / 4)), 160, 64};
	}

	MoveButtonsTextSetup(cbtD);
}


void MoveButtonsTextSetup(CombatData &cbtD){
	Actor *actor = cbtD.playerTeam->members[cbtD.focus];
	for(size_t i = 0; i < NUM_ACTOR_MOVES; ++i){
		if(actor->moves[i])
			cbtD.moveButtons[i].text = actor->moves[i]->name;
		else
			break; //early exit b/c moves should always be valid until nullptrs at the end
	}
}


void TeamsSetup(CombatData &cbtD){
	for(unsigned int i = 0; i < cbtD.playerTeam->members.size(); ++i){
		Actor *actor = cbtD.playerTeam->members[i];
		if(!IsDead(*actor))
			CombatData::playerAlive.push_back(actor);
	}
	for(unsigned int i = 0; i < cbtD.botTeam->members.size(); ++i){
		Actor *actor = cbtD.botTeam->members[i];
		if(!IsDead(*actor))
			CombatData::botAlive.push_back(actor);
	}
}


void DisableUnusedButtons(CombatData &cbtD){
	Actor *actor = cbtD.playerTeam->members[cbtD.focus];
	for(size_t i = 0; i < NUM_ACTOR_MOVES; ++i){
		if(actor->moves[i])
			cbtD.moveButtons[i].state = GuiControlState::GUI_STATE_NORMAL;
		else
			cbtD.moveButtons[i].state = GuiControlState::GUI_STATE_DISABLED;
	}
}


bool CanExecMoves(CombatData &cbtD){
	Vec<Actor *> allInCombat;
	for(auto &it : CombatData::playerAlive)
		allInCombat.push_back(it);
	for(auto &it : CombatData::botAlive)
		allInCombat.push_back(it);

	//O(n^2), 
	//TODO: find a better way of doing this...
	for(auto &it : allInCombat){
		bool found = false;
		for(auto &jt : CombatData::ctps){
			if(it == jt.caster){
				found = true;
				break;
			}
		}
		if(!found)
			return false;
	}

	return true;
}


bool LessThanCTP(CasterTargetsPair *first, CasterTargetsPair *second){
	//TODO: DONT FORGET SLOWING
	int priorities[2] = {first->move->priority, second->move->priority};

	for(auto &it : CombatData::effects[first->caster]){
		if(it.id == QUICKSILVER)
			priorities[0] -= (int)GetEffectPrimaryBuff(it.id);
	}
	for(auto &it : CombatData::effects[second->caster]){
		if(it.id == QUICKSILVER)
			priorities[1] -= (int)GetEffectPrimaryBuff(it.id);
	}

	return priorities[0] < priorities[1];
}


//NOTE: June 9, 2020
//This function will try to execute within one frame, this does not account for animations/sound
//These will be added at a later date, hopefully... So in order to keep the same relative flow control
//Create a macro-loop bounded by ctpsPtrs.size(), and iterate on confirmation that a move has finished
//animating / playing audio, the confirmation will most likely be a boolean with the combat data
void ExecMoves(CombatData &cbtD){
	if(!cbtD.executingMoves){
		CombatData::ctpsPtrs.resize(0);

		for(auto &it : CombatData::ctps)
			CombatData::ctpsPtrs.push_back(&it);

		std::sort(CombatData::ctpsPtrs.begin(), CombatData::ctpsPtrs.end(), LessThanCTP); //sort on priority 

		cbtD.execIndex = 0;
		cbtD.executingMoves = true;
	}

	if(cbtD.executingMoves){
		for(size_t i = 0; i < CombatData::ctpsPtrs.size(); ++i){
			ExecMove(*CombatData::ctpsPtrs[i]);
			cbtD.execIndex++;
			cbtD.executingMoves = cbtD.execIndex < CombatData::ctpsPtrs.size();
		}

		cbtD.aiMakePairs = true;
		cbtD.canAssign = true;
	}

	/* if(!(cbtD.execIndex < CombatData::ctpsPtrs.size())){ */
	/* 	//TODO: Tick effects here... */
	/* } */
}


void ExecMove(CasterTargetsPair &ctp){
	for(auto &it : ctp.targets){
		switch(ctp.move->id){
		case IGNITE:
			MoveImplementation::IGNITE(*it, *static_cast<CasterMove *>(&ctp));
			CombatData::actorSprites[ctp.caster].playAnimation("Use Physical");
			/* CombatData::actorSprites[it].playAnimation("Hurt Physical", 12); */
			CombatData::moveAnimPlaying = true;
			break;
		case BLAZE:
			MoveImplementation::BLAZE(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case FIREBALL:
			MoveImplementation::FIREBALL(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case EXPLOSION:
			MoveImplementation::EXPLOSION(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case FIREBEAM:
			MoveImplementation::FIREBEAM(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case FLAMESWORD:
			MoveImplementation::FLAMESWORD(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case FLAREKICK:
			MoveImplementation::FLAREKICK(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case FIREWALL:
			MoveImplementation::FIREWALL(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case SEARINGFLESH:
			MoveImplementation::SEARINGFLESH(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case MOLTENMETEOR:
			MoveImplementation::MOLTENMETEOR(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case WILDFIRE:
			MoveImplementation::WILDFIRE(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case RAZEEARTH:
			MoveImplementation::RAZEEARTH(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case FROMTHEASHES:
			MoveImplementation::FROMTHEASHES(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case SUPERNOVA:
			MoveImplementation::SUPERNOVA(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case WATERWHIP:
			MoveImplementation::WATERWHIP(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case TORRENT:
			MoveImplementation::TORRENT(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case TIDALWAVE:
			MoveImplementation::TIDALWAVE(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case COALESCE:
			MoveImplementation::COALESCE(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case WHIRLPOOL:
			MoveImplementation::WHIRLPOOL(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case HAILSTORM:
			MoveImplementation::HAILSTORM(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case AQUAPRISON:
			MoveImplementation::AQUAPRISON(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case RAINSTORM:
			MoveImplementation::RAINSTORM(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case DEWPOINT:
			MoveImplementation::DEWPOINT(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case FLASHFREEZE:
			MoveImplementation::FLASHFREEZE(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case RIPTIDE:
			MoveImplementation::RIPTIDE(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case VAPORVORTEX:
			MoveImplementation::VAPORVORTEX(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case HELLBROTH:
			MoveImplementation::HELLBROTH(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case TSUNAMI:
			MoveImplementation::TSUNAMI(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case SEEDSHOT:
			MoveImplementation::SEEDSHOT(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case ACORNASSAULT:
			MoveImplementation::ACORNASSAULT(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case SAPPINGSTEMS:
			MoveImplementation::SAPPINGSTEMS(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case NOURISH:
			MoveImplementation::NOURISH(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case LEAFLANCE:
			MoveImplementation::LEAFLANCE(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case FUNGALSPORES:
			MoveImplementation::FUNGALSPORES(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case BARBEDHUSK:
			MoveImplementation::BARBEDHUSK(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case GRASSBLADES:
			MoveImplementation::GRASSBLADES(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case PETALPIKE:
			MoveImplementation::PETALPIKE(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case UNDERGROWTH:
			MoveImplementation::UNDERGROWTH(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case SYMBIOSIS:
			MoveImplementation::SYMBIOSIS(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case ENSNARINGVINE:
			MoveImplementation::ENSNARINGVINE(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case RANCIDROSES:
			MoveImplementation::RANCIDROSES(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case DRYADSCURSE:
			MoveImplementation::DRYADSCURSE(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case ZAP:
			MoveImplementation::ZAP(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case CHARGE:
			MoveImplementation::CHARGE(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case DISCHARGE:
			MoveImplementation::DISCHARGE(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case LIGHTNINGBOLT:
			MoveImplementation::LIGHTNINGBOLT(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case DAZZLINGLIGHTS:
			MoveImplementation::DAZZLINGLIGHTS(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case ELECTROSTORM:
			MoveImplementation::ELECTROSTORM(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case LIGHTNINGKICK:
			MoveImplementation::LIGHTNINGKICK(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case SPARKINGSKIN:
			MoveImplementation::SPARKINGSKIN(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case SUPERCONDUCT:
			MoveImplementation::SUPERCONDUCT(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case STATICSHOCK:
			MoveImplementation::STATICSHOCK(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case EVANESCENTFIELD:
			MoveImplementation::EVANESCENTFIELD(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case HIGHVOLTAGE:
			MoveImplementation::HIGHVOLTAGE(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case MJOLNIR:
			MoveImplementation::MJOLNIR(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case CLOSEDCIRUIT:
			MoveImplementation::CLOSEDCIRUIT(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case IRONSPIKES:
			MoveImplementation::IRONSPIKES(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case CANNONBALL:
			MoveImplementation::CANNONBALL(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case SHARPEN:
			MoveImplementation::SHARPEN(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case SHATTERSHRAPNEL:
			MoveImplementation::SHATTERSHRAPNEL(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case BALLANDCHAIN:
			MoveImplementation::BALLANDCHAIN(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case QUICKSILVER:
			MoveImplementation::QUICKSILVER(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case COPPERCUTLASS:
			MoveImplementation::COPPERCUTLASS(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case RAZORWIRE:
			MoveImplementation::RAZORWIRE(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case MIDASTOUCH:
			MoveImplementation::MIDASTOUCH(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case CHROMEPLATED:
			MoveImplementation::CHROMEPLATED(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case MAGNETIZE:
			MoveImplementation::MAGNETIZE(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case BRASSKNUCKLES:
			MoveImplementation::BRASSKNUCKLES(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case ALLOYASSAULT:
			MoveImplementation::ALLOYASSAULT(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case PIERCINGPLATINUM:
			MoveImplementation::PIERCINGPLATINUM(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case HURLROCK:
			MoveImplementation::HURLROCK(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case STONESPEAR:
			MoveImplementation::STONESPEAR(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case FORTIFY:
			MoveImplementation::FORTIFY(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case QUICKSAND:
			MoveImplementation::QUICKSAND(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case ENTOMB:
			MoveImplementation::ENTOMB(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case CRUSH:
			MoveImplementation::CRUSH(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case TREMOR:
			MoveImplementation::TREMOR(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case ROCKSLIDE:
			MoveImplementation::ROCKSLIDE(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case ROLLINGBOULDER:
			MoveImplementation::ROLLINGBOULDER(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case SAPPHIRESTRIKE:
			MoveImplementation::SAPPHIRESTRIKE(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case RUBYRUSH:
			MoveImplementation::RUBYRUSH(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case GARNETGAZE:
			MoveImplementation::GARNETGAZE(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case EMERALDEDGE:
			MoveImplementation::EMERALDEDGE(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case OBSIDIANONSLAUGHT:
			MoveImplementation::OBSIDIANONSLAUGHT(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case ECTORAY:
			MoveImplementation::ECTORAY(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case TORMENT:
			MoveImplementation::TORMENT(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case CHAOSCLAW:
			MoveImplementation::CHAOSCLAW(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case PHANTOMWALTS:
			MoveImplementation::PHANTOMWALTS(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case GRAVETENDER:
			MoveImplementation::GRAVETENDER(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case MINDINVASION:
			MoveImplementation::MINDINVASION(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case BINDINGPAIN:
			MoveImplementation::BINDINGPAIN(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case PLAGUE:
			MoveImplementation::PLAGUE(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case WICKEDHEX:
			MoveImplementation::WICKEDHEX(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case PETRIFYINGWAIL:
			MoveImplementation::PETRIFYINGWAIL(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case DECAY:
			MoveImplementation::DECAY(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case VIVIDNIGHTMARE:
			MoveImplementation::VIVIDNIGHTMARE(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case ETHEREALFOG:
			MoveImplementation::ETHEREALFOG(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case NOXIOUSVOID:
			MoveImplementation::NOXIOUSVOID(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case FLASHFRICTION:
			MoveImplementation::FLASHFRICTION(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case SCORCHINGHEAT:
			MoveImplementation::SCORCHINGHEAT(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case DRAGONBREATH:
			MoveImplementation::DRAGONBREATH(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case ICARUSINFERNO:
			MoveImplementation::ICARUSINFERNO(*it, *static_cast<CasterMove *>(&ctp));
			//TODO: play the associated anims
			break;
		case REND:
			/* MoveImplementation::REND(*it, *static_cast<CasterMove *>(&ctp)); */
			//TODO: play the associated anims
			break;
		case SIRENSTEAR:
			/* MoveImplementation::SIRENSTEAR(*it, *static_cast<CasterMove *>(&ctp)); */
			//TODO: play the associated anims
			break;
		case CHILLINGBLAST:
			/* MoveImplementation::CHILLINGBLAST(*it, *static_cast<CasterMove *>(&ctp)); */
			//TODO: play the associated anims
			break;
		case DELUGE:
			/* MoveImplementation::DELUGE(*it, *static_cast<CasterMove *>(&ctp)); */
			//TODO: play the associated anims
			break;
		case NEEDLE:
			/* MoveImplementation::NEEDLE(*it, *static_cast<CasterMove *>(&ctp)); */
			//TODO: play the associated anims
			break;
		case BLOOM:
			/* MoveImplementation::BLOOM(*it, *static_cast<CasterMove *>(&ctp)); */
			//TODO: play the associated anims
			break;
		case TREECLEAVER:
			/* MoveImplementation::TREECLEAVER(*it, *static_cast<CasterMove *>(&ctp)); */
			//TODO: play the associated anims
			break;
		case VENOMCOATING:
			/* MoveImplementation::VENOMCOATING(*it, *static_cast<CasterMove *>(&ctp)); */
			//TODO: play the associated anims
			break;
		case SURGE:
			/* MoveImplementation::SURGE(*it, *static_cast<CasterMove *>(&ctp)); */
			//TODO: play the associated anims
			break;
		case OVERLOAD:
			/* MoveImplementation::OVERLOAD(*it, *static_cast<CasterMove *>(&ctp)); */
			//TODO: play the associated anims
			break;
		case IONSTRIKE:
			/* MoveImplementation::IONSTRIKE(*it, *static_cast<CasterMove *>(&ctp)); */
			//TODO: play the associated anims
			break;
		case PLASMAPULSE:
			/* MoveImplementation::PLASMAPULSE(*it, *static_cast<CasterMove *>(&ctp)); */
			//TODO: play the associated anims
			break;
		case SCRAPSLUG:
			/* MoveImplementation::SCRAPSLUG(*it, *static_cast<CasterMove *>(&ctp)); */
			//TODO: play the associated anims
			break;
		case ANNEAL:
			/* MoveImplementation::ANNEAL(*it, *static_cast<CasterMove *>(&ctp)); */
			//TODO: play the associated anims
			break;
		case ANODIZE:
			/* MoveImplementation::ANODIZE(*it, *static_cast<CasterMove *>(&ctp)); */
			//TODO: play the associated anims
			break;
		case GALVANIZEDGLAIVE:
			/* MoveImplementation::GALVANIZEDGLAIVE(*it, *static_cast<CasterMove *>(&ctp)); */
			//TODO: play the associated anims
			break;
		case GRAVELSPIN:
			/* MoveImplementation::GRAVELSPIN(*it, *static_cast<CasterMove *>(&ctp)); */
			//TODO: play the associated anims
			break;
		case SANDBOMB:
			/* MoveImplementation::SANDBOMB(*it, *static_cast<CasterMove *>(&ctp)); */
			//TODO: play the associated anims
			break;
		case CRYSTALCAGE:
			/* MoveImplementation::CRYSTALCAGE(*it, *static_cast<CasterMove *>(&ctp)); */
			//TODO: play the associated anims
			break;
		case FISSURE:
			/* MoveImplementation::FISSURE(*it, *static_cast<CasterMove *>(&ctp)); */
			//TODO: play the associated anims
			break;
		case SHADOWSLASH:
			/* MoveImplementation::SHADOWSLASH(*it, *static_cast<CasterMove *>(&ctp)); */
			//TODO: play the associated anims
			break;
		case PILFER:
			/* MoveImplementation::PILFER(*it, *static_cast<CasterMove *>(&ctp)); */
			//TODO: play the associated anims
			break;
		case BLOODCURDLE:
			/* MoveImplementation::BLOODCURDLE(*it, *static_cast<CasterMove *>(&ctp)); */
			//TODO: play the associated anims
			break;
		case BLACKHOLE:
			/* MoveImplementation::BLACKHOLE(*it, *static_cast<CasterMove *>(&ctp)); */
			//TODO: play the associated anims
			break;
		default:
			break;
		}
	}
	return;
}

//TODO: Make a tool that creates enemies
void CreateGoons(CombatData &cbtD){
	cbtD.botTeam = new Team(); //ewww dynamic alloc...
	Actor *foo = new Actor(); //TODO: Do arena alloc instead
	Actor *bar = new Actor();

	Element gridNone[NUMCELLS] = {
		Element::none, Element::none, Element::none, Element::none,
		Element::none, Element::none, Element::none, Element::none,
		Element::none, Element::none, Element::none, Element::none,
		Element::none, Element::none, Element::none, Element::none
	};

	foo->name = "Metis";
	for(size_t i = 0; i < NUMCELLS; ++i){
		foo->grids[0][i] = gridNone[i];
		foo->grids[1][i] = gridNone[i];
		foo->grids[2][i] = gridNone[i];
	}
	foo->grids[1][0] = Element::water;

	bar->name = "Callisto";
	for(size_t i = 0; i < NUMCELLS; ++i){
		bar->grids[0][i] = gridNone[i];
		bar->grids[1][i] = gridNone[i];
		bar->grids[2][i] = gridNone[i];
	}
	bar->grids[1][0] = Element::spectre;

	foo->moves[0] = &AllMoves::moves[102]; //Rend
	bar->moves[0] = &AllMoves::moves[122]; //Shadow Slash

	AssignStats(foo);
	AssignStats(bar);

	cbtD.botTeam->members.push_back(foo);
	cbtD.botTeam->members.push_back(bar);
}


void ResetSelectAvailList(CombatData &cbtD){
	CombatData::chosenTargets.resize(0);
	CombatData::potTargets.resize(0);
	cbtD.targetAliveList.text.resize(0);
	cbtD.targetSelectedList.text.resize(0);
}


void ApplyEffect(Actor &target, int id){

}

/* void tickEffects(casterTargetsPairs &ctp){ */
/* 	//In reverse order of moves, tick all effects of every actor */
/* 	for(int i = (int)ctp.size() - 1; i > -1; --i){ */
/* 		if(!isDead(ctp[i].first) && ctp[i].first->currEffects.size() > 0){ */
/* 			for(effect *e : ctp[i].first->currEffects) */
/* 				e->tick(e, ctp[i].first); */
/* 		} */
/* 	} */
/* } */


/* void cleanupDead(team &t){ */
/* 	for(int i = 0; i < (int)t.members.size(); ++i){ */
/* 		if(!isDead(t.members[i])){ */
/* 			t.members.erase(t.members.begin() + i); */
/* 			--i; */
/* 		} */
/* 	} */
/* } */
