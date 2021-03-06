#include "PLAYER.hpp"
#include "State.hpp"

static String intToStr(int value)
{
	std::string str = std::to_string(value);
	std::locale locale;
	String result(str, locale);
	return result;
}

void Player::setCurrentState(State * state)
{
	current = state;
}

std::string Player::getCurrentStateName()
{
	return current->getStateName();
}

Player::Player(AnimationManager &a, int X, int Y, SoundSystem * soundSystem) : Entity(a, X, Y)
{
	h = a.getFrameHeight(0, "stay");
	w = a.getFrameWidth(0, "stay");
	life = 3;
	//STATE = stay;
	setCurrentState((State* )(new StayState()));
	name = "Player";
	range = 5;
	this->hand = NULL;
	this->soundHandler = soundSystem;

	// @implement normal hud items connections

	if (!this->font.loadFromFile("fonts/arial.ttf")) assert(0 && "font not loaded");
	amountShow.setFont(font);
	amountShow.setFillColor(Color::White);
	amountShow.setCharacterSize(16);
}

bool Player::isMoving() {
	return dx || dy;
}
void Player::setHand(Slot * bl) { hand = bl; }
Slot * Player::getHand() { return hand; }
void Player::reduceAmount(std::list<Slot* >  * slots)
{
	if (hand == NULL) return;
	for (std::list<Slot* >::iterator it = slots->begin(); it != slots->end(); it++)
	{
		Slot * s = *(it);
		if (AbstractBlock::compare(s->block, hand->block))
		{
			if (--hand->amount == 0)
			{
				it = slots->erase(it);
				hand = NULL;
				return;
			}
			else return;
		}
	}
	return;
}

bool Player::isInRange(int x, int y, double offsetX, double offsetY)
{
	return (abs(this->x - x - offsetX) / 32 < range && abs(this->y - y - offsetY) / 32 < range);
}

void Player::KeyCheck()
{


	if (key["L"])
		walkLeftPlayer();

	if (key["R"])
		walkRightPlayer();

	if (key["Up"])
		jumpPlayer();

	if (key["Down"])
		duckPlayer();

	// keys release
	if (!key["R"] && !key["L"] 
	&& !key["Down"] && !key["Up"]
	&& getCurrentStateName() != "jump")
		stayPlayer();
}

void Player::update(float time, String TileMap[], std::list<AbstractBlock *> blocks)
{

	if(!this->hit)KeyCheck();
	anim.flip(dir);

	float encrease = dx * time;
	int encLimit = 32;
	if (fabs(encrease) > encLimit) { encrease = encrease > 0 ? encLimit : -encLimit; std::cout << "Enc Limit hit!" << std::endl; }
	x += encrease;
	Collision(0, TileMap, blocks);
	if (!onGround) dy = dy + 0.0005*time;
	encrease = dy * time;
	if (fabs(encrease) > encLimit) { encrease = encrease > 0 ? encLimit : -encLimit; std::cout << "Enc Limit hit!" << std::endl; }
	y += encrease;
	onGround = false;
	Collision(1, TileMap, blocks);


	if (x > W * BLOCK_SIZE || x < 0 || y < 0 || y > H * BLOCK_SIZE) playerPosNormalize();

	anim.tick(time);
	key["R"] = key["L"] = key["Up"] = key["Down"] = key["Space"] = false;
}
Entity * Player::clone()
{
	AnimationManager manager = *Entity::factory.getAnimationManager(this->anim.getFileNameOrigin(), (Texture *)this->anim.getTexture());
	return new Player(manager, this->x, this->y, this->soundHandler);
}
void Player::playerPosNormalize() {
	this->x = W * BLOCK_SIZE/2;
	this->y = 60;
}

void Player::walkLeftPlayer()
{
	if (current->getStateName() != "walkLeft") {
		//std::cout << "walkLeft" << std::endl;
		current->walkLeft(this);
	}
}

void Player::walkRightPlayer()
{
	if (current->getStateName() != "walkRight") {
		//std::cout << "walkRight" << std::endl;
		current->walkRight(this);
	}
}

void Player::jumpPlayer()
{
	if (current->getStateName() != "jump") {
		//std::cout << "jump" << std::endl;
		current->jump(this);
	}
}

void Player::stayPlayer()
{
	if (current->getStateName() != "stay") {
		//std::cout << "stay" << std::endl;
		current->stay(this);
	}
}

void Player::duckPlayer()
{
	if (current->getStateName() != "duck") {
		//std::cout << "duck" << std::endl;
		current->duck(this);
	}
}

void Player::Collision(int dir, String TileMap[], std::list<AbstractBlock *> blocks)
{
	for (int i = y / BLOCK_SIZE; i < (y + h) / BLOCK_SIZE; i++)
		for (int j = x / BLOCK_SIZE; j < (x + w) / BLOCK_SIZE; j++)
		{
			if (i >= H || j >= W || i < 0 || j < 0) continue;
			char s = TileMap[i][j];
			AbstractBlock * b = AbstractBlock::getBlockFromList(s, blocks);
			if (b && b->getCollision())
			{
				if (dx > 0 && dir == 0) x = j * BLOCK_SIZE - w;
				else if (dx < 0 && dir == 0) x = j * BLOCK_SIZE + BLOCK_SIZE;
				if (dy > 0 && dir == 1)
				{
					y = i * BLOCK_SIZE - h;
					dy = 0;
					onGround = true;

					if(getCurrentStateName() == "jump") stayPlayer();

					if (hit) {
						hit = false;
						dx = 0;
					}
				}
				else if (dy < 0 && dir == 1)
				{
					y = i * BLOCK_SIZE + BLOCK_SIZE;
					dy = 0;
					if (hit) {
						hit = false;
						dx = 0;
					}
				}
			}
		}
}

void Player::drawHUD(RenderWindow & window, int vmodex, int vmodey, HudItems & items)
{
	RectangleShape bg = items.playerHud;
	bg.setPosition(64, vmodey - 64);
	window.draw(bg);
	
	
	for (int i = 0; i < this->life; i++) {
		Sprite heart = items.heart;
		heart.setPosition(0 + 30 * i, 0);
		window.draw(heart);
	}
	if (!hand || !hand->block) return;
	else if (this->getHand()->amount == 0) this->setHand(nullptr);
	else {
		RectangleShape inHand = *(hand->block->getRectangleShape());
		String ss = intToStr(hand->amount);
		amountShow.setString(ss);
		amountShow.setPosition(48 + 32, vmodey - 48);
		inHand.setPosition(48 + 32, vmodey - 48);
		window.draw(inHand);
		window.draw(amountShow);
	}
}
