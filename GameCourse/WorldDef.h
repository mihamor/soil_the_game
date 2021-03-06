#pragma once
#include <SFML/Graphics.hpp>

//const int H = 18;
//const int W = 39;
//const int H = 120;
//const int W = 360;
const int H = 120;
const int W = 230;

//const int H = 10;
//const int W = 40;

typedef struct  {
	sf::RectangleShape invBg;
	sf::RectangleShape playerHud;
	sf::RectangleShape craftBg;
	sf::RectangleShape chestBg;
	sf::Sprite heart;
} HudItems;
namespace HUD {
	HudItems loadHudItems(std::string invBg, std::string chestBg, std::string playerHud, std::string craftBg, std::string heartSp);
}
enum WeaponType {
	Meele,
	Ranged
};