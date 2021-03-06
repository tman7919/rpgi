#pragma once
#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "gui.h"

struct Grid;

enum class State{
	console_dialog,
	limbo,
	inventory,
	overworld_move,
	overworld_idle,
	combat_act,
	combat_act_transition,
	combat_watch
};

struct GameState{
	State curr;
	State prev;
	State old;
};

bool CanTransition(GameState &, State &);

#endif
