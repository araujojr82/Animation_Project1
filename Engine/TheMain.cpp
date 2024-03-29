//Include glad and GLFW in correct order
#include "globalOpenGL_GLFW.h"

#include <iostream>						// C++ cin, cout, etc.
#include <glm/vec3.hpp>					// glm::vec3
#include <glm/vec4.hpp>					// glm::vec4
#include <glm/mat4x4.hpp>				// glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <glm/gtc/type_ptr.hpp>			// glm::value_ptr
#include <glm/gtc/quaternion.hpp>

#include <stdlib.h>
#include <stdio.h>
// Add the file stuff library (file stream>
#include <fstream>
#include <sstream>						// "String stream"f
#include <istream>
#include <string>
#include <vector>						// smart array, "array" in most languages
#include <random>
#include <chrono>

#include "Utilities.h"
#include "ModelUtilities.h"
#include "cMesh.h"
#include "cShaderManager.h" 
#include "cGameObject.h"
#include "cVAOMeshManager.h"

#include "Physics.h"

#include "cLightManager.h"

// Include all the things that are accessed in other files
#include "globalGameStuff.h"	

#include "commands\cCommandScheduler.h"
#include "commands\cComMoveTo.h"

#include "cAnimationState.h"
#include "assimp\cSimpleAssimpSkinnedMeshLoader_OneMesh.h"

//#include "cSteeringManager.h"

#include "cFBO.h" 
cFBO g_myFBO;
cFBO g_myMirrorFBO;

// The Game Boundaries:
const float maxX = 15.0f;
const float minX = -15.0f;
const float maxZ = 15.0f;
const float minZ = -15.0f;

cSteeringManager* g_pSteeringManager = NULL;

// Euclides: Control selected object for movement
int g_GameObjNumber = 0;				// game object vector position number 
int g_LightObjNumber = 0;				// light object vector position

bool g_lookAtON = false;
bool g_bIsWireframe = false;

cCommandScheduler g_theScheduler;

int g_NUMBER_OF_LIGHTS = 1;

std::vector< cGameObject* > g_vecGameObjects;

cGameObject* g_pThePlayerGO = NULL;
cGameObject* g_pSkyBoxObject = NULL;
cGameObject* g_pMirrorObject = NULL;
cGameObject* g_pSphereObject = NULL;

//cCamera* g_pTheCamera = NULL;
cMouseCamera* g_pTheMouseCamera = NULL;
float g_lastX = 0.0f;
float g_lastZ = 0.0f;
bool g_firstMouse = true;

cVAOMeshManager*	g_pVAOManager = 0;		// or NULL or nullptr

cShaderManager*		g_pShaderManager;		// Heap, new (and delete)
cLightManager*		g_pLightManager;

//cBasicTextureManager*	g_pTextureManager = 0;
CTextureManager*	g_pTextureManager = 0;

cModelAssetLoader*		g_pModelAssetLoader = NULL;

cDebugRenderer*			g_pDebugRenderer = 0;

bool g_IsWindowFullScreen = false;
GLFWwindow* g_pGLFWWindow = NULL;

bool g_bUseDeferred = true; // Switch between 1 pass or 2 passes...
bool g_bIsSecondPass = false;
const int RENDER_PASS_0 = 0;
const int RENDER_PASS_1 = 1;
const int RENDER_PASS_2 = 2;

GLint g_renderID = 0;

unsigned int g_drunkEffect = 0;
float g_drunkOffset = 0.0f;
float g_drunkOffsetChange = 0.0001f;
bool g_outline = false;

// This contains the AABB grid for the terrain...
//cAABBBroadPhase* g_terrainAABBBroadPhase = 0;

// Other uniforms:
//GLint uniLoc_materialDiffuse = -1;
//GLint uniLoc_materialAmbient = -1;
//GLint uniLoc_ambientToDiffuseRatio = -1; 	// Maybe	// 0.2 or 0.3
//GLint uniLoc_materialSpecular = -1;  // rgb = colour of HIGHLIGHT only
//									 // w = shininess of the 
//GLint uniLoc_bIsDebugWireFrameObject = -1;
//
//GLint uniLoc_eyePosition = -1;	// Camera position
//GLint uniLoc_mModel = -1;
//GLint uniLoc_mView = -1;
//GLint uniLoc_mProjection = -1;

struct sWindowConfig
{
public:
	int height = 480;
	int width = 640;
	std::string title = "TO BE REPLACED...";
};

struct sGOparameters		// for the Game Objects' input file
{
	std::string meshname;
	int nObjects;
	float x, y, z, scale;
	std::string random;
	float rangeX, rangeY, rangeZ, rangeScale;
};

struct sMeshparameters		// for the Meshes' input file
{
	std::string meshname;
	std::string meshFilename;
};

std::vector< pointTriangles > g_vecPoints;

// Forward declare the Functions
void loadConfigFile( std::string fileName, sWindowConfig& wConfig );
void loadLightObjects();
//void PhysicsStep( double deltaTime );
void updateAllObjects( double deltaTime );
//void newPlayerGO();
//void DrawObject( cGameObject* pTheGO );
float generateRandomNumber( float min, float max );
//void updateCamera( double deltaTime );
void mouse_callback( GLFWwindow* window, double xpos, double ypos );
void scroll_callback( GLFWwindow* window, double xoffset, double yoffset );
void ProcessCameraInput( GLFWwindow *window, double deltaTime );

static void error_callback( int error, const char* description )
{
	fprintf( stderr, "Error: %s\n", description );
}

//void turn_player( double deltaTime )
//{
//
//}

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
	theState.frameStepTime = 0.02f;

	return true;
}


void updateCurrentAnimation( eAnimationType type, bool loopAnim = true )
{
	cAnimationState* pTheAniState = ::g_pThePlayerGO->pAniState;

	if( pTheAniState->currentAnimationType != type )
	{
		if( type != NOTHING )
		{
			int Mybreak = 1;
		}

		cAnimationState::sStateDetails newState;
		if( !createState( ::g_pThePlayerGO, type, newState ) )
		{
			newState.name = "none";
		}
		newState.runInLoop = loopAnim;
		if( type == NOTHING )
		{
			if( pTheAniState->currentAnimationType == JUMP )
			{
				if( pTheAniState->currentAnimation.isFinished )
				{
					pTheAniState->currentAnimation = newState;
					pTheAniState->currentAnimationType = type;
				}
			}
			else
			{
				pTheAniState->currentAnimation = newState;
				pTheAniState->currentAnimationType = type;
			}
		}
		else
		{
			pTheAniState->currentAnimation = newState;
			pTheAniState->currentAnimationType = type;
		}
	}

	return;
}

void setState( eGOState nextState )
{
	eGOState theNextState = nextState;

	bool isFinished = ::g_pThePlayerGO->pAniState->currentAnimation.isFinished;
	eAnimationType currentAnimationType = ::g_pThePlayerGO->pAniState->currentAnimationType;

	if( currentAnimationType == eAnimationType::JUMP && isFinished )
	{
		theNextState = eGOState::DEFAULT;
		::g_pThePlayerGO->currentState = eGOState::DEFAULT;
	}

	if( theNextState != ::g_pThePlayerGO->currentState )
	{
		switch( ::g_pThePlayerGO->currentState )
		{
			case DEFAULT:	// IDLE
				::g_pThePlayerGO->currentState = theNextState;
				break;
			case JUMPING_STATIC:
			case JUMPING_SHORT:
			case JUMPING_LONG:
				if( isFinished )
					::g_pThePlayerGO->currentState = theNextState;
				// TODO: ALLOW ROTATION
				break;
			case STRAFING_LEFT:
				::g_pThePlayerGO->currentState = theNextState;
				break;
			case STRAFING_RIGHT:
				::g_pThePlayerGO->currentState = theNextState;
				break;
			case MOVING_LEFT:
				::g_pThePlayerGO->currentState = theNextState;
				break;
			case MOVING_RIGHT:
				::g_pThePlayerGO->currentState = theNextState;
				break;
			case TURNING_LEFT:
				::g_pThePlayerGO->currentState = theNextState;
				break;
			case TURNING_RIGHT:
				::g_pThePlayerGO->currentState = theNextState;
				break;
			case RUNNING:
				//if( nextAnimation == WALK_FORWARD ||
				//	nextAnimation == JUMP ||
				//	nextAnimation == NOTHING )
				::g_pThePlayerGO->currentState = theNextState;
				break;
			case WALKING_FORWARD:
				::g_pThePlayerGO->currentState = theNextState;
				break;
			case WALKING_BACKWARD:
				if( theNextState != JUMPING_STATIC &&
					theNextState != JUMPING_LONG &&
					theNextState != JUMPING_SHORT )	// CAN'T JUMP BACK!
					::g_pThePlayerGO->currentState = theNextState;
				break;
			case DOING_ACTION:
				::g_pThePlayerGO->currentState = theNextState;
				break;
			case DOING_NOTHING:
				::g_pThePlayerGO->currentState = theNextState;
				break;
		}

	}
	return;
}

void move_player()
{
	bool isFinished = ::g_pThePlayerGO->pAniState->currentAnimation.isFinished;
	eAnimationType currentAnimationType = ::g_pThePlayerGO->pAniState->currentAnimationType;

	switch( ::g_pThePlayerGO->currentState )
	{
		case DEFAULT:
			updateCurrentAnimation( eAnimationType::BASE );
			break;
		case JUMPING_STATIC:
			if( isFinished )
			{
				::g_pThePlayerGO->currentState = eGOState::DEFAULT;
				updateCurrentAnimation( eAnimationType::BASE );
			}
			else
			{
				updateCurrentAnimation( eAnimationType::JUMP, false );
			}
			break;
		case JUMPING_SHORT:
		case JUMPING_LONG:
			if( isFinished )
			{
				if( ::g_pThePlayerGO->bIsWalking )
				{
					::g_pThePlayerGO->currentState = eGOState::WALKING_FORWARD;
					updateCurrentAnimation( eAnimationType::WALK_FORWARD );
				}
				else if( ::g_pThePlayerGO->bIsRunning )
				{
					::g_pThePlayerGO->currentState = eGOState::RUNNING;
					updateCurrentAnimation( eAnimationType::RUN );
				}
				else
				{
					::g_pThePlayerGO->currentState = eGOState::DEFAULT;
					updateCurrentAnimation( eAnimationType::BASE );
				}
			}
			else
			{
				updateCurrentAnimation( eAnimationType::FORWARD_JUMP, false );
			}
			break;

		case STRAFING_LEFT:
			updateCurrentAnimation( eAnimationType::STRAFE_LEFT );
			break;
		case STRAFING_RIGHT:
			updateCurrentAnimation( eAnimationType::STRAFE_RIGHT );
			break;
		case MOVING_LEFT:
			updateCurrentAnimation( eAnimationType::MOVE_LEFT );
			break;
		case MOVING_RIGHT:
			updateCurrentAnimation( eAnimationType::MOVE_RIGHT );
			break;
		case TURNING_LEFT:
			updateCurrentAnimation( eAnimationType::TURN_LEFT );
			break;
		case TURNING_RIGHT:
			updateCurrentAnimation( eAnimationType::TURN_RIGHT );
			break;
		case RUNNING:
			updateCurrentAnimation( eAnimationType::RUN );
			break;
		case WALKING_FORWARD:
			updateCurrentAnimation( eAnimationType::WALK_FORWARD );
			break;
		case WALKING_BACKWARD:
			updateCurrentAnimation( eAnimationType::WALK_BACKWARD );
			break;
		case DOING_ACTION:
			updateCurrentAnimation( eAnimationType::ACTION, false );
			break;
		case DOING_NOTHING:
			updateCurrentAnimation( eAnimationType::NOTHING );
			break;
		default:
			break;
	}


	float totalVelocity = 0.0f;
	glm::vec3 movement = glm::vec3( 0.0f );

	if( isnan( ::g_pThePlayerGO->vel.x ) ) ::g_pThePlayerGO->vel.x = 0.0f;
	if( isnan( ::g_pThePlayerGO->vel.y ) ) ::g_pThePlayerGO->vel.y = 0.0f;
	if( isnan( ::g_pThePlayerGO->vel.z ) ) ::g_pThePlayerGO->vel.z = 0.0f;

	if( ::g_pThePlayerGO->currentState == eGOState::MOVING_RIGHT ||
		::g_pThePlayerGO->currentState == eGOState::STRAFING_RIGHT )
	{
		float maxSpeed = ::g_pThePlayerGO->mySpeed.right * ::g_pThePlayerGO->scale;

		movement = ::g_pThePlayerGO->getRightVector() * -0.01f;
		::g_pThePlayerGO->vel += movement;

		if( abs( ::g_pThePlayerGO->vel.x ) < 0.001 ) ::g_pThePlayerGO->vel.x = 0.0f;
		if( abs( ::g_pThePlayerGO->vel.y ) < 0.001 ) ::g_pThePlayerGO->vel.y = 0.0f;
		if( abs( ::g_pThePlayerGO->vel.z ) < 0.001 ) ::g_pThePlayerGO->vel.z = 0.0f;

		totalVelocity = abs( ::g_pThePlayerGO->vel.x ) +
			abs( ::g_pThePlayerGO->vel.y ) +
			abs( ::g_pThePlayerGO->vel.z );
		if( totalVelocity > maxSpeed )
		{
			::g_pThePlayerGO->vel = glm::normalize( ::g_pThePlayerGO->vel ) * maxSpeed;
		}

	}
	else if( ::g_pThePlayerGO->currentState == eGOState::MOVING_LEFT ||
			 ::g_pThePlayerGO->currentState == eGOState::STRAFING_LEFT )
	{
		float maxSpeed = ::g_pThePlayerGO->mySpeed.left * ::g_pThePlayerGO->scale;

		movement = ::g_pThePlayerGO->getRightVector() * 0.01f;

		::g_pThePlayerGO->vel += movement;

		if( abs( ::g_pThePlayerGO->vel.x ) < 0.001 ) ::g_pThePlayerGO->vel.x = 0.0f;
		if( abs( ::g_pThePlayerGO->vel.y ) < 0.001 ) ::g_pThePlayerGO->vel.y = 0.0f;
		if( abs( ::g_pThePlayerGO->vel.z ) < 0.001 ) ::g_pThePlayerGO->vel.z = 0.0f;

		totalVelocity = abs( ::g_pThePlayerGO->vel.x ) +
			abs( ::g_pThePlayerGO->vel.y ) +
			abs( ::g_pThePlayerGO->vel.z );
		if( totalVelocity > maxSpeed )
		{
			::g_pThePlayerGO->vel = glm::normalize( ::g_pThePlayerGO->vel ) * maxSpeed;
		}
	}
	else if( ::g_pThePlayerGO->currentState == eGOState::WALKING_FORWARD ||
			 ::g_pThePlayerGO->currentState == eGOState::RUNNING ||
			 ::g_pThePlayerGO->currentState == eGOState::JUMPING_SHORT ||
			 ::g_pThePlayerGO->currentState == eGOState::JUMPING_LONG )
	{
		float maxSpeed = ::g_pThePlayerGO->mySpeed.forward * ::g_pThePlayerGO->scale;

		if( ::g_pThePlayerGO->currentState == eGOState::RUNNING ||
			::g_pThePlayerGO->currentState == eGOState::JUMPING_SHORT )
			maxSpeed *= 2.0f;

		if( ::g_pThePlayerGO->currentState == eGOState::JUMPING_LONG )
			maxSpeed *= 4.0f;

		movement = ::g_pThePlayerGO->getFrontVector() * 0.01f;

		::g_pThePlayerGO->vel += movement;

		if( abs( ::g_pThePlayerGO->vel.x ) < 0.001 ) ::g_pThePlayerGO->vel.x = 0.0f;
		if( abs( ::g_pThePlayerGO->vel.y ) < 0.001 ) ::g_pThePlayerGO->vel.y = 0.0f;
		if( abs( ::g_pThePlayerGO->vel.z ) < 0.001 ) ::g_pThePlayerGO->vel.z = 0.0f;

		totalVelocity = abs( ::g_pThePlayerGO->vel.x ) +
			abs( ::g_pThePlayerGO->vel.y ) +
			abs( ::g_pThePlayerGO->vel.z );
		if( totalVelocity > maxSpeed )
		{
			::g_pThePlayerGO->vel = glm::normalize( ::g_pThePlayerGO->vel ) * maxSpeed;
		}
	}
	else if( ::g_pThePlayerGO->currentState == eGOState::WALKING_BACKWARD )
	{
		float maxSpeed = ::g_pThePlayerGO->mySpeed.backward * ::g_pThePlayerGO->scale;

		movement = ::g_pThePlayerGO->getFrontVector() * 0.01f;
		::g_pThePlayerGO->vel -= movement;

		if( abs( ::g_pThePlayerGO->vel.x ) < 0.001 ) ::g_pThePlayerGO->vel.x = 0.0f;
		if( abs( ::g_pThePlayerGO->vel.y ) < 0.001 ) ::g_pThePlayerGO->vel.y = 0.0f;
		if( abs( ::g_pThePlayerGO->vel.z ) < 0.001 ) ::g_pThePlayerGO->vel.z = 0.0f;

		totalVelocity = abs( ::g_pThePlayerGO->vel.x ) +
			abs( ::g_pThePlayerGO->vel.y ) +
			abs( ::g_pThePlayerGO->vel.z );
		if( totalVelocity > maxSpeed )
		{
			::g_pThePlayerGO->vel = glm::normalize( ::g_pThePlayerGO->vel ) * maxSpeed;
		}
	}
	else
	{
		movement = ::g_pThePlayerGO->vel * 0.5f;
		::g_pThePlayerGO->vel -= movement;

		if( abs( ::g_pThePlayerGO->vel.x ) < 0.001 ) ::g_pThePlayerGO->vel.x = 0.0f;
		if( abs( ::g_pThePlayerGO->vel.y ) < 0.001 ) ::g_pThePlayerGO->vel.y = 0.0f;
		if( abs( ::g_pThePlayerGO->vel.z ) < 0.001 ) ::g_pThePlayerGO->vel.z = 0.0f;
	}
	::g_pThePlayerGO->position += ::g_pThePlayerGO->vel;

	if( ::g_pThePlayerGO->currentState == TURNING_RIGHT )
		::g_pThePlayerGO->adjustQOrientationFormDeltaEuler( glm::vec3( 0.0f, -0.02f, 0.0f ) );
	if( ::g_pThePlayerGO->currentState == TURNING_LEFT )
		::g_pThePlayerGO->adjustQOrientationFormDeltaEuler( glm::vec3( 0.0f, 0.02f, 0.0f ) );


	return;
}

//void drawCapsule( glm::vec3 position )
//{
//	cGameObject* capsuleGO = new cGameObject();
//	capsuleGO->meshName = "capsule";
//	capsuleGO->bIsWireFrame = true;
//	capsuleGO->position = position;
//	DrawObject( capsuleGO );
//	delete capsuleGO;
//}

void drawTagCircle( cGameObject* pTheGO )
{
	//cGameObject* tagCircleGO = new cGameObject();
	//tagCircleGO->meshName = "circle";
	//tagCircleGO->textureBlend[0] = 1.0f;
	//tagCircleGO->position = pTheGO->position;

	//switch( pTheGO->enemyType )
	//{
	//	case ANGRY:
	//	{
	//		if( pTheGO->behaviour == eEnemyBehaviour::SEEK )
	//		{
	//			tagCircleGO->textureNames[0] = "red.bmp";
	//			::g_pDebugRenderer->addCircle( pTheGO->position, pTheGO->range, glm::vec3( 1.0f, 0.0f, 0.0f ) );
	//		}
	//		else if( pTheGO->behaviour == eEnemyBehaviour::FLEE )
	//		{
	//			tagCircleGO->textureNames[0] = "orange.bmp";
	//			::g_pDebugRenderer->addCircle( pTheGO->position, pTheGO->range, glm::vec3( 1.0f, 0.5f, 0.15f ) );
	//		}
	//		else
	//		{
	//			tagCircleGO->textureNames[0] = "white.bmp";
	//			::g_pDebugRenderer->addCircle( pTheGO->position, pTheGO->range, glm::vec3( 1.0f, 1.0f, 1.0f ) );
	//		}
	//	}
	//	break;

	//	case CURIOUS:
	//	{
	//		if( pTheGO->behaviour == eEnemyBehaviour::APPROACH )
	//		{
	//			tagCircleGO->textureNames[0] = "blue.bmp";
	//			::g_pDebugRenderer->addCircle( pTheGO->position, pTheGO->range, glm::vec3( 0.0f, 0.0f, 1.0f ) );
	//		}
	//		else if( pTheGO->behaviour == eEnemyBehaviour::EVADE )
	//		{
	//			tagCircleGO->textureNames[0] = "purple.bmp";
	//			::g_pDebugRenderer->addCircle( pTheGO->position, pTheGO->range, glm::vec3( 0.5f, 0.25f, 0.5f ) );
	//		}
	//		else
	//		{
	//			tagCircleGO->textureNames[0] = "gray.bmp";
	//			::g_pDebugRenderer->addCircle( pTheGO->position, pTheGO->range, glm::vec3( 0.5f, 0.5f, 0.5f ) );
	//		}
	//	}
	//	break;

	//	case FOLLOWER:
	//		if( pTheGO->behaviour == eEnemyBehaviour::SEEK )
	//		{
	//			tagCircleGO->textureNames[0] = "green.bmp";
	//			::g_pDebugRenderer->addCircle( pTheGO->position, pTheGO->range, glm::vec3( 0.0f, 1.0f, 0.0f ) );
	//		}
	//		else
	//		{
	//			tagCircleGO->textureNames[0] = "yellow.bmp";
	//			::g_pDebugRenderer->addCircle( pTheGO->position, pTheGO->range, glm::vec3( 1.0f, 1.0f, 0.0f ) );
	//		}
	//		break;

	//	case UNAVAIABLE:
	//		break;
	//}

	//DrawObject( tagCircleGO );
	//delete tagCircleGO;
}


void checkBoundaries( cGameObject* pTheGO )
{
	if( pTheGO->position.x > maxX ) pTheGO->position.x = minX;
	if( pTheGO->position.x < minX ) pTheGO->position.x = maxX;
	if( pTheGO->position.z > maxZ ) pTheGO->position.z = minZ;
	if( pTheGO->position.z < minZ ) pTheGO->position.z = maxZ;
}


void collisionCheck()
{
	unsigned int sizeOfVector = ( unsigned int )g_vecGameObjects.size();

	for( int i = 0; i != sizeOfVector; i++ )
	{
		cGameObject* pTheOtherGO = ::g_vecGameObjects[i];
		if( pTheOtherGO->type != eTypeOfGO::CHARACTER ) continue;

		if( ::g_pThePlayerGO == pTheOtherGO ) continue;	// Don't test itself

		float distance = glm::distance( ::g_pThePlayerGO->position, pTheOtherGO->position );
		if( distance < 0.5f )	//They are overlapping
		{
			//Decrease player health by 0.01
			::g_pThePlayerGO->health -= 0.1f;
			if( ::g_pThePlayerGO->health < 0 ) ::g_pThePlayerGO->health = 0.0f;
		}
	}

	return;
}

int main( void )
{
	glfwSetErrorCallback( error_callback );

	if( !glfwInit() )
	{
		std::cout << "ERROR: Couldn't init GLFW, so we're pretty much stuck; do you have OpenGL??" << std::endl;
		return -1;
	}

	int height = 480;	/* default */
	int width = 640;	// default
	std::string title = "OpenGL Rocks";

	sWindowConfig wConfig;

	loadConfigFile( "config.txt", wConfig );
	//loadObjectsFile( "objects.txt" );

	glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 2 );
	glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 0 );

	::g_pGLFWWindow = glfwCreateWindow( wConfig.width, wConfig.height,
										wConfig.title.c_str(),
										NULL, // glfwGetPrimaryMonitor(), //
										NULL );

	if( !::g_pGLFWWindow )
	{
		glfwTerminate();
		exit( EXIT_FAILURE );
	}

	glfwSetKeyCallback( ::g_pGLFWWindow, key_callback );
	// For the FBO to resize when the window changes
	glfwSetWindowSizeCallback( ::g_pGLFWWindow, window_size_callback );

	glfwMakeContextCurrent( ::g_pGLFWWindow );
	gladLoadGLLoader( ( GLADloadproc )glfwGetProcAddress );
	glfwSwapInterval( 1 );

	glfwSetCursorPosCallback( ::g_pGLFWWindow, mouse_callback );
	glfwSetInputMode( ::g_pGLFWWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
	glfwSetScrollCallback( ::g_pGLFWWindow, scroll_callback );

	std::cout << glGetString( GL_VENDOR ) << " "
		<< glGetString( GL_RENDERER ) << ", "
		<< glGetString( GL_VERSION ) << std::endl;
	std::cout << "Shader language version: " << glGetString( GL_SHADING_LANGUAGE_VERSION ) << std::endl;

	// General error string, used for a number of items during start up
	std::string error;

	::g_pShaderManager = new cShaderManager();

	cShaderManager::cShader vertShader;
	cShaderManager::cShader fragShader;

	vertShader.fileName = "simpleVert.glsl";
	//fragShader.fileName = "simpleFrag.glsl";
	fragShader.fileName = "simpleFragDeferred.glsl";

	::g_pShaderManager->setBasePath( "assets//shaders//" );

	// Shader objects are passed by reference so that
	//	we can look at the results if we wanted to. 
	if( !::g_pShaderManager->createProgramFromFile(
		"mySexyShader", vertShader, fragShader ) )
	{
		std::cout << "Oh no! All is lost!!! Blame Loki!!!" << std::endl;
		std::cout << ::g_pShaderManager->getLastError() << std::endl;

		return -1;
	}
	std::cout << "The shaders compiled and linked OK" << std::endl;

	::g_pDebugRenderer = new cDebugRenderer();
	if( !::g_pDebugRenderer->initialize( error ) )
	{
		std::cout << "Warning: couldn't init the debug renderer." << std::endl;
	}

	//Load models
	::g_pModelAssetLoader = new cModelAssetLoader();
	::g_pModelAssetLoader->setBasePath( "assets/models/" );

	::g_pVAOManager = new cVAOMeshManager();

	GLint currentProgID = ::g_pShaderManager->getIDFromFriendlyName( "mySexyShader" );
	GLint sexyShaderID = ::g_pShaderManager->getIDFromFriendlyName( "mySexyShader" );

	//if( !Load3DModelsIntoMeshManager( sexyShaderID, ::g_pVAOManager, ::g_pModelAssetLoader, error ) )
	//{
	//	std::cout << "Not all models were loaded..." << std::endl;
	//	std::cout << error << std::endl;
	//}

	// LOAD THE ANIMATED MODELS AND ADD TO THE SCENE
	LoadModelsIntoScene( sexyShaderID, ::g_pVAOManager );


	// Named unifrom block
	// Now many uniform blocks are there? 
	GLint numberOfUniformBlocks = -1;
	glGetProgramiv( currentProgID, GL_ACTIVE_UNIFORM_BLOCKS, &numberOfUniformBlocks );

	// https://www.opengl.org/wiki/GLAPI/glGetActiveUniformBlock

	// Set aside some buffers for the names of the blocks
	const int BUFFERSIZE = 1000;

	int name_length_written = 0;

	char NUBName_0[BUFFERSIZE] = { 0 };
	char NUBName_1[BUFFERSIZE] = { 0 };

	glGetActiveUniformBlockName( currentProgID,
								 0,
								 BUFFERSIZE,
								 &name_length_written,
								 NUBName_0 );

	glGetActiveUniformBlockName( currentProgID,
								 1,
								 BUFFERSIZE,
								 &name_length_written,
								 NUBName_1 );

	//	NUB_lighting
	//	NUB_perFrame

	GLuint NUB_Buffer_0_ID = 0;
	GLuint NUB_Buffer_1_ID = 0;

	glGenBuffers( 1, &NUB_Buffer_0_ID );
	glBindBuffer( GL_UNIFORM_BUFFER, NUB_Buffer_0_ID );

	glGenBuffers( 1, &NUB_Buffer_1_ID );
	glBindBuffer( GL_UNIFORM_BUFFER, NUB_Buffer_1_ID );

	::g_pLightManager = new cLightManager();

	::g_pLightManager->CreateLights( g_NUMBER_OF_LIGHTS );

	// Change Other lights parameters==========================
	{
		::g_pLightManager->vecLights[0].position = glm::vec3( 50.0f, 50.0f, 50.0f );
		::g_pLightManager->vecLights[0].attenuation.y = 0.03f;	// Linear		
	}
	//=========================================================
	::g_pLightManager->LoadShaderUniformLocations( currentProgID );

	//loadLightObjects();

	// Texture 
	::g_pTextureManager = new CTextureManager();

	std::cout << "GL_MAX_TEXTURE_IMAGE_UNITS: " << ::g_pTextureManager->getOpenGL_GL_MAX_TEXTURE_IMAGE_UNITS() << std::endl;
	std::cout << "GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS: " << ::g_pTextureManager->getOpenGL_GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS() << std::endl;

	::g_pTextureManager->setBasePath( "assets/textures" );
	if( !::g_pTextureManager->Create2DTextureFromBMPFile( "rick_texture.bmp", true ) )
	{
		std::cout << "Didn't load the texture. Oh no!" << std::endl;
	}
	else
	{
		std::cout << "Texture is loaded! Hazzah!" << std::endl;
	}
	::g_pTextureManager->Create2DTextureFromBMPFile( "morty.bmp", true );
	::g_pTextureManager->Create2DTextureFromBMPFile( "meeseeks.bmp", true );
	::g_pTextureManager->Create2DTextureFromBMPFile( "grass1.bmp", true );
	::g_pTextureManager->Create2DTextureFromBMPFile( "teste1.bmp", true );
	//::g_pTextureManager->Create2DTextureFromBMPFile( "red.bmp", true );	
	//::g_pTextureManager->Create2DTextureFromBMPFile( "blue.bmp", true );
	//::g_pTextureManager->Create2DTextureFromBMPFile( "white.bmp", true );
	//::g_pTextureManager->Create2DTextureFromBMPFile( "orange.bmp", true );
	::g_pTextureManager->Create2DTextureFromBMPFile( "purple.bmp", true );
	//::g_pTextureManager->Create2DTextureFromBMPFile( "gray.bmp", true );
	//::g_pTextureManager->Create2DTextureFromBMPFile( "green.bmp", true );
	//::g_pTextureManager->Create2DTextureFromBMPFile( "yellow.bmp", true );

	::g_pTextureManager->setBasePath( "assets/textures/skybox" );
	if( !::g_pTextureManager->CreateCubeTextureFromBMPFiles(
		"space",
		"face-r.bmp",						//posX_fileName			
		"face-l.bmp",						//negX_fileName											
		"face-d.bmp",		//BOTOM			//posY_fileName
		"face-t.bmp",		// TOP			//negY_fileName
		"face-f.bmp",						//posZ_fileName
		"face-b.bmp",						//negZ_fileName
		true, true ) )
	{
		std::cout << "Didn't load skybox" << std::endl;
	}

	//// THE AABB GENERATION --------------------------

	//// About the generate the AABB for the terrain
	//::g_terrainAABBBroadPhase = new cAABBBroadPhase();
	//// Perhaps you'd like something more sophisticated than this...
	//::g_terrainAABBBroadPhase->pDebugRenderer = ::g_pDebugRenderer;
	//
	//cMesh terrainMesh;
	//
	//if( ::g_pVAOManager->lookupMeshFromName( "map", terrainMesh ) )
	//{
	//	std::cout << "Generating the terrain AABB grid. This will take a moment..." << std::endl;

	//	::g_terrainAABBBroadPhase->genAABBGridFromMesh( terrainMesh );

	//}
	//// END OF THE AABB GENERATION -------------------

	::g_pSteeringManager = new cSteeringManager();

	::g_pThePlayerGO = findObjectByFriendlyName( PLAYERNAME, ::g_vecGameObjects );

	glm::vec3 camPos = ::g_pThePlayerGO->position + glm::vec3( 0.0f, 2.0f, -6.0f );
	::g_pTheMouseCamera = new cMouseCamera( camPos, glm::vec3( 0.0f, 1.0f, 0.0f ), 90.f, -20.f );

	glEnable( GL_DEPTH );

	// Create an FBO
	//	if ( ! g_myFBO.init(width, height, error) )
	if( !g_myFBO.init( 1920, 1080, error ) )
	{
		std::cout << "FBO error: " << error << std::endl;
	}
	else
	{
		std::cout << "FBO is good." << std::endl;
		std::cout << "\tFBO ID = " << g_myFBO.ID << std::endl;
		std::cout << "\tcolour texture ID = " << g_myFBO.colourTexture_0_ID << std::endl;
		std::cout << "\tnormal texture ID = " << g_myFBO.normalTexture_1_ID << std::endl;

		std::cout << "GL_MAX_COLOR_ATTACHMENTS = " << g_myFBO.getMaxColourAttachments() << std::endl;
		std::cout << "GL_MAX_DRAW_BUFFERS = " << g_myFBO.getMaxDrawBuffers() << std::endl;

	}

	// Create an FBO
	//	if ( ! g_myFBO.init(width, height, error) )
	if( !g_myMirrorFBO.init( 1920, 1080, error ) )
	{
		std::cout << "Mirror FBO error: " << error << std::endl;
	}
	else
	{
		std::cout << "Mirror FBO is good." << std::endl;
		std::cout << "\tFBO ID = " << g_myMirrorFBO.ID << std::endl;
		std::cout << "\tcolour texture ID = " << g_myMirrorFBO.colourTexture_0_ID << std::endl;
		std::cout << "\tnormal texture ID = " << g_myMirrorFBO.normalTexture_1_ID << std::endl;

		std::cout << "GL_MAX_COLOR_ATTACHMENTS = " << g_myMirrorFBO.getMaxColourAttachments() << std::endl;
		std::cout << "GL_MAX_DRAW_BUFFERS = " << g_myMirrorFBO.getMaxDrawBuffers() << std::endl;

	}

	setWindowFullScreenOrWindowed( ::g_pGLFWWindow, ::g_IsWindowFullScreen );


	// Gets the "current" time "tick" or "step"
	double lastTimeStep = glfwGetTime();

	//drawRange();

	// Main game or application loop
	while( !glfwWindowShouldClose( ::g_pGLFWWindow ) )
	{
		// Essentially the "frame time"
		// Now many seconds that have elapsed since we last checked
		double curTime = glfwGetTime();
		double deltaTime = curTime - lastTimeStep;
		lastTimeStep = curTime;

		move_player();

		::g_pSteeringManager->updateAll( deltaTime );

		// Check for collisions with the player and update it's health
		collisionCheck();

		::g_pShaderManager->useShaderProgram( "mySexyShader" );

		// Check to see if we are using Deferred Renderer
		if( !::g_bUseDeferred )
		{
			// Direct everything to the FBO
			//GLint bIsSecondPassLocID = glGetUniformLocation( sexyShaderID, "bIsSecondPass" );
			//glUniform1i( bIsSecondPassLocID, GL_FALSE );
			// Direct everything to the FBO
			GLint renderPassNumber_LocID = glGetUniformLocation( sexyShaderID, "renderPassNumber" );
			glUniform1i( renderPassNumber_LocID, RENDER_PASS_0 );

			glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			// Clear colour AND depth buffer
			g_myFBO.clearBuffers();

			RenderScene( ::g_vecGameObjects, ::g_pGLFWWindow, deltaTime );
		}
		else
		{	// Using the Deferred Renderer

			////// ##########################################
			////g_myMirrorFBO

			//glm::vec3 tempPosition = ::g_pTheMouseCamera->Position;
			//// rotate the camera around (to mirror position)
			//::g_pTheMouseCamera->Position = ::g_pMirrorObject->position;
			//::g_pTheMouseCamera->Yaw += 180.0f;
			//::g_pTheMouseCamera->Pitch = -::g_pTheMouseCamera->Pitch;
			//::g_pTheMouseCamera->ProcessMouseMovement( 0, 0, false );

			//// Direct everything to the FBO		
			//::g_bIsSecondPass = true;
			//
			//GLint renderPassNumber_LocID = glGetUniformLocation( sexyShaderID, "renderPassNumber" );
			//glUniform1i( renderPassNumber_LocID, RENDER_PASS_0 );

			//glBindFramebuffer( GL_FRAMEBUFFER, g_myMirrorFBO.ID );

			//// Clear colour AND depth buffer
			//g_myMirrorFBO.clearBuffers();

			//RenderScene( ::g_vecGameObjects, ::g_pGLFWWindow, deltaTime );

			//// Revert the camera to correct position
			//::g_pTheMouseCamera->Position = tempPosition;
			//::g_pTheMouseCamera->Yaw -= 180.0f;
			//::g_pTheMouseCamera->Pitch = -::g_pTheMouseCamera->Pitch;
			//::g_pTheMouseCamera->ProcessMouseMovement( 0, 0, true );

			////// ##########################################

			// Direct everything to the FBO		
			::g_bIsSecondPass = false;

			GLint renderPassNumber_LocID = glGetUniformLocation( sexyShaderID, "renderPassNumber" );
			glUniform1i( renderPassNumber_LocID, RENDER_PASS_0 );

			glBindFramebuffer( GL_FRAMEBUFFER, g_myFBO.ID );

			// Clear colour AND depth buffer
			g_myFBO.clearBuffers();

			RenderScene( ::g_vecGameObjects, ::g_pGLFWWindow, deltaTime );

			// -----------> The Second Pass

			// Render it again, but point the the FBO texture... 
			::g_bIsSecondPass = true;
			glBindFramebuffer( GL_FRAMEBUFFER, 0 );

			GLfloat	zero = 0.0f;							 //clearBuffers();
			GLfloat one = 1.0f;								 //clearBuffers();
			glClearBufferfv( GL_COLOR, 0, &zero );			 //clearBuffers();
			glClearBufferfv( GL_DEPTH, 0, &one );			 //clearBuffers();

															 //glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

			::g_pShaderManager->useShaderProgram( "mySexyShader" );

			//glUniform1i( bIsSecondPassLocID, GL_TRUE );
			glUniform1i( renderPassNumber_LocID, RENDER_PASS_1 );

			GLint texFBOColour2DTextureUnitID = 10;
			GLint texFBOColour2DLocID = glGetUniformLocation( sexyShaderID, "texFBOColour2D" );
			GLint texFBONormal2DTextureUnitID = 11;
			GLint texFBONormal2DLocID = glGetUniformLocation( sexyShaderID, "texFBONormal2D" );
			GLint texFBOWorldPosition2DTextureUnitID = 12;
			GLint texFBOWorldPosition2DLocID = glGetUniformLocation( sexyShaderID, "texFBOVertexWorldPos2D" );

			//GLint texFBODepth2DTextureUnitID = 13;													   //ADDED
			//GLint texFBODepth2DLocID = glGetUniformLocation( sexyShaderID, "texFBODepth2D" );		   //ADDED


			// Pick a texture unit... 
			glActiveTexture( GL_TEXTURE0 + texFBOColour2DTextureUnitID );
			glBindTexture( GL_TEXTURE_2D, g_myFBO.colourTexture_0_ID );
			glUniform1i( texFBOColour2DLocID, texFBOColour2DTextureUnitID );

			glActiveTexture( GL_TEXTURE0 + texFBONormal2DTextureUnitID );
			glBindTexture( GL_TEXTURE_2D, g_myFBO.normalTexture_1_ID );
			glUniform1i( texFBONormal2DLocID, texFBONormal2DTextureUnitID );

			glActiveTexture( GL_TEXTURE0 + texFBOWorldPosition2DTextureUnitID );
			glBindTexture( GL_TEXTURE_2D, g_myFBO.vertexWorldPos_2_ID );
			glUniform1i( texFBOWorldPosition2DLocID, texFBOWorldPosition2DTextureUnitID );

			//glActiveTexture( GL_TEXTURE0 + texFBODepth2DTextureUnitID );				   //ADDED
			//glBindTexture( GL_TEXTURE_2D, g_myFBO.depthTexture_ID );					   //ADDED
			//glUniform1i( texFBODepth2DLocID, texFBODepth2DTextureUnitID );				   //ADDED

			// Set the sampler in the shader to the same texture unit (20)

			glfwGetFramebufferSize( ::g_pGLFWWindow, &width, &height );

			GLint screenWidthLocID = glGetUniformLocation( sexyShaderID, "screenWidth" );
			GLint screenHeightLocID = glGetUniformLocation( sexyShaderID, "screenHeight" );
			glUniform1f( screenWidthLocID, ( float )width );
			glUniform1f( screenHeightLocID, ( float )height );

			if( ::g_pThePlayerGO->friendlyName == "Rick" )
			{// This if for Rick's Drunk Effect

				::g_drunkEffect = 1;

				if( ::g_drunkOffset >= 0.01f )
					::g_drunkOffsetChange = -0.0001;
				else if( ::g_drunkOffset <= -0.01f )
					::g_drunkOffsetChange = 0.0001;

				::g_drunkOffset += ::g_drunkOffsetChange;
			}
			else
			{
				::g_drunkEffect = 0;
				::g_drunkOffset = 0.0f;
			}
			GLint DrunkOffset_LocID = glGetUniformLocation( sexyShaderID, "DrunkOffset" );
			glUniform1f( DrunkOffset_LocID, ::g_drunkOffset );

			GLint DrunkFlag_LocID = glGetUniformLocation( sexyShaderID, "DrunkEffect" );
			glUniform1i( DrunkFlag_LocID, ::g_drunkEffect );

			GLint OutlineFlag_LocID = glGetUniformLocation( sexyShaderID, "useOutline" );
			glUniform1i( OutlineFlag_LocID, ::g_outline );

			std::vector< cGameObject* >  vecCopy2ndPass;
			// Push back a SINGLE quad or GIANT triangle that fills the entire screen
			vecCopy2ndPass.push_back( ::g_pSkyBoxObject );
			RenderScene( vecCopy2ndPass, ::g_pGLFWWindow, deltaTime );

			// #################################################################
			//glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			//
			//::g_pShaderManager->useShaderProgram( "mySexyShader" );

			//glUniform1i( renderPassNumber_LocID, RENDER_PASS_2 );

			//GLint fullRenderedImage2D_LocID = glGetUniformLocation( sexyShaderID, "fullRenderedImage2D" );

			////glUniform1i( fullRenderedImage2D_LocID, g_renderID );
			//glUniform1i( fullRenderedImage2D_LocID, g_renderID );  //12 );
			//
			//std::vector< cGameObject* >  vecCopy2ndPass1;
			//// Draw just the terrain as a test
			////vecCopy2ndPass1.push_back( ::g_pSkyBoxObject );
			//vecCopy2ndPass1.push_back( ::g_pMirrorObject );
			//RenderScene( vecCopy2ndPass1, ::g_pGLFWWindow, deltaTime );
			// #################################################################
		}

		//move_player( deltaTime );

		//::g_pSteeringManager->updateAll( deltaTime );

		//// Check for collisions with the player and update it's health
		//collisionCheck();

		// Update camera
		ProcessCameraInput( ::g_pGLFWWindow, deltaTime );
		//// No need to update the camera if nothing has changed
		//if( ::g_pThePlayerGO->position != ::g_pThePlayerGO->prevPosition ||
		//	::g_pThePlayerGO->qOrientation != ::g_pThePlayerGO->prevOrientation )
		//{
		//	::g_pTheMouseCamera->moveCamera();
		//}
		//::g_pThePlayerGO->prevOrientation = ::g_pThePlayerGO->qOrientation;
		//::g_pThePlayerGO->prevPosition = ::g_pThePlayerGO->position;

		// *********************************************
		//    ___ _        ___              __     ___                          
		//   / __| |___  _| _ ) _____ __  __\ \   / __|__ _ _ __  ___ _ _ __ _  
		//   \__ \ / / || | _ \/ _ \ \ / |___> > | (__/ _` | '  \/ -_) '_/ _` | 
		//   |___/_\_\\_, |___/\___/_\_\    /_/   \___\__,_|_|_|_\___|_| \__,_| 
		//            |__/                                                      
		//cPhysicalProperties skyBoxPP;
		//::g_pSkyBoxObject->GetPhysState( skyBoxPP );
		//skyBoxPP.position = ::g_pTheCamera->getEyePosition();
		//::g_pSkyBoxObject->SetPhysState( skyBoxPP );
		::g_pSkyBoxObject->position = ::g_pTheMouseCamera->Position;


		std::stringstream ssTitle;
		ssTitle << "Animation: Project 1"
			<< "Player Health: " << ::g_pThePlayerGO->health;

		glfwSetWindowTitle( ::g_pGLFWWindow, ssTitle.str().c_str() );

		glfwSwapBuffers( ::g_pGLFWWindow );
		glfwPollEvents();


	}// while ( ! glfwWindowShouldClose(window) )


	glfwDestroyWindow( ::g_pGLFWWindow );
	glfwTerminate();

	// 
	delete ::g_pShaderManager;
	delete ::g_pVAOManager;

	//    exit(EXIT_SUCCESS);
	return 0;
}

//Load Config.txt
void loadConfigFile( std::string fileName, sWindowConfig& wConfig )
{
	// TODO change this config formating
	std::ifstream infoFile( fileName );
	if( !infoFile.is_open() )
	{	// File didn't open...
		std::cout << "Can't find config file" << std::endl;
		std::cout << "Using defaults" << std::endl;
	}
	else
	{	// File DID open, so read it... 
		std::string a;

		infoFile >> a;	// "Game"	//std::cin >> a;
		infoFile >> a;	// "Config"
		infoFile >> a;	// "width"
		infoFile >> wConfig.width;	// 1080
		infoFile >> a;	// "height"
		infoFile >> wConfig.height;	// 768
		infoFile >> a;		// Title_Start

		std::stringstream ssTitle;		// Inside "sstream"
		bool bKeepReading = true;
		do
		{
			infoFile >> a;		// Title_End??
			if( a != "Title_End" )
			{
				ssTitle << a << " ";
			}
			else
			{	// it IS the end! 
				bKeepReading = false;
				wConfig.title = ssTitle.str();
			}
		} while( bKeepReading );
	}
}

// Generate real random numbers
float generateRandomNumber( float min, float max )
{

	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();

	std::default_random_engine generator( seed );
	std::uniform_real_distribution<float> distribution( min, max );

	float randomNumber = 0.0;

	randomNumber = distribution( generator );
	return randomNumber;

}

bool CheckDistance( glm::vec3 position, glm::vec3 fromPoint, float minDistance, float maxDistance )
{
	float thisDistance = 0.0f;
	thisDistance = glm::distance( position, fromPoint );

	if( thisDistance >= minDistance || thisDistance <= maxDistance ) return true;

	return false;
}

glm::vec3 GetRandomPosition()
{
	glm::vec3 center = glm::vec3( 0.0f );
	glm::vec3 newPosition = glm::vec3( 0.0f );
	bool validPosition = false;

	while( !validPosition )
	{
		newPosition = glm::vec3( generateRandomNumber( -200, 200 ),
								 generateRandomNumber( -200, 200 ),
								 generateRandomNumber( -200, 200 ) );
		validPosition = CheckDistance( newPosition, center, 30.0f, 100.0f );
	}

	return newPosition;
}
//
////Load objects.txt
//void loadObjectsFile( std::string fileName )
//{
//	//sGOparameters sGOpar;
//	std::vector <sGOparameters> allObjects;
//
//	std::ifstream objectsFile( fileName );
//	if( !objectsFile.is_open() )
//	{	// File didn't open...
//		std::cout << "Can't find config file" << std::endl;
//		std::cout << "Using defaults" << std::endl;
//	}
//	else
//	{	// File DID open, so loop through the file and pushback to structure
//		while( !objectsFile.eof() && objectsFile.is_open() ) {
//			allObjects.push_back( parseObjLine( objectsFile ) );
//		}
//		objectsFile.close();  //Closing "costfile.txt"
//	}
//
//	for( int index = 0; index != allObjects.size(); index++ )
//	{
//		// Check, Number of Objects must be at least 1
//		if( allObjects[index].nObjects == 0 ) allObjects[index].nObjects = 1;
//
//		// Create the number of gameObjects specified in the file for each line 
//		for( int i = 0; i != allObjects[index].nObjects; i++ )
//		{
//			// Create a new GO
//			cGameObject* pTempGO = new cGameObject();
//
//			pTempGO->meshName = allObjects[index].meshname; // Set the name of the mesh
//
//			//pTempGO->diffuseColour = glm::vec4( 0.5f, 0.5f, 0.5f, 1.0f );
//			pTempGO->diffuseColour = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );
//			//pTempGO->rotation = glm::vec3( 0.0f );
//			pTempGO->overwrtiteQOrientationFromEuler( glm::vec3( 0.0f, 0.0f, 0.0f ) );
//
//
//			// SOME OBJECTS ARE RANDOMLY PLACED WHEN RANDOM=TRUE ON FILE
//			if( allObjects[index].random == "true" )
//			{
//				pTempGO->position.x = generateRandomNumber( -allObjects[index].rangeX, allObjects[index].rangeX );
//				pTempGO->position.y = generateRandomNumber( -allObjects[index].rangeY, allObjects[index].rangeY );
//				pTempGO->position.z = generateRandomNumber( -allObjects[index].rangeZ, allObjects[index].rangeZ );
//				pTempGO->scale = allObjects[index].rangeScale;
//			}
//			else
//			{   // position and scale are fixed
//				pTempGO->position.x = allObjects[index].x;
//				pTempGO->position.y = allObjects[index].y;
//				pTempGO->position.z = allObjects[index].z;
//				pTempGO->scale = allObjects[index].scale;
//			}
//			// NO VELOCITY
//			pTempGO->vel = glm::vec3( 0.0f );
//
//			if( pTempGO->meshName == "rick" )
//			{
//				//pTempGO->textureNames[0] = "rick_texture.bmp";
//				//pTempGO->textureBlend[0] = 1.0f;
//				pTempGO->type = eTypeOfGO::CHARACTER;
//				pTempGO->team = eTeam::PLAYER;
//				pTempGO->behaviour = eBehaviour::UNAVAIABLE;
//			}
//
//			else if( pTempGO->meshName == "scary" )
//			{
//				//pTempGO->textureNames[0] = "scary.bmp";
//				//pTempGO->textureBlend[0] = 1.0f;
//				pTempGO->type = eTypeOfGO::CHARACTER;
//				pTempGO->team = eTeam::ENEMY;
//				pTempGO->behaviour = eBehaviour::ANGRY;
//				pTempGO->range = 6.0f;
//				pTempGO->health = 100.0f;
//				pTempGO->maxVel = 1.5f;
//			}
//
//			else if( pTempGO->meshName == "morty" )
//			{
//				//pTempGO->textureNames[0] = "morty.bmp";
//				//pTempGO->textureBlend[0] = 1.0f;
//				pTempGO->type = eTypeOfGO::CHARACTER;
//				pTempGO->team = eTeam::ENEMY;
//				pTempGO->behaviour = eBehaviour::FOLLOWER;
//				pTempGO->range = 8.0f;
//				pTempGO->health = 100.0f;
//				pTempGO->maxVel = 1.0f;
//			}
//				
//			else if( pTempGO->meshName == "meeseeks" )
//			{
//				//pTempGO->textureNames[0] = "meeseeks.bmp";
//				//pTempGO->textureBlend[0] = 1.0f;
//				pTempGO->type = eTypeOfGO::CHARACTER;
//				pTempGO->team = eTeam::ENEMY;
//				pTempGO->behaviour = eBehaviour::CURIOUS;
//				pTempGO->range = 6.0f;
//				pTempGO->health = 100.0f;
//				pTempGO->maxVel = 1.5f;
//			}
//				
//			else if( pTempGO->meshName == "circle" )
//			{
//				//pTempGO->textureNames[0] = "white.bmp";
//				//pTempGO->textureBlend[0] = 1.0f;
//				pTempGO->type = eTypeOfGO::OTHER;
//				pTempGO->team = eTeam::NONE;
//				pTempGO->behaviour = eBehaviour::UNAVAIABLE;
//			}
//				
//			else if( pTempGO->meshName == "terrain" )
//			{
//				//pTempGO->textureNames[0] = "moon.bmp";
//				//pTempGO->textureBlend[0] = 1.0f;
//				pTempGO->type = eTypeOfGO::TERRAIN;
//				pTempGO->team = eTeam::NONE;
//				pTempGO->behaviour = eBehaviour::UNAVAIABLE;
//			}
//			
//
//			
//			
//			::g_vecGameObjects.push_back( pTempGO );
//		}
//	}
//}

//// Parse the file line to fit into the structure
//sGOparameters parseObjLine( std::ifstream &source ) {
//
//	sGOparameters sGOpar;
//
//	//Scanning a line from the file
//	source >> sGOpar.meshname >> sGOpar.nObjects
//		>> sGOpar.x >> sGOpar.y >> sGOpar.z >> sGOpar.scale
//		>> sGOpar.random
//		>> sGOpar.rangeX >> sGOpar.rangeY >> sGOpar.rangeZ
//		>> sGOpar.rangeScale;
//
//
//	return sGOpar;
//}

//Load meshlist.txt
//void loadMeshesFile( std::string fileName, GLint ShaderID )
//{
//std::vector <sMeshparameters> allMeshes;

//std::ifstream objectsFile( fileName );
//if( !objectsFile.is_open() )
//{	// File didn't open...
//	std::cout << "Can't find config file" << std::endl;
//	std::cout << "Using defaults" << std::endl;
//}
//else
//{	// File DID open, so loop through the file and pushback to structure
//	while( !objectsFile.eof() && objectsFile.is_open() ) {
//		allMeshes.push_back( parseMeshLine( objectsFile ) );
//	}
//	objectsFile.close();  //Closing "costfile.txt"
//}

//for( int index = 0; index != allMeshes.size(); index++ )
//{
//	cMesh testMesh;
//	testMesh.name = allMeshes[index].meshname;
//	//if( !LoadPlyFileIntoMesh( allMeshes[index].meshFilename, testMesh ) )
//	if( !LoadPlyFileIntoMeshWithUV( allMeshes[index].meshFilename, testMesh ) )
//	{
//		std::cout << "Didn't load model" << std::endl;
//		// do something??
//	}
//	if( testMesh.name == "ball" )
//	{
//		if( !::g_pVAOManager->loadMeshIntoVAO( testMesh, ShaderID, true ) )
//		{
//			std::cout << "Could not load mesh into VAO" << std::endl;
//		}
//	}
//	else
//	{
//		if( !::g_pVAOManager->loadMeshIntoVAO( testMesh, ShaderID ) )
//		{
//			std::cout << "Could not load mesh into VAO" << std::endl;
//		}
//	}		
//}
//}

//// Parse the file line to fit into the structure
//sMeshparameters parseMeshLine( std::ifstream &source ) {
//
//	sMeshparameters sMeshpar;
//
//	//Scanning a line from the file
//	source >> sMeshpar.meshname >> sMeshpar.meshFilename;
//
//	return sMeshpar;
//}

void loadLightObjects()
{
	for( int index = 0; index < g_NUMBER_OF_LIGHTS; index++ )
	{
		// Create a new GO
		cGameObject* pTempGO = new cGameObject();

		pTempGO->meshName = "sphere"; // Set the name of the mesh

									  // position is based on light position
		pTempGO->position = ::g_pLightManager->vecLights[index].position;

		if( index == 0 ) pTempGO->scale = 3.0f;
		else pTempGO->scale = 1.0f;

		// Each light is initially white
		pTempGO->diffuseColour = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );

		pTempGO->bIsLight = true;
		pTempGO->myLight = index;

		::g_vecGameObjects.push_back( pTempGO );
	}
}

void updateAllObjects( double deltaTime )
{
	for( int i = 0; i != ::g_theScheduler.commandGroups.size(); i++ )
	{
		if( !g_theScheduler.commandGroups[i]->isDone() )
		{
			// Run the Command groups in sequence
			// Each command group runs its commands in paralel
			g_theScheduler.commandGroups[i]->update( deltaTime );
			break;
		}
	}

	return;
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback( GLFWwindow* window, double xpos, double zpos )
{
	if( ::g_firstMouse )
	{
		::g_lastX = ( float )xpos;
		::g_lastZ = ( float )zpos;
		::g_firstMouse = false;
	}

	float xoffset = ( float )xpos - ::g_lastX;
	float zoffset = ::g_lastZ - ( float )zpos; // reversed since y-coordinates go from bottom to top

	::g_lastX = ( float )xpos;
	::g_lastZ = ( float )zpos;

	float rateOfTurn = xoffset * -0.01;

	//::g_pTheBall->adjustQOrientationFormDeltaEuler( glm::vec3( 0.0f, rateOfTurn, 0.0f ) );
	::g_pTheMouseCamera->ProcessMouseMovement( xoffset, zoffset );
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback( GLFWwindow* window, double xoffset, double zoffset )
{
	::g_pTheMouseCamera->ProcessMouseScroll( ( float )zoffset );
}

//void printCameraDetails()
//{
//	std::cout << "Camera Pos: "
//		<< ::g_pTheMouseCamera->Position.x << ", "
//		<< ::g_pTheMouseCamera->Position.y << ", "
//		<< ::g_pTheMouseCamera->Position.z
//		<< " | WorldUp: "
//		<< ::g_pTheMouseCamera->WorldUp.x << ", "
//		<< ::g_pTheMouseCamera->WorldUp.y << ", "
//		<< ::g_pTheMouseCamera->WorldUp.z
//		<< " | Yaw: " << ::g_pTheMouseCamera->Yaw
//		<< " | Pitch: " << ::g_pTheMouseCamera->Pitch
//		<< std::endl;
//}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void ProcessCameraInput( GLFWwindow *window, double deltaTime )
{
	if( glfwGetKey( window, GLFW_KEY_ESCAPE ) == GLFW_PRESS )
		glfwSetWindowShouldClose( window, true );

	if( glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS )
		::g_pTheMouseCamera->ProcessKeyboard( FORWARD, ( float )deltaTime );
	if( glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS )
		::g_pTheMouseCamera->ProcessKeyboard( BACKWARD, ( float )deltaTime );
	if( glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS )
		::g_pTheMouseCamera->ProcessKeyboard( LEFT, ( float )deltaTime );
	if( glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS )
		::g_pTheMouseCamera->ProcessKeyboard( RIGHT, ( float )deltaTime );
	if( glfwGetKey( window, GLFW_KEY_PAGE_UP ) == GLFW_PRESS )
		::g_pTheMouseCamera->ProcessKeyboard( UP, ( float )deltaTime );
	if( glfwGetKey( window, GLFW_KEY_PAGE_DOWN ) == GLFW_PRESS )
		::g_pTheMouseCamera->ProcessKeyboard( DOWN, ( float )deltaTime );
}

void setWindowFullScreenOrWindowed( GLFWwindow* pTheWindow, bool IsFullScreen )
{
	// Set full-screen or windowed
	if( ::g_IsWindowFullScreen )
	{
		// Find the size of the primary monitor
		GLFWmonitor* pPrimaryScreen = glfwGetPrimaryMonitor();
		const GLFWvidmode* pPrimMonVidMode = glfwGetVideoMode( pPrimaryScreen );
		// Set this window to full screen, matching the size of the monitor:
		glfwSetWindowMonitor( pTheWindow, pPrimaryScreen,
							  0, 0,				// left, top corner 
							  pPrimMonVidMode->width, pPrimMonVidMode->height,
							  GLFW_DONT_CARE );	// refresh rate

		std::cout << "Window now fullscreen at ( "
			<< pPrimMonVidMode->width << " x "
			<< pPrimMonVidMode->height << " )" << std::endl;
	}
	else
	{
		// Make the screen windowed. (i.e. It's CURRENTLY full-screen)
		// NOTE: We aren't saving the "old" windowed size - you might want to do that...
		// HACK: Instead, we are taking the old size and mutiplying it by 75% 
		// (the thinking is: the full-screen switch always assumes we want the maximum
		//	resolution - see code above - but when we make that maximum size windowed,
		//  it's going to be 'too big' for the screen)
		GLFWmonitor* pPrimaryScreen = glfwGetPrimaryMonitor();
		const GLFWvidmode* pPrimMonVidMode = glfwGetVideoMode( pPrimaryScreen );

		// Put the top, left corner 10% of the size of the full-screen
		int topCornerTop = ( int )( ( float )pPrimMonVidMode->height * 0.1f );
		int topCornerLeft = ( int )( ( float )pPrimMonVidMode->width * 0.1f );
		// Make the width and height 75% of the full-screen resolution
		int height = ( int )( ( float )pPrimMonVidMode->height * 0.75f );
		int width = ( int )( ( float )pPrimMonVidMode->width * 0.75f );

		glfwSetWindowMonitor( pTheWindow, NULL,		// This NULL makes the screen windowed
							  topCornerLeft, topCornerTop,				// left, top corner 
							  width, height,
							  GLFW_DONT_CARE );	// refresh rate

		std::cout << "Window now windowed at ( "
			<< width << " x " << height << " )"
			<< " and offset to ( "
			<< topCornerLeft << ", " << topCornerTop << " )"
			<< std::endl;
	}
	return;
}