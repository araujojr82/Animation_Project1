#ifndef _cAnimationState_HG_
#define _cAnimationState_HG_

#include <string>
#include <vector>

extern enum eAnimationType;

class cAnimationState
{
public:
	cAnimationState() {};

	struct sStateDetails 
	{
		sStateDetails() : 
			currentTime( 0.0f ), 
			totalTime( 0.0f ),
			frameStepTime( 0.0f ),
			runInLoop( true ),
			isFinished( false ){};
		std::string name;
		float currentTime;		// Time (frame) in current animation
		float totalTime;		// Total time animation goes
		float frameStepTime;	// Number of seconds to 'move' the animation	
		bool runInLoop;
		bool isFinished;
		// Returns true if time had to be reset
		// (for checking to see if the animation has finished or not)
		// TODO: Deal with running the animation backwards, perhaps?? 
		bool IncrementTime(bool bResetToZero = true);
		
	};
	
	//std::vector<sStateDetails> vecAnimationQueue;
	sStateDetails defaultAnimation;
	sStateDetails currentAnimation;
	eAnimationType currentAnimationType;
};

#endif 

