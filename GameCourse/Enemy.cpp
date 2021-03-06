#include "Enemy.hpp"
#include <iostream>
#include <list>
#include "Windows.h"
#include "Block.hpp"

const float ENEMY_SPEED = 0.05;

Enemy::Enemy(AnimationManager a, int X, int Y, bool dir) : Entity(a, X,Y)
{
	anim.set("walk");
	this->_observers.clear();
	//anim.animList["walk"].sprite.setColor(Color::Red);
	dx = 0.05;
	this->dir = dir;
	if (dir == 1) { dx = -dx; anim.flip(true); }
	w = anim.getFrameWidth(0, "walk");
	h = anim.getFrameHeight(0, "walk");
	name = "Enemy";
	this->initPosition = Vector2i(X, Y);
}

Enemy::~Enemy() {}

double normalizeDx(double dx) {
	if (dx > 0)
		return 0.05;
	return -0.05;
}

void Enemy::normalizePosition() {

	this->x = initPosition.x;
	this->y = initPosition.y;
}
void Enemy::update(float time, String TileMap[], std::list<AbstractBlock *> blocks)
{
	//dx = normalizeDx(dx);
	x += dx * time;
	Collision(0, TileMap, blocks);
	if (!onGround) dy = dy + 0.0005*time;
	y += dy * time;
	onGround = false;
	Collision(1, TileMap, blocks);


	if (x > W * 32 || x < 0 || y < 0 || y > H * 32) normalizePosition();
	
	if (life)anim.tick(time);
}

Entity * Enemy::clone()
{
	AnimationManager manager = *Entity::factory.getAnimationManager(this->anim.getFileNameOrigin(), (Texture *)this->anim.getTexture());
	return new Enemy(manager, this->x, this->y, this->dir);
}

void Enemy::Collision(int num, String TileMap[], std::list<AbstractBlock *> & blocks)
{

	for (int i = (y) / BLOCK_SIZE; i < (y + h) / BLOCK_SIZE; i++)
		for (int j = (x) / BLOCK_SIZE; j < (x + w) / BLOCK_SIZE; j++) {
			//Block  b(TileMap, i, j);

			if (i >= H || j >= W || i < 0 || j < 0) continue;
			char s = TileMap[i][j];
			AbstractBlock * b = AbstractBlock::getBlockFromList(s, blocks);
			if (b && b->getCollision()) {
				if (dx > 0 && num == 0) {
					dx = 0.05;
					x = j * BLOCK_SIZE - w;
					dir = true;
					dx = -dx; ///
					anim.flip(true);
				} else if (dx < 0 && num == 0) {
					dx = -0.05;
					x = j * BLOCK_SIZE + BLOCK_SIZE;
					dx = -dx;
					dir = false;
					anim.flip(false);
				} if (dy > 0 && num == 1) {
					y = i * BLOCK_SIZE - h;
					dy = 0;
					onGround = true;
					STATE = stay;
					if (hit) {
						hit = false;
						if (dir) dx = -ENEMY_SPEED;
						else dx = ENEMY_SPEED;
					}
				} if (dy < 0 && num == 1) {
					y = i * BLOCK_SIZE + BLOCK_SIZE;
					dy = 0;
					if (hit) {
						hit = false;
						if (dir) dx = -ENEMY_SPEED;
						else dx = ENEMY_SPEED;
					}
				}
			}
		}
}