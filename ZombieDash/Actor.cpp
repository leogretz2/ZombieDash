#include "Actor.h"
#include "StudentWorld.h"


// EXIT IMPLEMENTATIONS ////////////////////////////////////////////////////////////////////////////////

void Exit::doSomeThing() {
	getWorld()->activateOnAppropriateActors(this);
}


// PIT IMPLEMENTATIONS /////////////////////////////////////////////////////////////////////////////////

void Pit::doSomeThing() {
	getWorld()->activateOnAppropriateActors(this);
}


// FLAME IMPLEMENTATIONS ///////////////////////////////////////////////////////////////////////////////
void Flame::doSomeThing() {
	if (isDead())
		return;
	if (getTicks() >= 2) {
		setDead();
		return;
	}
	getWorld()->activateOnAppropriateActors(this);
	increaseTicks();
}

void Flame::activateIfAppropriate(Actor* a) { 
	if (!a->isntFlammable())
		a->dieByFallOrBurnIfAppropriate(); 
}

// VOMIT IMPLEMENTATIONS ///////////////////////////////////////////////////////////////////////////////

void Vomit::doSomeThing() {
	if (isDead())
		return;
	if (getTicks() >= 2) {
		setDead();
		return;
	}
	getWorld()->activateOnAppropriateActors(this);
	increaseTicks();
}

void Vomit::activateIfAppropriate(Actor* a) {
	if (a->triggersZombieVomit())
		a->beVomitedOnIfAppropriate();
}

// LANDMINE IMPLEMENTATIONS ////////////////////////////////////////////////////////////////////////////

void Landmine::doSomeThing() {
	if (isDead())
		return;
	if (!m_active) {
		decreaseTicks();
		if (getTicks() == 0)
			m_active = true;
		return;
	}
	getWorld()->activateOnAppropriateActors(this);
}

void Landmine::activateIfAppropriate(Actor* a) {
	if (a->triggersOnlyActiveLandmines()) {
		dieByFallOrBurnIfAppropriate();
		a->dieByFallOrBurnIfAppropriate();
	}
}

void Landmine::dieByFallOrBurnIfAppropriate() {
	setDead();

	getWorld()->playSound(SOUND_LANDMINE_EXPLODE);
	for (int i = getX() - 16; i < getX() + 17; i += 16)
		for (int j = getY() + 16; j > getY() - 17; j -= 16)
			getWorld()->addActor(new Flame(i, j, up, getWorld()));
	getWorld()->addActor(new Pit(getX(), getY(), getWorld()));
}

// GOODIE IMPLEMENTATIONS //////////////////////////////////////////////////////////////////////////////

void Goodie::activateIfAppropriate(Actor* a) {
	if (a->canPickUp())
		a->pickUpGoodieIfAppropriate(this);
	else return;
	getWorld()->increaseScore(50);
	setDead();
	getWorld()->playSound(SOUND_GOT_GOODIE);
}


// VACCINE GOODIE IMPLEMENTATIONS //////////////////////////////////////////////////////////////////////

void VaccineGoodie::doSomeThing() {
	if (isDead())
		return;
	getWorld()->activateOnAppropriateActors(this);
}

void VaccineGoodie::pickUp(Penelope* p) {
	p->increaseVaccines();
}


// GAS CAN GOODIE IMPLEMENTATIONS //////////////////////////////////////////////////////////////////////

void GasCanGoodie::doSomeThing() {
	if (isDead())
		return;
	getWorld()->activateOnAppropriateActors(this);
}

void GasCanGoodie::pickUp(Penelope* p) {
	p->increaseFlameCharges();
}


// LANDMINE GOODIE IMPLEMENTATIONS /////////////////////////////////////////////////////////////////////

void LandmineGoodie::doSomeThing() {
	if (isDead())
		return;
	getWorld()->activateOnAppropriateActors(this);
}

void LandmineGoodie::pickUp(Penelope* p) {
	p->increaseLandmines();
}


// PENELOPE IMPLEMENTATIONS ////////////////////////////////////////////////////////////////////////////

void Penelope::doSomeThing() {
	if (isDead())
		return;
	if (getInfected()) {
		if (getInfectionDuration() < 500 && getInfectionDuration() > 0)
			increaseInfection();
		else if (getInfectionDuration() >= 500) {
			setDead();
			return;
		}	
	}
	int dir;
	//int dir = 1000; 
	
	if (getWorld()->getKey(dir)) {
	//if(1){
		switch (dir) {
			case KEY_PRESS_SPACE:
				if (getNumFlameCharges() >= 1) {
					decreaseFlameCharges();
					getWorld()->playSound(SOUND_PLAYER_FIRE);
					switch (getDirection()) {
					case right:
						for (int i = 1; i < 4; i++) {
							if (!getWorld()->isFlameBlockedAt(getX() + i * SPRITE_WIDTH, getY()))
								getWorld()->addActor(new Flame(getX() + i * SPRITE_WIDTH, getY(), right, getWorld()));
							else break;
						}
						break;
					case left:
						for (int i = 1; i < 4; i++) {
							if (!getWorld()->isFlameBlockedAt(getX() - i * SPRITE_WIDTH, getY()))
								getWorld()->addActor(new Flame(getX() - i * SPRITE_WIDTH, getY(), right, getWorld()));
							else break;
						}
						break;
					case up:
						for (int i = 1; i < 4; i++) {
							if (!getWorld()->isFlameBlockedAt(getX(), getY() + i * SPRITE_HEIGHT))
								getWorld()->addActor(new Flame(getX(), getY() + i * SPRITE_HEIGHT, right, getWorld()));
							else break;
						}
						break;
					case down:
						for (int i = 1; i < 4; i++) {
							if (!getWorld()->isFlameBlockedAt(getX(), getY() - i * SPRITE_HEIGHT))
								getWorld()->addActor(new Flame(getX(), getY() - i * SPRITE_HEIGHT, right, getWorld()));
							else break;
						}
						break;
					default: break;
					}
				}
				break;
			case KEY_PRESS_TAB:
				if (getNumLandmines() >= 1) {
					getWorld()->addActor(new Landmine(getX(), getY(), getWorld()));
					decreaseLandmines();
				}
				break;
			case KEY_PRESS_ENTER:
				if (getNumVaccines() >= 1) {
					setUninfected();
					clearInfection();
					decreaseVaccines();
				}
			case KEY_PRESS_RIGHT:
				setDirection(right);
				if(!getWorld()->isBlocked(getX()+4, getY(), getDirection()))
					moveTo(getX() + 4.0, getY());
				break;
			case KEY_PRESS_LEFT:
				setDirection(left);
				if (!getWorld()->isBlocked(getX() - 4, getY(), getDirection()))
					moveTo(getX() - 4.0, getY());
				break;
			case KEY_PRESS_UP:
				setDirection(up);
				if (!getWorld()->isBlocked(getX(), getY()+4, getDirection()))
					moveTo(getX(), getY() + 4.0);
				break;
			case KEY_PRESS_DOWN:
				setDirection(down);
				if (!getWorld()->isBlocked(getX(), getY()-4, getDirection()))
					moveTo(getX(), getY() - 4.0);
				break;
			default: break;
		}
	}
}

void Penelope::useExitIfAppropriate() {
	getWorld()->recordLevelFinishedIfAllCitizensGone();
}

void Penelope::dieByFallOrBurnIfAppropriate() {
	setDead();
	getWorld()->playSound(SOUND_PLAYER_DIE);
}


// CITIZEN IMPLEMENTATIONS ////////////////////////////////////////////////////////////////////////////

void Human::beVomitedOnIfAppropriate() {
	if (getInfectionDuration() == 0)
		m_infection = 1;
	setInfected();
	getWorld()->playSound(SOUND_CITIZEN_INFECTED); 
}

void Citizen::doSomeThing() {
	if (isDead()) {
		increaseTicks();
		return;
	}
	if (getInfected()) {
		increaseInfection();
		if (getInfectionDuration() >= 500) {
			setDead();
			getWorld()->playSound(SOUND_ZOMBIE_BORN);
			getWorld()->increaseScore(-1000);
			if (randInt(1, 10) <= 3)
				getWorld()->addActor(new SmartZombie(getX(), getY(), getWorld()));
			else
				getWorld()->addActor(new DumbZombie(getX(), getY(), getWorld()));
			increaseTicks();
			return;
		}
	}
	if (getTicks() % 2 == 0) {
		increaseTicks();
		return;
	}
	double otherX, otherY, distance;
	bool isThreat;
	if (getWorld()->locateNearestCitizenTrigger(getX(), getY(), otherX, otherY, distance, isThreat)) {
		if (!isThreat && distance <= 80) {
			int dir;
			bool samerc = false;
			if (getX() == otherX) {
				if (getY() < otherY) dir = up;
				else dir = down;
				samerc = true;
			}else if (getY() == otherY) {
				if (getX() < otherX) dir = right;
				else dir = left;
				samerc = true;
			}
			if (samerc) {
				double dest_X = 0;
				double dest_Y = 0;
				if (moveToIfWorks(dest_X, dest_Y, dir)) {
					increaseTicks();
					return;
				}
				samerc = false;
			}
			int coord;
			if (getX() < otherX && getY() < otherY) { dir = randInt(0, 1) * 90; coord = 1; samerc = true; }
			else if (getX() > otherX && getY() < otherY) { dir = randInt(1, 2) * 90; coord = 2; samerc = true; }
			else if (getX() > otherX && getY() > otherY) { dir = randInt(2, 3) * 90; coord = 3; samerc = true; }
			else if (getX() < otherX && getY() > otherY){
				switch (randInt(0, 1)) {
				case 0: dir = right; break;
				case 1: dir = down; break;
				default: break;
				}
				coord = 4;
				samerc = true;
			}
			if (samerc) {
				double dest_X2 = 0;
				double dest_Y2 = 0;
				if (moveToIfWorks(dest_X2, dest_Y2, dir)) {
					increaseTicks();
					return;
				}
				else {
					//samerc = false;
					switch (coord) {
					case 1: if (dir == 0) dir = 90; else dir = 0; /*samerc = true;*/  break;
					case 2: if (dir == 90) dir = 180; else dir = 90;  break;
					case 3: if (dir == 180) dir = 270; else dir = 180;  break;
					case 4: if (dir == 270) dir = 0; else dir = 270;  break;
					default: break;
					}
				}
				double dest_X3 = 0;
				double dest_Y3 = 0;
				if (moveToIfWorks(dest_X3, dest_Y3, dir)) {
					increaseTicks();
					return;
				}
			}
		}
	}
	double otherX2, otherY2, distance2;
	if (getWorld()->locateNearestCitizenThreat(getX(), getY(), otherX2, otherY2, distance2)) {
		if (distance2 <= 80) {
			double newDist = distance2;
			double dest_X4 = 0;
			double dest_Y4 = 0;
			int dir_4 = 0;
			for (int dir = 0; dir < 271; dir += 90) {
				double dest_XPot = 0;
				double dest_YPot = 0;
				computeNewCoords(dest_XPot, dest_YPot, 2, dir);
				if (!getWorld()->isZombieorCitizenBlocked(dest_XPot, dest_YPot, dir)) {
					double dx = abs(dest_XPot - otherX2);
					double dy = abs(dest_YPot - otherY2);
					double euc = sqrt(dx * dx + dy * dy);
					if (euc > newDist) {
						newDist = euc;
						dest_X4 = dest_XPot;
						dest_Y4 = dest_YPot;
					}
				}
			}
			if (newDist == distance2) {
				increaseTicks();
				return;
			}
			else {
				setDirection(dir_4);
				moveTo(dest_X4, dest_Y4);
				increaseTicks();
				return;
			}
		}
	}
	return;
}

bool Citizen::moveToIfWorks(double dest_X, double dest_Y, int dir) {
	computeNewCoords(dest_X, dest_Y, 2, dir);
	if (!getWorld()->isZombieorCitizenBlocked(dest_X, dest_Y, dir)) {
		setDirection(dir);
		moveTo(dest_X, dest_Y);
		increaseTicks();
		return true;
	}
	return false;
}

void Citizen::useExitIfAppropriate() {
	getWorld()->increaseScore(500);
	setDead();
	getWorld()->playSound(SOUND_CITIZEN_SAVED);
	getWorld()->recordCitizenGone();
}

void Citizen::dieByFallOrBurnIfAppropriate() {
	getWorld()->increaseScore(-1000);
	setDead();
	getWorld()->recordCitizenGone();
}


// DUMB ZOMBIE IMPLEMENTATIONS ////////////////////////////////////////////////////////////////////////

void Agent::computeNewCoords(double& vX, double& vY, int diff, int dir) {
	switch (dir) {
	case up:
		vY = getY() + diff;
		vX = getX();
		break;
	case down:
		vY = getY() - diff;
		vX = getX();
		break;
	case left:
		vX = getX() - diff;
		vY = getY();
		break;
	case right:
		vX = getX() + diff;
		vY = getY();
		break;
	default:
		break;
	}
}

void DumbZombie::doSomeThing() {
	if (isDead()) {
		increaseTicks();
		return;
	}
	if (getTicks() % 2 == 0) {
		increaseTicks();
		return;
	}	
	double vX = 0;
	double vY = 0;
	computeNewCoords(vX, vY, SPRITE_HEIGHT, getDirection());
	if (getWorld()->isZombieVomitTriggerAt(vX, vY)) {  // If there is a human nearby, vomit
		if (randInt(1,3) == 1) {
			getWorld()->addActor(new Vomit(vX, vY, getDirection(), getWorld()));
			getWorld()->playSound(SOUND_ZOMBIE_VOMIT);
			return;
		}
	}
	if (getMoveDist() == 0) {		// If movement plan is empty, randomly determine new one and new random direction
		setMoveDist(randInt(3,10));
		setDirection(randInt(0, 3) * 90);
	}
	double dest_X = 0;  
	double dest_Y = 0;
	computeNewCoords(dest_X, dest_Y, 1, getDirection());  // Determine new coordinates 1 pixel in the correct direction
	if (!getWorld()->isZombieorCitizenBlocked(dest_X, dest_Y, getDirection())) {
		moveTo(dest_X, dest_Y);	// Move to new coordinates if possible
		decreaseMoveDist();
	} else setMoveDist(0); // If can't, reset movement plan
	
	increaseTicks();
}

void DumbZombie::dieByFallOrBurnIfAppropriate() {
	getWorld()->increaseScore(1000);
	setDead(); 
	getWorld()->playSound(SOUND_ZOMBIE_DIE);
	if (randInt(1, 10) == 1) {
		double dest_X = 0;
		double dest_Y = 0;
		switch (randInt(0, 3) * 90) {
			case 0: dest_X = getX() + SPRITE_WIDTH; dest_Y = getY(); break;
			case 90: dest_X = getX(); dest_Y = getY() + SPRITE_HEIGHT; break;
			case 180: dest_X = getX() - SPRITE_WIDTH; dest_Y = getY(); break;
			case 270: dest_X = getX(); dest_Y = getY() - SPRITE_HEIGHT; break;
			default: break;
		}
		if (!getWorld()->overlapWithAnything(dest_X, dest_Y))
			getWorld()->addActor(new VaccineGoodie(dest_X, dest_Y, getWorld()));
	}
}


// SMART ZOMBIE IMPLEMENTATIONS ////////////////////////////////////////////////////////////////////////

void SmartZombie::doSomeThing() {
	if (isDead()) {
		increaseTicks();
		return;
	}
	if (getTicks() % 2 == 0) {
		increaseTicks();
		return;
	}
	double vX = 0;
	double vY = 0;
	computeNewCoords(vX, vY, SPRITE_HEIGHT, getDirection());
	if (getWorld()->isZombieVomitTriggerAt(vX, vY)) {  // If there is a human nearby, vomit
		if (randInt(1,3) == 1) {
			getWorld()->addActor(new Vomit(vX, vY, getDirection(), getWorld()));
			getWorld()->playSound(SOUND_ZOMBIE_VOMIT);
			return;
		}
	}
	if (getMoveDist() == 0) {		// If movement plan is empty, determine new one and new random direction
		setMoveDist(randInt(3, 10));
		double otherX, otherY, distance;
		if (getWorld()->locateNearestVomitTrigger(getX(), getY(), otherX, otherY, distance)) {
			findNewDir(otherX, otherY, distance);
		}
	}
	double dest_X = 0;
	double dest_Y = 0;
	computeNewCoords(dest_X, dest_Y, 1, getDirection());  // Determine new coordinates 1 pixel in the correct direction
	if (!getWorld()->isZombieorCitizenBlocked(dest_X, dest_Y, getDirection())) {
		moveTo(dest_X, dest_Y);	// Move to new coordinates if possible
		decreaseMoveDist();
	}
	else setMoveDist(0); // If can't, reset movement plan

	increaseTicks();
}

void SmartZombie::dieByFallOrBurnIfAppropriate() { 
	getWorld()->increaseScore(2000);
	setDead();
	getWorld()->playSound(SOUND_ZOMBIE_DIE);
}

void SmartZombie::findNewDir(double otherX, double otherY, double distance) {
	if (distance > 80)
		setDirection(randInt(0, 3) * 90);
	else {
		if (getX() == otherX) {
			if (getY() < otherY)
				setDirection(up);
			else
				setDirection(down);
		}
		else if (getY() == otherY) {
			if (getX() < otherX)
				setDirection(right);
			else
				setDirection(left);
		}
		else if (getX() < otherX && getY() < otherY)
			setDirection(randInt(0, 1) * 90);
		else if (getX() > otherX && getY() < otherY)
			setDirection(randInt(1, 2) * 90);
		else if (getX() > otherX && getY() > otherY)
			setDirection(randInt(2, 3) * 90);
		else {
			switch (randInt(0, 1)) {
			case 0: setDirection(right); break;
			case 1: setDirection(down); break;
			default: break;
			}
		}
	}
}

