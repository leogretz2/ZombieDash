#ifndef ACTOR_H_
#define ACTOR_H_

#include "GraphObject.h"
#include <cstdlib>
#include <ctime>

class StudentWorld;
class Goodie;
class Penelope;

// Students:  Add code to this file, Actor.cpp, StudentWorld.h, and StudentWorld.cpp

class Actor : public GraphObject {
public:

	static const int right = 0;
	static const int left = 180;
	static const int up = 90;
	static const int down = 270;

	Actor(int imageID, double x, double y, int dir, int depth, StudentWorld* sw) :
		GraphObject(imageID, x, y, dir, depth), m_sw(sw), m_alive(true) {}
	virtual ~Actor() {}

	// Action to perform for each tick.
	virtual void doSomeThing() = 0;

	// Is this actor dead?
	bool isDead() const { return !m_alive; }

	// Mark this actor as dead.
	void setDead() { m_alive = false; }

	// Returns the World the Actor is in
	StudentWorld* getWorld(){return m_sw;}

	// If this is an activated object, perform its effect on a (e.g., for an
	// Exit have a use the exit).
	virtual void activateIfAppropriate(Actor* a) { return; }

	// If this object uses exits, use the exit.
	virtual void useExitIfAppropriate() { return; }

	// If this object can die by falling into a pit or burning, die.
	virtual void dieByFallOrBurnIfAppropriate() { return; }

	// If this object can be infected by vomit, get infected.
	virtual void beVomitedOnIfAppropriate() { return; }

	// If this object can pick up goodies, pick up g
	virtual void pickUpGoodieIfAppropriate(Goodie* g) { return; }

	// Does this object block agent movement?
	virtual bool isBlockingType() const { return false; }

	// Does this object block flames?
	virtual bool blocksFlame() const { return false; }

	// Is this object immune to flame?
	virtual bool isntFlammable() const { return false; }

	// Can pick up goodies?
	virtual bool canPickUp() const { return false; }

	// Does this object trigger landmines only when they're active?
	virtual bool triggersOnlyActiveLandmines() const { return false; }

	// Can this object cause a zombie to vomit?
	virtual bool triggersZombieVomit() const { return false; }

	// Is this object a threat to citizens?
	virtual bool threatensCitizens() const { return false; }

	// Does this object trigger citizens to follow it or flee it?
	virtual bool triggersCitizens() const { return false; }

private:
	bool m_alive;
	StudentWorld* m_sw;
};

// WALL /////////////////////////////////////////////////////////////////////////////////////////////WALL////////
class Wall : public Actor {
public:
	Wall(double x, double y, StudentWorld* sw) : Actor(IID_WALL, x, y, right, 0, sw) {}

	//wall’s doSomeThing doesn’t do anything, it just returns.
	virtual void doSomeThing() { return; }

	// Wall is a blocking type
	virtual bool isBlockingType() const { return true; }

	// Wall blocks flame
	virtual bool blocksFlame() const { return true; }

	// Wall is not flammable
	virtual bool isntFlammable() const { return true; }
};


/// ACTIVATING OBECTS ///////////////////////////////////////////////////////////// ACTIVATING OBJECTS ////////
class ActivatingObject : public Actor
{
public:
	ActivatingObject(int imageID, double x, double y, int dir, int depth, StudentWorld* sw) :
		Actor(imageID, x, y, dir, depth, sw) {}
};

class Exit : public ActivatingObject
{
public:
	Exit(double x, double y, StudentWorld* sw) : ActivatingObject(IID_EXIT, x, y, right, 1, sw) {}

	// If exit is not dead, calls the activateAllAppropriateActors from StudentWorld class
	virtual void doSomeThing();

	// Exit blocks flame
	virtual bool blocksFlame() const { return true; }

	// Exit is not flammable
	virtual bool isntFlammable() const { return true; }

	// Calls useExitIfAppropriate if the actor uses exits
	virtual void activateIfAppropriate(Actor* a) { a->useExitIfAppropriate(); }
};

class Pit : public ActivatingObject
{
public:
	Pit(double x, double y, StudentWorld* sw) : ActivatingObject(IID_PIT, x, y, right, 0, sw) {}

	// If pit is not dead, calls the activateAllAppropriateActors from StudentWorld class
	virtual void doSomeThing();

	// Calls dieByBurnOrFallIfAppropriate if the actor dies by pit
	virtual void activateIfAppropriate(Actor* a) { a->dieByFallOrBurnIfAppropriate(); }
};

class Projectile : public ActivatingObject
{
public:
	Projectile(int imageID, double x, double y, int dir, StudentWorld* sw, int initTicks) :
		ActivatingObject(imageID, x, y, dir, 0, sw) , m_ticks(initTicks) {}

	//All projectiles are not flammable
	virtual bool isntFlammable() const { return true; }

	// Increase the ticks(how many frames the projectile has been alive) by one
	void increaseTicks() { m_ticks++; }

	// Decrease the ticks by one
	void decreaseTicks() { m_ticks--; }

	// Returns the ticks of the projectile
	int getTicks() { return m_ticks; }
private:
	int m_ticks;
};

class Flame : public Projectile
{
public:
	Flame(double x, double y, int dir, StudentWorld* sw) : Projectile(IID_FLAME, x, y, dir, sw, 0) {}

	// Flame threatens citizen
	virtual bool threatensCitizens() const { return true; }

	//If flame is not dead, calls the activateAllAppropriateActors from StudentWorld class
	virtual void doSomeThing();

	// Calls dieByBurnOrFallIfAppropriate if the actor dies by flame. Fames cause people to burn and die
	virtual void activateIfAppropriate(Actor* a);
};

class Vomit : public Projectile
{
public:
	Vomit(double x, double y, int dir, StudentWorld* sw) : Projectile(IID_VOMIT, x, y, dir, sw, 0) {}

	// If Vomit is not dead (dies if been alive for more than two ticks) calls the activateAllAppropriateActors from StudentWorld class
	virtual void doSomeThing();

	// Vomit threatens citizens
	virtual bool threatensCitizens() const { return true; }

	// Calls setInfected if the actor can be vomited on. Vomit infects people
	virtual void activateIfAppropriate(Actor* a);
};

class Landmine : public Projectile
{
public:
	Landmine(double x, double y, StudentWorld* sw) : Projectile(IID_LANDMINE, x, y, right, sw, 30),
		m_active(false) {}

	// If Landmine is not dead (dies if been alive for more than thirty ticks) calls the activateAllAppropriateActors from StudentWorld class
	virtual void doSomeThing();

	// Calls dieByBurnOrFallIfAppropriate if the actor dies by flame and on itself. Landmines create flames that cause people to burn and die
	virtual void activateIfAppropriate(Actor* a);

	// Kills the landmine and creates a flame on the location and in the eight places all around it. Also creates a pit on location
	virtual void dieByFallOrBurnIfAppropriate();

	// – Sets ticks to a specified value
	void setTicks(int ticks) { m_safetyTicks = ticks; }

	// Sets the landmine to inactive
	void setInactive() { m_active = false; }

private:
	int m_safetyTicks;
	bool m_active;
};

class Goodie : public ActivatingObject
{
public:
	Goodie(int imageID, double x, double y, StudentWorld* sw) : 
		ActivatingObject(imageID, x, y, right, 1, sw) {}

	// Calls pickUpGoodie if the actor picks goodies up. Goodie doesn’t implement doSomething because the individual goodies doSomeThing
	virtual void activateIfAppropriate(Actor* a);

	// Kills the goodie
	virtual void dieByFallOrBurnIfAppropriate() { setDead(); }

	// Have p pick up this goodie.
	virtual void pickUp(Penelope* p) = 0;
};

class VaccineGoodie : public Goodie
{
public:
	VaccineGoodie(double x, double y, StudentWorld* sw) : Goodie(IID_VACCINE_GOODIE, x, y, sw) {}

	// calls activateOnAppropriate from Student class
	virtual void doSomeThing();

	// increases the score by 50 and increases the amount of vaccines by 1
	virtual void pickUp(Penelope* p);
};

class GasCanGoodie : public Goodie
{
public:
	GasCanGoodie(double x, double y, StudentWorld* sw) : Goodie(IID_GAS_CAN_GOODIE, x, y, sw) {}


	virtual void doSomeThing();
	virtual void pickUp(Penelope* p);
};

class LandmineGoodie : public Goodie
{
public:
	LandmineGoodie(double x, double y, StudentWorld* sw) : Goodie(IID_LANDMINE_GOODIE, x, y, sw) {}
	virtual void doSomeThing();
	virtual void pickUp(Penelope* p);
};


/// AGENTS ////////////////////////////////////////////////////////////////////////////////////// AGENTS ////////
class Agent : public Actor
{
public:
	Agent(int imageID, double x, double y, int dir, StudentWorld* sw) :
		Actor(imageID, x, y, dir, 0, sw) {}
	virtual bool isBlockingType() const { return true; }
	virtual bool triggersOnlyActiveLandmines() const { return true; }
	void computeNewCoords(double& vX, double& vY, int diff, int dir);
};



class Human : public Agent
{
public:
	Human(int imageID, double x, double y, StudentWorld* sw) :
		Agent(imageID, x, y, right, sw) , m_infected(false), m_infection(0) {}
	virtual bool triggersZombieVomit() const { return true; }

	virtual void beVomitedOnIfAppropriate();

	// Make this human uninfected by vomit.
	void clearInfection() { m_infection = 0; }

	// Returns whether infected or not
	bool getInfected() { return m_infected; }

	// Increase infection
	void increaseInfection() { m_infection++; }

	// How many ticks since this human was infected by vomit?
	int getInfectionDuration() const { return m_infection; }

	// Set to infected
	void setInfected() { m_infected = true; }

	// Set to uninfected
	void setUninfected() { m_infected = false; }


private: 
	bool m_infected;
	int m_infection;
};

class Penelope : public Human {
public:
	Penelope(double x, double y, StudentWorld* sw) : Human(IID_PLAYER, x, y, sw),
		m_landmines(100), m_flmthrw(0), m_vacc(0) {};
	virtual void doSomeThing();
	virtual void useExitIfAppropriate();
	virtual void dieByFallOrBurnIfAppropriate();
	virtual void pickUpGoodieIfAppropriate(Goodie* g) { g->pickUp(this); }
	virtual bool triggersCitizens() const { return true; }
	virtual bool canPickUp() const { return true; }
	
	// Increase the number of vaccines the object has.
	void increaseVaccines() { m_vacc++; }

	// Decrease the number of vaccines the object has.
	void decreaseVaccines() { m_vacc--; }

	// Increase the number of flame charges the object has.
	void increaseFlameCharges() { m_flmthrw+=5; }

	// Decrease the number of flame charges the object has.
	void decreaseFlameCharges() { m_flmthrw--; }

	// Increase the number of landmines the object has.
	void increaseLandmines() { m_landmines+=2; }

	// Decrease the number of landmines the object has.
	void decreaseLandmines() { m_landmines--; }

	// How many vaccines does the object have?
	int getNumVaccines() const { return m_vacc; }

	// How many flame charges does the object have?
	int getNumFlameCharges() const { return m_flmthrw; }

	// How many landmines does the object have?
	int getNumLandmines() const { return m_landmines; }

private:
	int m_landmines;
	int m_flmthrw;
	int m_vacc;
};

class Citizen : public Human
{
public:
	Citizen(double x, double y, StudentWorld* sw) :
		Human(IID_CITIZEN, x, y, sw), m_ticks(1) {}
	virtual void doSomeThing();
	// Make sure to use different dead so doesn't subtract points
	virtual void useExitIfAppropriate();
	virtual void dieByFallOrBurnIfAppropriate();
	void increaseTicks() { m_ticks++; }
	void decreaseTicks() { m_ticks--; }
	int getTicks() { return m_ticks; }
	bool moveToIfWorks(double dest_X, double dest_Y, int dir);
private:
	int m_ticks;

};

class Zombie : public Agent
{
public:
	Zombie(double x, double y, StudentWorld* sw) : Agent(IID_ZOMBIE, x, y, right, sw),
		m_ticks(1), m_movementPlanDist(0) {}
	virtual bool triggersCitizens() const { return true; }
	void increaseTicks() { m_ticks++; }
	void decreaseTicks() { m_ticks--; }
	int getTicks() { return m_ticks; }
	void setMoveDist(int newDist) { m_movementPlanDist = newDist; }
	void decreaseMoveDist() { m_movementPlanDist--; }
	int getMoveDist() { return m_movementPlanDist; }

private:
	int m_ticks;
	int m_movementPlanDist;

};

class DumbZombie : public Zombie
{
public:
	DumbZombie(double x, double y, StudentWorld* sw) : Zombie(x, y, sw) {}
	virtual void doSomeThing();
	virtual void dieByFallOrBurnIfAppropriate();
private:
};

class SmartZombie : public Zombie
{
public:
	SmartZombie(double x, double y, StudentWorld* sw) : Zombie(x, y, sw) {}
	virtual void doSomeThing();
	virtual void dieByFallOrBurnIfAppropriate();
	virtual void findNewDir(double otherX, double otherY, double distance);
};

#endif // ACTOR_H_
