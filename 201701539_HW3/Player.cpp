#include "stdafx.h"
#include "Player.h"
Player::Player(wstring pPath, POINT pPos) :GameObject(pPath, pPos) {
}

Player::~Player() {
}

void Player::moveUp() {
	pos.y -= 5;
}

void Player::moveDown() {
	pos.y += 5;
}