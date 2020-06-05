#include "include\move.h"

Move AllMoves::moves[126] = {
	Move(1,"Ignite",Element::fire,1,125,1,50,0,0,true,false,false,Ownership::general),
	Move(2,"Blaze",Element::fire,2,83,1,75,0,30,false,false,false,Ownership::general),
	Move(3,"Fireball",Element::fire,3,49,1,100,0,50,false,false,false,Ownership::general),
	Move(4,"Explosion",Element::fire,5,121,1,125,0,100,true,false,false,Ownership::general),
	Move(5,"Fire Beam",Element::fire,7,46,1,175,0,140,false,false,false,Ownership::general),
	Move(6,"Flame Sword",Element::fire,4,80,1,150,0,120,true,false,false,Ownership::character0),
	Move(7,"Flare Kick",Element::fire,4,109,1,150,0,120,true,false,false,Ownership::character1),
	Move(8,"Firewall",Element::fire,4,39,-1,0,0,120,true,false,false,Ownership::character2),
	Move(9,"Searing Flesh",Element::fire,4,18,-1,0,0,120,true,false,false,Ownership::character3),
	Move(10,"Molten Meteor",Element::fire,8,89,3,175,0,170,true,false,false,Ownership::character0),
	Move(11,"Wildfire",Element::fire,8,17,1,175,0,170,false,false,false,Ownership::character1),
	Move(12,"Raze Earth",Element::fire,8,69,127,175,0,170,false,false,false,Ownership::character2),
	Move(13,"From the Ashes",Element::fire,8,84,1,0,175,170,true,false,false,Ownership::character3),
	Move(14,"Supernova",Element::fire,8,75,127,225,0,190,false,false,false,Ownership::general),
	Move(15,"Water Whip",Element::water,1,107,1,50,0,0,true,false,false,Ownership::general),
	Move(16,"Torrent",Element::water,2,120,1,75,0,10,false,false,false,Ownership::general),
	Move(17,"Tidal Wave",Element::water,3,14,1,100,0,40,true,false,false,Ownership::general),
	Move(18,"Coalesce",Element::water,5,55,-1,0,125,100,false,false,false,Ownership::general),
	Move(19,"Whirlpool",Element::water,7,90,1,175,0,150,true,false,false,Ownership::general),
	Move(20,"Hailstorm",Element::water,4,97,127,165,0,145,false,false,false,Ownership::character0),
	Move(21,"Aqua Prison",Element::water,4,32,1,165,0,160,false,false,false,Ownership::character1),
	Move(22,"Rainstorm",Element::water,4,56,127,0,0,50,false,false,false,Ownership::character2),
	Move(23,"Dew Point",Element::water,4,64,1,0,75,90,false,false,false,Ownership::character3),
	Move(24,"Flash Freeze",Element::water,8,23,1,175,0,160,false,false,false,Ownership::character0),
	Move(25,"Rip Tide",Element::water,8,103,2,175,0,160,false,false,false,Ownership::character1),
	Move(26,"Vapor Vortex",Element::water,8,34,1,175,0,160,false,false,false,Ownership::character2),
	Move(27,"Hell-Broth",Element::water,8,116,1,175,0,160,false,false,false,Ownership::character3),
	Move(28,"Tsunami",Element::water,8,104,127,225,0,190,true,false,false,Ownership::general),
	Move(29,"Seed Shot",Element::nature,1,86,1,50,0,0,true,false,false,Ownership::general),
	Move(30,"Acorn Assault",Element::nature,2,3,1,100,0,20,false,false,false,Ownership::general),
	Move(31,"Sapping Stems",Element::nature,3,57,1,75,0,45,false,false,false,Ownership::general),
	Move(32,"Nourish",Element::nature,5,52,-1,0,150,110,false,false,false,Ownership::general),
	Move(33,"Leaf Lance",Element::nature,7,27,1,175,0,145,true,false,false,Ownership::general),
	Move(34,"Fungal Spores",Element::nature,4,92,1,150,0,125,true,false,false,Ownership::character0),
	Move(35,"Barbed Husk",Element::nature,4,9,-1,0,75,95,false,false,false,Ownership::character1),
	Move(36,"Grass Blades",Element::nature,4,77,3,125,0,110,false,false,false,Ownership::character2),
	Move(37,"Petal Pike",Element::nature,4,78,1,125,0,100,false,false,false,Ownership::character3),
	Move(38,"Undergrowth",Element::nature,8,4,1,175,0,170,true,false,false,Ownership::character0),
	Move(39,"Symbiosis",Element::nature,8,99,-1,0,175,160,false,false,false,Ownership::character1),
	Move(40,"Ensnaring Vine",Element::nature,8,37,1,175,0,165,true,false,false,Ownership::character2),
	Move(41,"Rancid Roses",Element::nature,8,70,1,175,0,165,true,false,false,Ownership::character3),
	Move(42,"Dryad's Curse",Element::nature,8,117,127,225,0,190,false,false,false,Ownership::general),
	Move(43,"Zap",Element::lightning,1,24,1,50,0,0,true,false,false,Ownership::general),
	Move(44,"Charge",Element::lightning,2,71,-1,0,0,10,false,false,false,Ownership::general),
	Move(45,"Discharge",Element::lightning,3,43,1,125,0,90,true,false,false,Ownership::general),
	Move(46,"Lightning Bolt",Element::lightning,5,126,3,125,0,100,false,false,false,Ownership::general),
	Move(47,"Dazzling Lights",Element::lightning,7,64,127,0,0,145,true,false,false,Ownership::general),
	Move(48,"Electrostorm",Element::lightning,4,40,2,175,0,165,true,false,false,Ownership::character0),
	Move(49,"Lightning Kick",Element::lightning,4,72,1,165,0,145,false,false,false,Ownership::character1),
	Move(50,"Sparking Skin",Element::lightning,4,45,-1,125,50,125,false,false,false,Ownership::character2),
	Move(51,"Superconduct",Element::lightning,4,82,1,165,0,145,true,false,false,Ownership::character3),
	Move(52,"Static Shock",Element::lightning,8,30,1,175,0,160,false,false,false,Ownership::character0),
	Move(53,"Evanescent Field",Element::lightning,8,110,-1,0,0,135,true,false,false,Ownership::character1),
	Move(54,"High Voltage",Element::lightning,8,20,-1,0,0,150,true,false,false,Ownership::character2),
	Move(55,"Mjolnir",Element::lightning,8,87,1,180,0,170,false,false,false,Ownership::character3),
	Move(56,"Closed Ciruit",Element::lightning,8,1,127,225,0,195,false,false,false,Ownership::general),
	Move(57,"Iron Spikes",Element::metal,1,7,1,50,0,0,false,false,false,Ownership::general),
	Move(58,"Cannonball",Element::metal,2,119,1,100,0,15,false,false,false,Ownership::general),
	Move(59,"Sharpen",Element::metal,3,28,-1,0,0,65,true,false,false,Ownership::general),
	Move(60,"Shatter Shrapnel",Element::metal,5,58,2,115,0,80,false,false,false,Ownership::general),
	Move(61,"Ball and Chain",Element::metal,7,65,1,175,0,155,true,false,false,Ownership::general),
	Move(62,"Quicksilver",Element::metal,4,96,-1,0,0,65,false,false,false,Ownership::character0),
	Move(63,"Copper Cutlass",Element::metal,4,8,1,125,0,90,false,false,false,Ownership::character1),
	Move(64,"Brass Knuckles",Element::metal,4,91,1,125,0,90,true,false,false,Ownership::character2),
	Move(65,"Midas Touch",Element::metal,4,93,1,125,0,100,true,false,false,Ownership::character3),
	Move(66,"Chrome Plated",Element::metal,8,53,-1,0,75,135,true,false,false,Ownership::character0),
	Move(67,"Magnetize",Element::metal,8,42,1,125,0,165,true,false,false,Ownership::character1),
	Move(68,"Steel Fist",Element::metal,8,100,1,175,0,150,false,false,false,Ownership::character2),
	Move(69,"Alloy Assault",Element::metal,8,85,1,180,0,170,true,false,false,Ownership::character3),
	Move(70,"Piercing Platinum",Element::metal,8,38,127,225,0,190,true,false,false,Ownership::general),
	Move(71,"Hurl Rock",Element::rock,1,81,1,50,0,0,true,false,false,Ownership::general),
	Move(72,"Stone Spear",Element::rock,2,66,1,100,0,40,false,false,false,Ownership::general),
	Move(73,"Fortify",Element::rock,3,35,-1,0,75,75,true,false,false,Ownership::general),
	Move(74,"Quicksand",Element::rock,5,105,1,125,0,95,true,false,false,Ownership::general),
	Move(75,"Entomb",Element::rock,7,5,1,160,0,160,true,false,false,Ownership::general),
	Move(76,"Crush",Element::rock,4,12,1,160,0,125,true,false,false,Ownership::character0),
	Move(77,"Tremor",Element::rock,4,94,1,160,0,125,true,false,false,Ownership::character1),
	Move(78,"Rockslide",Element::rock,4,102,1,160,0,125,false,false,false,Ownership::character2),
	Move(79,"Rolling Boulder",Element::rock,4,59,1,160,0,125,true,false,false,Ownership::character3),
	Move(80,"Sapphire Strike",Element::rock,8,108,1,145,0,155,true,false,false,Ownership::character0),
	Move(81,"Ruby Rush",Element::rock,8,36,1,145,0,155,true,false,false,Ownership::character1),
	Move(82,"Garnet Gaze",Element::rock,8,10,1,145,0,155,false,false,false,Ownership::character2),
	Move(83,"Emerald Edge",Element::rock,8,61,1,145,0,155,false,false,false,Ownership::character3),
	Move(84,"Obsidian Onslaught",Element::rock,8,118,127,225,0,190,true,false,false,Ownership::general),
	Move(85,"Ectoray",Element::spectre,1,88,1,50,0,0,false,false,false,Ownership::general),
	Move(86,"Torment",Element::spectre,2,63,1,75,0,25,false,false,false,Ownership::general),
	Move(87,"Chaos Claw",Element::spectre,3,60,1,100,0,75,false,false,false,Ownership::general),
	Move(88,"Phantom Walts",Element::spectre,5,22,1,0,75,100,false,false,false,Ownership::general),
	Move(89,"Gravetender",Element::spectre,7,47,0,0,0,155,false,false,false,Ownership::general),
	Move(90,"Mind Invasion",Element::spectre,4,16,1,75,0,115,true,false,false,Ownership::character0),
	Move(91,"Binding Pain",Element::spectre,4,29,1,75,0,100,true,false,false,Ownership::character1),
	Move(92,"Plague",Element::spectre,4,21,1,125,0,100,true,false,false,Ownership::character2),
	Move(93,"Wicked Hex",Element::spectre,4,98,1,125,0,100,true,false,false,Ownership::character3),
	Move(94,"Petrifying Palm",Element::spectre,8,106,1,175,0,165,false,false,false,Ownership::character0),
	Move(95,"Decay",Element::spectre,8,114,1,175,0,150,true,false,false,Ownership::character1),
	Move(96,"Vivid Nightmare",Element::spectre,8,11,1,175,0,155,false,false,false,Ownership::character2),
	Move(97,"Ethereal Fog",Element::spectre,8,123,1,160,0,170,true,false,false,Ownership::character3),
	Move(98,"Noxious Void",Element::spectre,8,6,127,225,0,190,true,false,false,Ownership::general),
	Move(99,"Flash Friction",Element::fire,1,31,1,75,0,30,true,false,false,Ownership::general),
	Move(100,"Scorching Heat",Element::fire,4,2,1,125,0,100,true,false,false,Ownership::general),
	Move(101,"Dragon Breath",Element::fire,6,62,1,150,0,125,true,false,false,Ownership::general),
	Move(102,"Icarus Inferno",Element::fire,8,44,2,175,0,160,false,false,false,Ownership::general),
	Move(103,"Rend",Element::water,1,26,1,75,0,35,true,false,false,Ownership::general),
	Move(104,"Siren's Tear",Element::water,4,48,-1,0,125,95,true,false,false,Ownership::general),
	Move(105,"Chilling Blast",Element::water,6,13,1,150,0,125,false,false,false,Ownership::general),
	Move(106,"Drowing Deluge",Element::water,8,41,2,175,0,170,true,false,false,Ownership::general),
	Move(107,"Needle",Element::nature,1,122,1,75,0,25,false,false,false,Ownership::general),
	Move(108,"Bloom",Element::nature,4,50,-1,0,125,80,false,false,false,Ownership::general),
	Move(109,"Tree Cleaver",Element::nature,6,112,2,150,0,125,false,false,false,Ownership::general),
	Move(110,"Venom Coating",Element::nature,8,51,-1,0,75,150,false,false,false,Ownership::general),
	Move(111,"Surge",Element::lightning,1,25,1,75,0,25,true,false,false,Ownership::general),
	Move(112,"Overload",Element::lightning,4,113,-1,125,0,100,true,false,false,Ownership::general),
	Move(113,"Ion Strike",Element::lightning,6,124,1,150,0,125,false,false,false,Ownership::general),
	Move(114,"Plasma Pulse",Element::lightning,8,76,2,175,0,165,false,false,false,Ownership::general),
	Move(115,"Scrap Slug",Element::metal,1,25,1,75,0,45,false,false,false,Ownership::general),
	Move(116,"Anneal",Element::metal,4,113,-1,125,0,100,true,false,false,Ownership::general),
	Move(117,"Anodize",Element::metal,6,124,-1,150,0,125,false,false,false,Ownership::general),
	Move(118,"Galvanized Glaive",Element::metal,8,76,2,175,0,170,true,false,false,Ownership::general),
	Move(119,"Gravel Spin",Element::rock,1,68,1,75,0,55,true,false,false,Ownership::general),
	Move(120,"Sand Bomb",Element::rock,4,15,1,145,0,115,false,false,false,Ownership::general),
	Move(121,"Crystal Cage",Element::rock,6,95,1,125,0,125,false,false,false,Ownership::general),
	Move(122,"Fissure",Element::rock,8,73,2,175,0,165,false,false,false,Ownership::general),
	Move(123,"Shadow Slash",Element::spectre,1,19,1,75,0,50,true,false,false,Ownership::general),
	Move(124,"Pilfer",Element::spectre,4,67,1,100,0,100,true,false,false,Ownership::general),
	Move(125,"Bloodcurdle",Element::spectre,6,79,1,0,0,125,false,false,false,Ownership::general),
	Move(126,"Black Hole",Element::spectre,8,74,2,175,0,170,false,false,false,Ownership::general)
};

