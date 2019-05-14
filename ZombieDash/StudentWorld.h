#ifndef STUDENTWORLD_H_
#define STUDENTWORLD_H_

#include "GameWorld.h"
#include "Level.h"
//#include "Actor.h"

class Actor;
class Penelope;

#include <list>
//#include <vector>
#include <string>


class StudentWorld : public GameWorld
{
public:

	static const int right = 0;
	static const int left = 180;
	static const int up = 90;
	static const int down = 270;

    StudentWorld(std::string assetPath);
	virtual ~StudentWorld();

    virtual int init();
    virtual int move();
    virtual void cleanUp();

	// Add an actor to the world.
	void addActor(Actor* a);

	// Record that one more citizen on the current level is gone (exited,
	// died, or turned into a zombie).
	void recordCitizenGone();

	// Indicate that the player has finished the level if all citizens
	  // are gone.
	void recordLevelFinishedIfAllCitizensGone();

	// For each actor overlapping a, activate a if appropriate.
	void activateOnAppropriateActors(Actor* a);

	// Determines if penelope can move to specified location
	bool isBlocked(double x, double y, int dir);

	// Determines if the zombie can move to the specified location
	bool isZombieorCitizenBlocked(double x, double y, int dir);

	// Determines the blockages at the specified location
	bool blockages(double x, double y, int dir, Actor* a);

	// Is creation of a flame blocked at the indicated location?
	bool isFlameBlockedAt(double x, double y);

	// Is there something at the indicated location that might cause a
	  // zombie to vomit (i.e., a human)?
	bool isZombieVomitTriggerAt(double x, double y);

	// Return true if there is a living human, otherwise false.  If true,
	 // otherX, otherY, and distance will be set to the location and distance
	 // of the human nearest to (x,y).
	bool locateNearestVomitTrigger(double x, double y, double& otherX, double& otherY, double& distance);

	// Return true if there is a living zombie or Penelope, otherwise false.
	  // If true, otherX, otherY, and distance will be set to the location and
	  // distance of the one nearest to (x,y), and isThreat will be set to true
	  // if it's a zombie, false if a Penelope.
	bool locateNearestCitizenTrigger(double x, double y, double& otherX, double& otherY, double& distance, bool& isThreat);

	// Return true if there is a living zombie, false otherwise.  If true,
	  // otherX, otherY and distance will be set to the location and distance
	  // of the one nearest to (x,y).
	bool locateNearestCitizenThreat(double x, double y, double& otherX, double& otherY, double& distance);

	// Returns true if the object overlaps with any other object
	bool overlapWithAnything(double x, double y);
	
	// Returns true if actors a and b overlap
	bool overlap(Actor* a, Actor* b);

	// Returns true if actor a and the actor at (x,y) overlap
	bool overlapXY(double x, double y, Actor* a);


private:
	Penelope* p = nullptr;
	std::list<Actor*> actors;
	std::list<Actor*> penelope;
	int convertLevel();
	int m_citizens;
	int status;
};

#endif // STUDENTWORLD_H_
