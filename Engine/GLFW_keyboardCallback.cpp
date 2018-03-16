#include "globalOpenGL_GLFW.h"
#include "globalGameStuff.h"

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

/*static*/ void key_callback( GLFWwindow* window, int key, int scancode, int action, int mods )
{

	// TEMP ANIMATIONS------------------------------
	cAnimationState::sStateDetails walkForward;
	walkForward.name = "assets/models/rick/Walking.fbx";
	walkForward.totalTime = 0.8f;
	walkForward.frameStepTime = 0.005f;

	cAnimationState::sStateDetails walkBackward;
	walkBackward.name = "assets/models/rick/Walking Backward.fbx";
	walkBackward.totalTime = 0.8f;
	walkBackward.frameStepTime = 0.005f;

	cAnimationState::sStateDetails leftAnimation;
	leftAnimation.name = "assets/models/rick/Left Strafe Walk.fbx";
	leftAnimation.totalTime = 0.8f;
	leftAnimation.frameStepTime = 0.005f;

	cAnimationState::sStateDetails rightAnimation;
	rightAnimation.name = "assets/models/rick/Right Strafe Walk.fbx";
	rightAnimation.totalTime = 0.8f;
	rightAnimation.frameStepTime = 0.005f;

	cAnimationState::sStateDetails jumpAnimation;
	jumpAnimation.name = "assets/models/rick/Jumping.fbx";
	jumpAnimation.totalTime = 1.1f;
	jumpAnimation.frameStepTime = 0.005f;

	cAnimationState::sStateDetails idleAnimation2;
	idleAnimation2.name = "assets/models/rick/Breathing Idle.fbx";
	idleAnimation2.totalTime = 1.1f;
	idleAnimation2.frameStepTime = 0.005f;

	cAnimationState::sStateDetails leftAnimation2;
	leftAnimation2.name = "assets/models/rick/Crouched Sneaking Left.fbx";
	leftAnimation2.totalTime = 0.8f;
	leftAnimation2.frameStepTime = 0.005f;

	cAnimationState::sStateDetails rightAnimation2;
	rightAnimation2.name = "assets/models/rick/Crouched Sneaking Right.fbx";
	rightAnimation2.totalTime = 0.8f;
	rightAnimation2.frameStepTime = 0.005f;

	cAnimationState::sStateDetails actionAnimation;
	actionAnimation.name = "assets/models/rick/Gangnam Style.fbx";
	actionAnimation.totalTime = 1.1f;
	actionAnimation.frameStepTime = 0.005f;

	cAnimationState::sStateDetails runningAnimation;
	runningAnimation.name = "assets/models/rick/Running.fbx";
	runningAnimation.totalTime = 1.1f;
	runningAnimation.frameStepTime = 0.005f;
	// ----------------------------------------------

	MOVEMENT_CHANGE = false;

	if( key == GLFW_KEY_ESCAPE && action == GLFW_PRESS )
		glfwSetWindowShouldClose( window, GLFW_TRUE );

	if( key == GLFW_KEY_SPACE && action == GLFW_PRESS )
	{
		//		::g_GameObjects[1]->position.y += 0.01f;
		//::g_vecGameObjects[1]->position.y += 0.01f;
		::g_pThePlayerGO->pAniState->vecAnimationQueue.push_back( jumpAnimation );
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
			//::g_pLightManager->vecLights[0].position.x -= CAMERASPEED;
			::g_pThePlayerGO->pAniState->vecAnimationQueue.push_back( leftAnimation2 );
		}
		else
		{
			if( isAltKeyDown( mods, true ) )
			{	
			}
			else
			{	// Turn Left
				::g_pThePlayerGO->pAniState->vecAnimationQueue.push_back( leftAnimation );
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
			::g_pThePlayerGO->pAniState->vecAnimationQueue.push_back( rightAnimation2 );
			//::g_pLightManager->vecLights[0].position.x += CAMERASPEED;
		}
		else
		{
			if( isAltKeyDown( mods, true ) )
			{	// F=ma, so changing the accel REALLY is like putting a force
				//	on an object
			
			}
			else
			{	// Turn right 
				::g_pThePlayerGO->pAniState->vecAnimationQueue.push_back( rightAnimation );

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
			::g_pThePlayerGO->pAniState->vecAnimationQueue.push_back( runningAnimation );

		}
		else
		{
			if( isAltKeyDown( mods, true ) )
			{

			}
			else
			{
				::g_pThePlayerGO->pAniState->vecAnimationQueue.push_back( walkForward );
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
				::g_pThePlayerGO->pAniState->vecAnimationQueue.push_back( walkBackward );
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
				::g_pThePlayerGO->pAniState->vecAnimationQueue.push_back( idleAnimation2 );
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
			{	// "roll" left (counter-clockwise)
				//::g_pSteeringManager->ANGLE_CHANGE *= 1.05f;
				::g_pThePlayerGO->pAniState->vecAnimationQueue.push_back( actionAnimation );
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