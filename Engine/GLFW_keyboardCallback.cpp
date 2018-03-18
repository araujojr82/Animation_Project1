#include "globalOpenGL_GLFW.h"
#include "globalGameStuff.h"

#include <iostream>

#include "cAnimationState.h"

extern cSteeringManager* g_pSteeringManager;

bool isShiftKeyDown( int mods, bool bByItself = true );
bool isCtrlKeyDown( int mods, bool bByItself = true );
bool isAltKeyDown( int mods, bool bByItself = true );

bool bIsWKeyDown = false;

extern void setState( eGOState nextAnimation );

void changePlayerGO()
{
	for( int i = 0; i != ::g_vecGameObjects.size(); i++ )
	{
		if( ::g_vecGameObjects[i]->type == eTypeOfGO::CHARACTER &&
			::g_vecGameObjects[i]->team == eTeam::PLAYER &&
			::g_vecGameObjects[i] != ::g_pThePlayerGO )
		{ // Found another player GO!
			::g_pThePlayerGO->behaviour = eBehaviour::FOLLOWER;

			::g_pThePlayerGO = ::g_vecGameObjects[i];
			::g_pThePlayerGO->behaviour = eBehaviour::UNAVAIABLE;
			//::g_pTheMouseCamera->setTarget( ::g_pThePlayerGO );
			//::g_pTheMouseCamera->moveCamera();
			return;
		}

	}
	return;
}

/*static*/ void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods )
{

	if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
		glfwSetWindowShouldClose( window, GLFW_TRUE );

	if( key == GLFW_KEY_TAB && action == GLFW_PRESS )
	{
		changePlayerGO();
	}
	
	if( key == GLFW_KEY_ENTER && action == GLFW_PRESS )
	{
		::g_bIsWireframe = !::g_bIsWireframe;
	}
	
	switch( key )
	{
	case GLFW_KEY_A:		// Left
		if( isShiftKeyDown( mods, true ) )
		{
			if( action == GLFW_PRESS )
			{
				setState( eGOState::MOVING_LEFT );
			}
			if( action == GLFW_RELEASE )
			{
				setState( eGOState::DOING_NOTHING );
			}
		}
		else
		{	
			if( action == GLFW_PRESS )
			{
				setState( eGOState::STRAFING_LEFT );
			}
			if( action == GLFW_RELEASE )
			{
				setState( eGOState::DOING_NOTHING );
			}
		}
		break;
	case GLFW_KEY_D:		// Right

		if( isShiftKeyDown( mods, true ) )
		{
			if( action == GLFW_PRESS )
			{
				setState( eGOState::MOVING_RIGHT );
			}
			if( action == GLFW_RELEASE )
			{
				setState( eGOState::DOING_NOTHING );
			}
		}
		else
		{
			if( action == GLFW_PRESS )
			{
				setState( eGOState::STRAFING_RIGHT );
			}
			if( action == GLFW_RELEASE )
			{
				setState( eGOState::DOING_NOTHING );
			}
		}
		break;
	case GLFW_KEY_W:		// Forward
		if( isShiftKeyDown( mods, true ) )
		{
			if( action == GLFW_PRESS )
			{
				setState( eGOState::RUNNING );
				g_pThePlayerGO->bIsRunning = true;
			}
			if( action == GLFW_RELEASE )
			{
				setState( eGOState::DOING_NOTHING );
				g_pThePlayerGO->bIsRunning = false;
			}
		}
		else
		{
			if( action == GLFW_PRESS )
			{
				setState( eGOState::WALKING_FORWARD );
				g_pThePlayerGO->bIsWalking = true;					
			}
			if( action == GLFW_RELEASE )
			{
				setState( eGOState::DOING_NOTHING );
				g_pThePlayerGO->bIsWalking = false;
			}
		}
		break;
	case GLFW_KEY_S:		// Backwards
		if( action == GLFW_PRESS )
		{
			setState( eGOState::WALKING_BACKWARD );
		}
		if( action == GLFW_RELEASE )
		{
			setState( eGOState::DOING_NOTHING );
		}				
		break;
	case GLFW_KEY_Q:		// Turn Left
		if( action == GLFW_PRESS )
		{
			setState( eGOState::TURNING_LEFT );
		}
		if( action == GLFW_RELEASE )
		{
			setState( eGOState::DOING_NOTHING );
		}
		break;
	case GLFW_KEY_E:		// Turn Right

		if( action == GLFW_PRESS )
		{
			setState( eGOState::TURNING_RIGHT );
		}
		if( action == GLFW_RELEASE )
		{
			setState( eGOState::DOING_NOTHING );
		}
		break;

	case GLFW_KEY_T:		// Perform Action

		if( action == GLFW_PRESS )
		{
			setState( eGOState::DOING_ACTION );
		}
		break;

	case GLFW_KEY_SPACE:
		if( action == GLFW_PRESS )
		{
			if( ::g_pThePlayerGO->bIsWalking )
				setState( eGOState::JUMPING_SHORT );
			
			else if( ::g_pThePlayerGO->bIsRunning )
		    	setState( eGOState::JUMPING_LONG );			
			
			else
				setState( eGOState::JUMPING_STATIC );
			
			return;
		}
		break;

	case GLFW_KEY_UP:
		break;

	case GLFW_KEY_DOWN:
		break;

	case GLFW_KEY_LEFT:		
		break;

	case GLFW_KEY_RIGHT:		
		break;
	}

	return;
}

// Helper functions
bool isShiftKeyDown( int mods, bool bByItself /*=true*/ )
{
	if( bByItself )
	{	// shift by itself?
		if( mods == GLFW_MOD_SHIFT )	{ return true; }
		else							{ return false; }
	}
	else
	{	// shift with anything else, too
		if( ( mods && GLFW_MOD_SHIFT ) == GLFW_MOD_SHIFT )	{ return true; }
		else												{ return false; }
	}
	// Shouldn't never get here, so return false? I guess?
	return false;
}

bool isCtrlKeyDown( int mods, bool bByItself /*=true*/ )
{
	if( bByItself )
	{	// shift by itself?
		if( mods == GLFW_MOD_CONTROL )	{ return true; }
		else							{ return false; }
	}
	else
	{	// shift with anything else, too
		if( ( mods && GLFW_MOD_CONTROL ) == GLFW_MOD_CONTROL )	{ return true; }
		else												{ return false; }
	}
	// Shouldn't never get here, so return false? I guess?
	return false;
}

bool isAltKeyDown( int mods, bool bByItself /*=true*/ )
{
	if( bByItself )
	{	// shift by itself?
		if( mods == GLFW_MOD_ALT ) { return true; }
		else { return false; }
	}
	else
	{	// shift with anything else, too
		if( ( mods && GLFW_MOD_ALT ) == GLFW_MOD_ALT ) { return true; }
		else { return false; }
	}
	// Shouldn't never get here, so return false? I guess?
	return false;
}