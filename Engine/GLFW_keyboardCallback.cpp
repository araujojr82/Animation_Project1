#include "globalOpenGL_GLFW.h"
#include "globalGameStuff.h"

#include "assimp\cSimpleAssimpSkinnedMeshLoader_OneMesh.h"

#include <iostream>

#include "cAnimationState.h"

extern cSteeringManager* g_pSteeringManager;

bool isShiftKeyDown( int mods, bool bByItself = true );
bool isCtrlKeyDown( int mods, bool bByItself = true );
bool isAltKeyDown( int mods, bool bByItself = true );

bool MOVING_FORWARD = false;
bool MOVING_BACKWARD = false;
bool TURNING_LEFT = false;
bool TURNING_RIGHT = false;
bool MOVEMENT_CHANGE = false;

glm::vec3 movement = glm::vec3( 0.0f );

void changePlayerGO()
{
	for( int i = 0; i != ::g_vecGameObjects.size(); i++ )
	{
		if( ::g_vecGameObjects[i]->type == eTypeOfGO::CHARACTER &&
			::g_vecGameObjects[i]->team == eTeam::PLAYER &&
			::g_vecGameObjects[i] != ::g_pThePlayerGO )
		{ // Found another player GO!
			::g_pThePlayerGO->enemyType = eEnemyType::FOLLOWER;

			::g_pThePlayerGO = ::g_vecGameObjects[i];
			::g_pThePlayerGO->enemyType = eEnemyType::UNAVAIABLE;
			return;
		}

	}
	return;
}


bool createState( cGameObject* pTheGO, eAnimationType type, cAnimationState::sStateDetails &theState )
{
	std::string theAnimPath;
	if( !pTheGO->getAnimationPath( type, theAnimPath ) )
	{
		return false;
	}
	else

	theState.name = theAnimPath;
	theState.totalTime = pTheGO->pSimpleSkinnedMesh->GetDuration( theAnimPath );
	theState.frameStepTime = 0.01f;

	return true;
}

/*static*/ void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods )
{
	MOVEMENT_CHANGE = false;

	if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
		glfwSetWindowShouldClose( window, GLFW_TRUE );

	if( key == GLFW_KEY_TAB && action == GLFW_PRESS )
	{
		changePlayerGO();
	}

	if( key == GLFW_KEY_SPACE && action == GLFW_PRESS )
	{
		//		::g_GameObjects[1]->position.y += 0.01f;
		//::g_vecGameObjects[1]->position.y += 0.01f;
		cAnimationState::sStateDetails newState;
		if( createState( ::g_pThePlayerGO, eAnimationType::JUMP, newState ) )
		{
			::g_pThePlayerGO->pAniState->vecAnimationQueue.push_back( newState );
		}
		
	}
	
	if( key == GLFW_KEY_ENTER && action == GLFW_PRESS )
	{
		::g_bIsWireframe = !::g_bIsWireframe;
	}
	
	const float CAMERASPEED = 0.1f;

	const float CAM_ACCELL_THRUST = 100.0f;

	//	const float CAMERASPEED = 100.0f;
	switch( key )
	{
		// HACK: Change orientation of pLeftTeapot
		// 5,6 - rotation around x
		// 7,8 - rotation around y 
		// 9,0 - rotation around z
	case GLFW_KEY_5:
		//pLeftTeapot->qOrientation.x += 1.0f;		// NOT Euler x axis!!
		//pTheBall->position += glm::vec3( 0.1f, 0.0f, 0.0f );
		//pTheBall->adjustQOrientationFormDeltaEuler( glm::vec3(+0.1f, 0.0f, 0.0f ) );
		break;
	case GLFW_KEY_6:
		//pTheBall->position += glm::vec3( -0.1f, 0.0f, 0.0f ) ;
		//pTheBall->adjustQOrientationFormDeltaEuler( glm::vec3( -0.1f, 0.0f, 0.0f ) );
		break;
	case GLFW_KEY_7:
		//pTheBall->position += glm::vec3( 0.0f, +0.1f, 0.0f );
		//pTheBall->adjustQOrientationFormDeltaEuler( glm::vec3( 0.0f, +0.1f, 0.0f ) );
		break;
	case GLFW_KEY_8:
		//pTheBall->position += glm::vec3( 0.0f, -0.1f, 0.0f );
		//pTheBall->adjustQOrientationFormDeltaEuler( glm::vec3( 0.0f, -0.1f, 0.0f ) );
		break;
	case GLFW_KEY_9:
		//pTheBall->position += glm::vec3( 0.0f, 0.0f, +0.1f );
		break;
	case GLFW_KEY_0:
		//pTheBall->position += glm::vec3( 0.0f, 0.0f, -0.1f );
		::g_pThePlayerGO->vel = glm::vec3( 0.0f );
		break;

	case GLFW_KEY_N:
		break;

	case GLFW_KEY_M:
		break;

	case GLFW_KEY_A:		// Left
		if( isShiftKeyDown( mods, true ) )
		{
			cAnimationState::sStateDetails newState;
			if( createState( ::g_pThePlayerGO, eAnimationType::MOVE_LEFT, newState ) )
			{
				::g_pThePlayerGO->pAniState->vecAnimationQueue.push_back( newState );
			}
		}
		else
		{
			if( isAltKeyDown( mods, true ) )
			{	
			}
			else
			{	// Turn Left
				cAnimationState::sStateDetails newState;
				if( createState( ::g_pThePlayerGO, eAnimationType::STRAFE_LEFT, newState ) )
				{
					::g_pThePlayerGO->pAniState->vecAnimationQueue.push_back( newState );
				}
				//if( action == GLFW_PRESS )
				//{
				//	MOVEMENT_CHANGE = true;
				//	TURNING_LEFT = true;
				//}
				//else if( action == GLFW_RELEASE )
				//{
				//	MOVEMENT_CHANGE = true;
				//	TURNING_LEFT = false;
				//}
				//::g_pSteeringManager->CIRCLE_RADIUS *= 0.95f;
				
			}
		}
		break;
	case GLFW_KEY_D:		// Right

		if( isShiftKeyDown( mods, true ) )
		{
			cAnimationState::sStateDetails newState;
			if( createState( ::g_pThePlayerGO, eAnimationType::MOVE_RIGHT, newState ) )
			{
				::g_pThePlayerGO->pAniState->vecAnimationQueue.push_back( newState );
			}			
		}
		else
		{
			if( isAltKeyDown( mods, true ) )
			{	// F=ma, so changing the accel REALLY is like putting a force
				//	on an object
			
			}
			else
			{	// Turn right 
				cAnimationState::sStateDetails newState;
				if( createState( ::g_pThePlayerGO, eAnimationType::STRAFE_RIGHT, newState ) )
				{
					::g_pThePlayerGO->pAniState->vecAnimationQueue.push_back( newState );
				}
				//if( action == GLFW_PRESS )
				//{
				//	MOVEMENT_CHANGE = true;
				//	TURNING_RIGHT = true;
				//}
				//else if( action == GLFW_RELEASE )
				//{
				//	MOVEMENT_CHANGE = true;
				//	TURNING_RIGHT = false;
				//}
				//::g_pSteeringManager->CIRCLE_RADIUS *= 1.05f;
			}
		}
		break;
	case GLFW_KEY_W:		// Forward
		if( isShiftKeyDown( mods, true ) )
		{
			//::g_pLightManager->vecLights[0].position.z += CAMERASPEED;
			cAnimationState::sStateDetails newState;
			if( createState( ::g_pThePlayerGO, eAnimationType::RUN, newState ) )
			{
				::g_pThePlayerGO->pAniState->vecAnimationQueue.push_back( newState );
			}

		}
		else
		{
			if( isAltKeyDown( mods, true ) )
			{

			}
			else
			{
				cAnimationState::sStateDetails newState;
				if( createState( ::g_pThePlayerGO, eAnimationType::WALK_FORWARD, newState ) )
				{
					::g_pThePlayerGO->pAniState->vecAnimationQueue.push_back( newState );
				}
				//if( action == GLFW_PRESS )
				//{
				//	MOVEMENT_CHANGE = true;
				//	MOVING_FORWARD = true;
				//}
				//else if( action == GLFW_RELEASE )
				//{
				//	MOVEMENT_CHANGE = true;
				//	MOVING_FORWARD = false;
				//}
				//::g_pSteeringManager->CIRCLE_DISTANCE *= 1.05f;
			}
		}
		break;
	case GLFW_KEY_S:		// Backwards
		if( isShiftKeyDown( mods, true ) )
		{
			::g_pLightManager->vecLights[0].position.z -= CAMERASPEED;
		}
		else
		{
			if( isAltKeyDown( mods, true ) )
			{

			}
			else
			{	
				cAnimationState::sStateDetails newState;
				if( createState( ::g_pThePlayerGO, eAnimationType::WALK_BACKWARD, newState ) )
				{
					::g_pThePlayerGO->pAniState->vecAnimationQueue.push_back( newState );
				}
				//if( action == GLFW_PRESS )
				//{
				//	MOVEMENT_CHANGE = true;
				//	MOVING_BACKWARD = true;
				//}
				//else if( action == GLFW_RELEASE )
				//{
				//	MOVEMENT_CHANGE = true;
				//	MOVING_BACKWARD = false;
				//}
				//::g_pSteeringManager->CIRCLE_DISTANCE *= 0.95f;
			}
		}
		break;
	case GLFW_KEY_Q:		// "Down" (along y axis)
		if( isShiftKeyDown( mods, true ) )
		{
			//::g_pLightManager->vecLights[0].position.y -= CAMERASPEED;
		}
		else
		{
			//::g_cameraXYZ.y -= CAMERASPEED;	
			if( isCtrlKeyDown( mods, true ) )
			{
				// Move camera (ctrl)
				
			}
			else
			{	// "roll" left (counter-clockwise)
				//::g_pSteeringManager->ANGLE_CHANGE *= 0.95f;
			}
		}
		break;
	case GLFW_KEY_E:		// "Up" (along y axis)
		if( isShiftKeyDown( mods, true ) )
		{
			::g_pLightManager->vecLights[0].position.y += CAMERASPEED;

		}
		else
		{
			//::g_cameraXYZ.y += CAMERASPEED;	
			if( isCtrlKeyDown( mods, true ) )
			{
				// Move camera (ctrl)
				
			}
			else
			{	
				//::g_pSteeringManager->ANGLE_CHANGE *= 1.05f;
				cAnimationState::sStateDetails newState;
				if( createState( ::g_pThePlayerGO, eAnimationType::ACTION, newState ) )
				{
					::g_pThePlayerGO->pAniState->vecAnimationQueue.push_back( newState );
				}
			}
		}
		break;

	case GLFW_KEY_1:
		::g_pLightManager->vecLights[0].attenuation.y *= 0.99f;	// less 1%
		break;
	case GLFW_KEY_2:
		::g_pLightManager->vecLights[0].attenuation.y *= 1.01f; // more 1%
		if( ::g_pLightManager->vecLights[0].attenuation.y <= 0.0f )
		{
			::g_pLightManager->vecLights[0].attenuation.y = 0.001f;	// Some really tiny value
		}
		break;
	case GLFW_KEY_3:	// Quad
		::g_pLightManager->vecLights[0].attenuation.z *= 0.99f;	// less 1%
		break;
	case GLFW_KEY_4:	//  Quad
		::g_pLightManager->vecLights[0].attenuation.z *= 1.01f; // more 1%
		if( ::g_pLightManager->vecLights[0].attenuation.z <= 0.0f )
		{
			::g_pLightManager->vecLights[0].attenuation.z = 0.001f;	// Some really tiny value
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

	//// HACK: print output to the console
	//	std::cout << "Light[0] linear atten: "
	//		<< ::g_pLightManager->vecLights[0].attenuation.y << ", "
	//		<< ::g_pLightManager->vecLights[0].attenuation.z << std::endl;
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