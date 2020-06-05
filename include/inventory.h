#pragma once
#ifndef INVENTORY_H
#define INVENTORY_H
#include <string>
#include <vector>

#include "element.h"
#include "elemino.h"
#include "actor.h"
#include "raylib\raylib.h"

const int PORTRAIT_PADDING = 8;
const int PORTRAIT_SEPARATION = 80;
const int PORTRAIT_WIDTH = 64;
const int NUM_STATS = 6;

static const char *TEXT_MATK = "MATK: ";
static const char *TEXT_MDEF = "MDEF: ";
static const char *TEXT_HP = "HP: ";
static const char *TEXT_PDEF = "PDEF: ";
static const char *TEXT_PATK = "PATK: ";
static const char *TEXT_MP = "MP: ";

const Vector2 INV_POS = (Vector2){360, 64};
const int INV_GRID_WIDTH = 400;
const int INV_ELMN_OFFSET = 80;

const int MAX_TOTAL_ELMN = 512;

struct Player;
struct Actor;

template<typename T>
using Vec = std::vector<T>;

struct InventoryData{
	Grid *grids;
	Elemino *inFlight = nullptr; //The elemino not being dragged in open space

	InventoryPortraits portraits;
	Label statText[6];
	BoxLabel tooltip;

	const unsigned int MAX_INV_DISP = 20;
	unsigned char page;
	unsigned char focus;
	unsigned char lastFocus;
};

struct TotalEleminoes{
	static Elemino storage[MAX_TOTAL_ELMN];
	static unsigned int size;
};

void OpenInventory(Game &);
void HandleInventoryPortraits(InventoryData &);
void PositionEleminoes(Player &); //For the inventory
void StatTextSetup(InventoryData &, Actor &);
void UpdateStatText(InventoryData &, Actor &);
void StatToolTip(InventoryData &, Actor &);

void CheckForEleminoClicked(Game &); //Check if an elemino has been clicked on to start dragging
void MouseTouchElemino(Game &); //Drag, Rotate, and Check for release while mouse if touching elemino
void HandleFlyingElemino(Game &); //Either snap it to a grid or back into the inventory

void UnloadTexturesFrom(InventoryData &);

void AssignElementGrid(Grid *, Actor &); //Loop over grid and add the element to the actor's grid
#endif

