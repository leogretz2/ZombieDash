#include "StudentWorld.h"
#include "GameConstants.h"
#include "Actor.h"
#include <string>
#include <sstream>
#include <cmath>
using namespace std;

GameWorld* createStudentWorld(string assetPath)
{
	return new StudentWorld(assetPath);
}


StudentWorld::StudentWorld(string assetPath)
: GameWorld(assetPath)
{
	m_citizens = 0;
	status = -1;
}

StudentWorld::~StudentWorld()
{
	cleanUp();
}

int StudentWorld::init() {
	switch (convertLevel()) {
	case 4:
		return GWSTATUS_LEVEL_ERROR;
		break;
	case 2:
		return GWSTATUS_PLAYER_WON;
		break;
	case 1:
		return GWSTATUS_CONTINUE_GAME;
		break;
	default: 
		return GWSTATUS_LEVEL_ERROR;
		break;
	}
}

int StudentWorld::move()
{
	if(!p->isDead())
		p->doSomeThing();
	for (list<Actor*>::iterator i = actors.begin(); i != actors.end(); i++) {
		(*i)->doSomeThing();
		if (p->isDead()) {
			decLives();
			status = GWSTATUS_PLAYER_DIED;
			return status;
		}
		if (status == GWSTATUS_FINISHED_LEVEL) {
			playSound(SOUND_LEVEL_FINISHED);
			return status;
		}
	}

	for (list<Actor*>::iterator i = actors.begin(); i != actors.end(); i++) {
		if ((*i)->isDead()) {
			delete *i;
			i = actors.erase(i);
			i--;
		}
	}

	ostringstream statusLine;
	statusLine << "Score: " << getScore() << "  Level: " << getLevel() <<
		"  Lives: " << getLives() << "  Vaccines: " << p->getNumVaccines() <<
		"  Flames: " << p->getNumFlameCharges() << "  Mines: " << p->getNumLandmines() <<
		"  Infected: " << p->getInfectionDuration();
	setGameStatText(statusLine.str());

	return status;
}

void StudentWorld::cleanUp()
{
	if(p != nullptr)
		delete p;
	p = nullptr;

	for (list<Actor*>::iterator i = actors.begin(); i != actors.end();) {
		delete *i;
		i = actors.erase(i);
	}
}

void StudentWorld::addActor(Actor* a) {
	actors.push_back(a);
}

void StudentWorld::recordCitizenGone() {
	m_citizens--;
}

void StudentWorld::recordLevelFinishedIfAllCitizensGone() {
	if (m_citizens == 0)
		status = GWSTATUS_FINISHED_LEVEL;
}

void StudentWorld::activateOnAppropriateActors(Actor* a) {
	
	for (list<Actor*>::iterator i = actors.begin(); i != actors.end(); i++) {
		if (overlap(a, *i) && (a != *i) && !a->isDead() && !(*i)->isDead())
			a->activateIfAppropriate(*i);
	}
	if (overlap(a, p))
		a->activateIfAppropriate(p);
}


int StudentWorld::convertLevel() {
	Level lev(assetPath());
	string levelFile;

	switch (getLevel()) {
	case 1:
		levelFile = "level01.txt";
		break;
	case 2:
		levelFile = "level02.txt";
		break;
	case 3:
		levelFile = "level03.txt";
		break;
	case 4:
		levelFile = "level04.txt";
		break;
	case 99:
	default:
		status = GWSTATUS_PLAYER_WON;
		return status;
		break;
	}

	//string levelFile = "level05.txt";
	Level::LoadResult result = lev.loadLevel(levelFile);

	if (result == Level::load_fail_file_not_found) {
		//cerr << "Cannot find level01.txt data file" << endl;
		status = GWSTATUS_LEVEL_ERROR;
		return status;
	}
	else if (result == Level::load_fail_bad_format) {
		//cerr << "Your level was improperly formatted" << endl;
		status = GWSTATUS_LEVEL_ERROR;
		return status;
	}
	else if (result == Level::load_success)
	{
		//addActor(new Landmine(16, 48, this));
		//cerr << "Successfully loaded level" << endl;
		for (int i = 0; i < LEVEL_WIDTH; i++) {
			for (int j = 0; j < LEVEL_HEIGHT; j++) {
				Level::MazeEntry ge = lev.getContentsOf(i, j); // level_x=5, level_y=10
				switch (ge) // so x=80 and y=160
				{
					case Level::empty:
						break;
					case Level::smart_zombie:
						addActor(new SmartZombie(i * SPRITE_WIDTH, j * SPRITE_HEIGHT, this));
						break;
					case Level::dumb_zombie:
						addActor(new DumbZombie(i * SPRITE_WIDTH, j * SPRITE_HEIGHT, this));
						break;
					case Level::player:
						p = new Penelope(i * SPRITE_WIDTH, j* SPRITE_HEIGHT, this);
						penelope.push_back(p);
						break;
					case Level::exit:
						addActor(new Exit(i * SPRITE_WIDTH, j * SPRITE_HEIGHT, this));
						break;
					case Level::wall:
						addActor(new Wall(i * SPRITE_WIDTH, j * SPRITE_HEIGHT, this));
						break;
					case Level::pit:
						addActor(new Pit(i * SPRITE_WIDTH, j * SPRITE_HEIGHT, this));
						break;
					case Level::citizen:
						addActor(new Citizen(i * SPRITE_WIDTH, j * SPRITE_HEIGHT, this));
						m_citizens++;
						break;
					case Level::vaccine_goodie:
						addActor(new VaccineGoodie(i * SPRITE_WIDTH, j * SPRITE_HEIGHT, this));
						break;
					case Level::gas_can_goodie:
						addActor(new GasCanGoodie(i * SPRITE_WIDTH, j * SPRITE_HEIGHT, this));
						break;
					case Level::landmine_goodie:
						addActor(new LandmineGoodie(i * SPRITE_WIDTH, j * SPRITE_HEIGHT, this));
						break;
				}
			}
		}
	}
	status = GWSTATUS_CONTINUE_GAME;
	return status;
}

bool StudentWorld::isBlocked(double x, double y, int dir) {
	bool value = false;
	for (list<Actor*>::iterator i = actors.begin(); i != actors.end(); i++) {
		if ((*i)->isBlockingType()) {
			value = blockages(x, y, dir, *i);
			if (value) return value;
		}
	}
	return value;	
}

bool StudentWorld::isZombieorCitizenBlocked(double x, double y, int dir) {
	bool value = false;
	for (list<Actor*>::iterator i = actors.begin(); i != actors.end(); i++) {
		if ((*i)->isBlockingType()) {
			value = blockages(x, y, dir, *i);
			if (value) return value;
		}
	}
	value = blockages(x, y, dir, p);
	return value;
}



bool StudentWorld::blockages(double x, double y, int dir, Actor* a) {
	switch (dir) {
		case right:
			if ((y >= a->getY() && y < a->getY() + SPRITE_HEIGHT ||
				y + SPRITE_HEIGHT > a->getY() && y + SPRITE_HEIGHT <= a->getY() + SPRITE_HEIGHT)
				&& (x + SPRITE_WIDTH > a->getX() && x + SPRITE_WIDTH <= a->getX() + SPRITE_WIDTH))
				return true;
			break;
		case left:
			if ((y >= a->getY() && y < a->getY() + SPRITE_HEIGHT ||
				y + SPRITE_HEIGHT >a->getY() && y + SPRITE_HEIGHT <= a->getY() + SPRITE_HEIGHT)
				&& (x < a->getX() + SPRITE_WIDTH && x >= a->getX()))
				return true;
			break;
		case up:
			if ((x >= a->getX() && x < a->getX() + SPRITE_WIDTH ||
				x + SPRITE_WIDTH > a->getX() && x + SPRITE_WIDTH <= a->getX() + SPRITE_WIDTH)
				&& (y + SPRITE_HEIGHT <= a->getY() + SPRITE_HEIGHT && y + SPRITE_HEIGHT > a->getY()))
				return true;
			break;
		case down:
			if ((x >= a->getX() && x < a->getX() + SPRITE_WIDTH ||
				x + SPRITE_WIDTH >a->getX() && x + SPRITE_WIDTH <= a->getX() + SPRITE_WIDTH)
				&& (y < a->getY() + SPRITE_HEIGHT && y >= a->getY()))
				return true;
			break;
		default: return false; break;
	}
	return false;
}

bool StudentWorld::isFlameBlockedAt(double x, double y) {
	for (list<Actor*>::iterator i = actors.begin(); i != actors.end(); i++) {
		if ((*i)->blocksFlame()) {
			if (overlapXY(x, y, *i))
				return true;
		}
	}
}


bool StudentWorld::isZombieVomitTriggerAt(double x, double y){
	for (list<Actor*>::iterator i = actors.begin(); i != actors.end(); i++) {
		if ((*i)->triggersZombieVomit()) {
			if (overlapXY(x, y, *i))
				return true;
		}
	}
	if (p->triggersZombieVomit())
		if (overlapXY(x, y, p))
			return true;
	return false;
}

bool StudentWorld::locateNearestVomitTrigger(double x, double y, double& otherX, double& otherY, double& distance) {
	double sqEuc = 150000;
	double targX = 0;
	double targY = 0;
	bool value = false;
	for (list<Actor*>::iterator i = actors.begin(); i != actors.end(); i++) {
		if ((*i)->triggersZombieVomit()) {
			double dx = abs(x - (*i)->getX());
			double dy = abs(y - (*i)->getY());
			double tempEuc = dx*dx + dy*dy;
			double tempX = (*i)->getX();
			double tempY = (*i)->getY();
			if (tempEuc < sqEuc) {
				sqEuc = tempEuc;
				targX = tempX;
				targY = tempY;
				value = true;
			}
		}
	}
	double dx2 = abs(x - p->getX());
	double dy2 = abs(y - p->getY());
	double tempEuc2 = dx2 * dx2 + dy2 * dy2;
	if (tempEuc2 < sqEuc) {
		sqEuc = tempEuc2;
		targX = p->getX();
		targY = p->getY();
		value = true;
	}
	otherX = targX;
	otherY = targY;
	distance = sqrt(sqEuc);
	return value;
}

bool StudentWorld::locateNearestCitizenTrigger(double x, double y, double& otherX, double& otherY, double& distance, bool& isThreat) {
	double sqEuc = 150000;
	double targX = 0;
	double targY = 0;
	bool value = false;
	bool threat = false;
	double distToZ = 0;
	double dx = abs(x - p->getX());
	double dy = abs(y - p->getY());
	double distToP = sqrt(dx * dx + dy * dy);
	if (!locateNearestCitizenThreat(x, y, targX, targY, distToZ)) {
		sqEuc = distToP;
		targX = p->getX();
		targY = p->getY();
		value = true;
	}
	else {
		if (distToZ > distToP) {
			sqEuc = distToP;
			targX = p->getX();
			targY = p->getY();
			threat = false;
			value = true;
		}
		else {
			sqEuc = distToZ;
			threat = true;
			value = true;
		}
	}
	distance = sqEuc;
	otherX = targX;
	otherY = targY;
	isThreat = threat;
	return value;
}

bool StudentWorld::locateNearestCitizenThreat(double x, double y, double& otherX, double& otherY, double& distance) {
	double sqEuc = 150000;
	double targX = 0;
	double targY = 0;
	bool value = false;
	for (list<Actor*>::iterator i = actors.begin(); i != actors.end(); i++) {
		if ((*i)->triggersCitizens()) {
			double dx = abs(x - (*i)->getX());
			double dy = abs(y - (*i)->getY());
			double tempEuc = dx * dx + dy * dy;
			double tempX = (*i)->getX();
			double tempY = (*i)->getY();
			if (tempEuc < sqEuc) {
				sqEuc = tempEuc;
				targX = tempX;
				targY = tempY;
				value = true;
			}
		}
	}
	distance = sqrt(sqEuc);
	otherX = targX;
	otherY = targY;
	return value;
}

bool StudentWorld::overlapWithAnything(double x, double y) {
	for (list<Actor*>::iterator i = actors.begin(); i != actors.end(); i++) {
		if (overlapXY(x, y, *i))
			return true;
	}
	if (overlapXY(x, y, p))
		return true;
	return false;
}

bool StudentWorld::overlap(Actor* a, Actor* b) {
	int dx = abs(a->getX() - b->getX());
	int dy = abs(a->getY() - b->getY());
	return (dx*dx + dy * dy <= 100);
}

bool StudentWorld::overlapXY(double x, double y, Actor* a) {
	int dx = abs(a->getX() - x);
	int dy = abs(a->getY() - y);
	return (dx*dx + dy * dy <= 100);
}

