#include "include\actor.h"
#include "include\move.h"

/* Actor::Actor(const Actor* a){ */
/* 	for(int i = 0; i < NUMGRIDS; ++i) */
/* 		for(int j = 0; j < NUMCELLS; ++j) */
/* 			this->grids[i][j] = a->grids[i][j]; */

/* 	this->remHP = a->remHP; */
/* 	this->remMP = a->remMP; */
/* 	this->maxHP = a->maxHP; */
/* 	this->maxMP = a->maxMP; */
/* 	this->pIncap = a->pIncap; */
/* 	this->mIncap = a->mIncap; */
/* 	this->isDisabled = a->isDisabled; */
	

/* 	for(int i = 0; i < NUMELEMENTS; ++i){ */
/* 		this->mAtk[i] = a->mAtk[i]; */
/* 		this->mDef[i] = a->mDef[i]; */
/* 		this->pAtk[i] = a->pAtk[i]; */
/* 		this->pDef[i] = a->pDef[i]; */
/* 	} */

/* 	this->hpBonus = a->hpBonus; */
/* 	this->mpBonus = a->mpBonus; */

/* 	this->type[0] = a->type[0]; */
/* 	this->type[1] = a->type[1]; */

/* } */

/*
Actor::Actor(const Actor_serialized &as){
	for (int i = 0; i < NUMGRIDS; ++i)
		for(int j = 0; j < NUMCELLS; ++j)
			grids[i][j] = as.grids[i][j];

	remHP = as.remHP;
	remMP = as.remMP;
	type[0] = type[1] = Element::none;
	assignStats(this);

	for(int i = 0; i < NUMMOVES; ++i)
		moves[i] = nullptr;

	chosen = nullptr;
}


Actor_serialized::Actor_serialized(Actor *a){
	if(!a)
		return;

	for(int i = 0; i < NUMGRIDS; ++i)
		for(int j = 0; j < NUMCELLS; ++j)
			this->grids[i][j] = a->grids[i][j];

	this->remHP = a->remHP;
	this->remMP = a->remMP;

	for(int i = 0; i < NUMMOVES; ++i){
		if(!a->moves[i])
			this->moveIds[i] = 0;
		else
			this->moveIds[i] = a->moves[i]->id;
	}
}


void saveActor(Actor *a){
	Actor_serialized saved(a);
	std::ofstream file(a->name + ".Actor", std::ios::binary);
	const void *data = &saved;
	file.write((char*)data, sizeof(saved));
	file.close();
}


void loadActor(Actor *&a, const Actor_serialized &as){
	a = new Actor(as);
}
*/


void AssignStats(Actor *a){
	//Init all stats to 0
	for(int i = 0; i < NUMELEMENTS; ++i)
		a->mAtk[i] = a->mDef[i] = a->pAtk[i] = a->pDef[i] = 0;

	a->hpBonus = 0;
	a->mpBonus = 0;

	for(int i = 0; i < NUMGRIDS; ++i){
		for(int j = 0; j < NUMCELLS; ++j){
			if(i == 0){
				if(a->grids[i][j] > Element::none && a->grids[i][j] <= Element::spectre){
					a->mAtk[a->grids[i][j] - 1]++; //At the elem., incr.
					a->mDef[a->grids[i][j] - 1]++;
				}
			}
			if(i == 1){
				if(a->grids[i][j] > Element::none && a->grids[i][j] <= Element::spectre){
					a->pDef[a->grids[i][j] - 1]++;
					/* a->hpBonus += (a->grids[i][j]); //FIXME!!!, add HPBONUS_***! */
					switch(a->grids[i][j]){ //TODO: check if this works
					case Element::fire:
						a->hpBonus += HPBONUS_FIRE;
						break;
					case Element::water:
						a->hpBonus += HPBONUS_WATER;
						break;
					case Element::nature:
						a->hpBonus += HPBONUS_NATURE;
						break;
					case Element::lightning:
						a->hpBonus += HPBONUS_LIGHTNING;
						break;
					case Element::metal:
						a->hpBonus += HPBONUS_METAL;
						break;
					case Element::rock:
						a->hpBonus += HPBONUS_ROCK;
						break;
					case Element::spectre:
						a->hpBonus += HPBONUS_SPECTRE;
						break;
					default:
						break;
					}
				}	
			}
			if(i == 2){
				if(a->grids[i][j] > Element::none && a->grids[i][j] <= Element::spectre){
					a->pAtk[a->grids[i][j] - 1]++;
					/* a->mpBonus += (a->grids[i][j]); //FIXME!!!, add MPBONUS_***! */
					switch(a->grids[i][j]){ //TODO: check if this works
					case Element::fire:
						a->mpBonus += MPBONUS_FIRE;
						break;
					case Element::water:
						a->mpBonus += MPBONUS_WATER;
						break;
					case Element::nature:
						a->mpBonus += MPBONUS_NATURE;
						break;
					case Element::lightning:
						a->mpBonus += MPBONUS_LIGHTNING;
						break;
					case Element::metal:
						a->mpBonus += MPBONUS_METAL;
						break;
					case Element::rock:
						a->mpBonus += MPBONUS_ROCK;
						break;
					case Element::spectre:
						a->mpBonus += MPBONUS_SPECTRE;
						break;
					default:
						break;
					}
				}
			}
			
			a->maxHP = (int)ceil(INITHP * (1.0f + HPMPBONUS_MULT * a->hpBonus));
			a->maxMP = (int)ceil(INITMP * (1.0f + HPMPBONUS_MULT * a->mpBonus));
		}
	}

	AssignType(*a);
}


/*void fullRestore(Actor &a){*/
/*	a.remHP = a.maxHP;*/
/*	a.remMP = a.maxMP;*/
/*}*/


/*void fullRestore(Vec<Actor*> &v){*/
/*	for(Actor *&a : v){*/
/*		a->remHP = a->maxHP;*/
/*		a->remMP = a->maxMP;*/
/*	}*/
/*}*/

//TODO Check if this actually works...
void AssignType(Actor &a){
	int counts[NUMELEMENTS] = { 0,0,0,0,0,0,0 };
	
	for(int i = 0; i < NUMGRIDS; ++i){
		for(int j = 0; j < NUMCELLS; ++j){
			if(a.grids[i][j] > Element::none && a.grids[i][j] <= Element::spectre){
				counts[a.grids[i][j] - 1]++;
			}
		}
	}
	
	/* int max = -1; */
	/* Element found = Element::none; */ 
	/* Element foundNext = Element::none; */
	/* bool getSecondary = true; //Should it find the secondary type? */
	/* for(int i = 0; i < NUMELEMENTS; ++i){ */
	/* 	if(counts[i] > max){ */
	/* 		max = counts[i]; */
	/* 		found = (Element)(i + 1); */
	/* 		getSecondary = true; */
	/* 	} */
	/* 	if(counts[i] == max && found != (Element)(i + 1) && getSecondary){ */
	/* 		foundNext = (Element)(i + 1); */
	/* 		getSecondary = false; */
	/* 	} */
	/* } */
	int max = 0;
	int maxIndex = -1;
	for(int i = 0; i < NUMELEMENTS; ++i){
		if(counts[i] > max){
			max = counts[i];
			maxIndex = i;
		}
	}
	int nextMaxIndex = -1;
	for(int i = 0; i < NUMELEMENTS; ++i){
		if(counts[i] == max && i != maxIndex){
			nextMaxIndex = i;
			break;
		}
	}
	Element found, foundNext;
	if(maxIndex == -1){
		found = Element::none;
		foundNext = Element::none;
	}
	else{
		found = (Element)(maxIndex + 1);
		if(nextMaxIndex != -1)
			foundNext = (Element)(nextMaxIndex + 1);
		else
			foundNext = Element::none;
	}

	a.type[0] = found;
	a.type[1] = foundNext;
}


/*bool isDead(const Actor &a){*/
/*	if(a.remHP <= 0)*/
/*		return true;*/
/*	return false;*/
/*}*/


/*bool isDead(const Actor *a){*/
/*	if(a->remHP <= 0)*/
/*		return true;*/
/*	return false;*/
/*}*/


/*std::ostream& operator<<(std::ostream &out, const Actor &a){*/
/*	out << a.name << "'s Stats: " << std::endl;*/
/*	out << "HP: " << a.remHP << "/" << a.maxHP << std::endl;*/
/*	out << "MP: " << a.remMP << "/" << a.maxMP << std::endl;*/
/*	//out << "HPBS: " << a.hpBonus << std::endl;*/
/*	//out << "MPBS: " << a.mpBonus << std::endl;*/
/*	out << "PATK: \n    ";*/
/*	for(int i = 0; i < NUMELEMENTS; ++i){*/
/*		if(i != NUMELEMENTS - 1)*/
/*			out << (Element)(i + 1) << ": " << a.pAtk[i] << " ";*/
/*		else*/
/*			out << (Element)(i + 1) << ": " << a.pAtk[i] << std::endl;*/
/*	}*/
/*	out << "PDEF: \n    ";*/
/*	for(int i = 0; i < NUMELEMENTS; ++i){*/
/*		if(i != NUMELEMENTS - 1)*/
/*			out << (Element)(i + 1) << ": " << a.pDef[i] << " ";*/
/*		else*/
/*			out << (Element)(i + 1) << ": " << a.pDef[i] << std::endl;*/
/*	}*/
/*	out << "MATK: \n    ";*/
/*	for(int i = 0; i < NUMELEMENTS; ++i){*/
/*		if(i != NUMELEMENTS - 1)*/
/*			out << (Element)(i + 1) << ": " << a.mAtk[i] << " ";*/
/*		else*/
/*			out << (Element)(i + 1) << ": " << a.mAtk[i] << std::endl;*/
/*	}*/
/*	out << "MDEF: \n    ";*/
/*	for(int i = 0; i < NUMELEMENTS; ++i){*/
/*		if(i != NUMELEMENTS - 1)*/
/*			out << (Element)(i + 1) << ": " << a.mDef[i] << " ";*/
/*		else*/
/*			out << (Element)(i + 1) << ": " << a.mDef[i] << std::endl;*/
/*	}*/
/*	return out;*/
/*}*/
