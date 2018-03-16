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

int g_NUMBER_OF_LIGHTS = 4;

std::vector< cGameObject* > g_vecGameObjects;

cGameObject* g_pThePlayerGO = NULL;

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

bool g_bUseDeferred = false; // Switch between 1 pass or 2 passes...

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
sGOparameters parseObjLine( std::ifstream &source );
void loadObjectsFile( std::string fileName );
sMeshparameters parseMeshLine( std::ifstream &source );
//void loadMeshesFile( std::string fileName, GLint ShaderID );
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


#include <math.h>

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

void setState( eAnimationType nextAnimation )
{
	bool isFinished = ::g_pThePlayerGO->pAniState->currentAnimation.isFinished;

	if( nextAnimation != ::g_pThePlayerGO->currentState )
	{
		switch( ::g_pThePlayerGO->currentState )
		{
			case BASE:	// IDLE
				::g_pThePlayerGO->currentState = nextAnimation;
				break;
			case JUMP:
				if( isFinished )
					::g_pThePlayerGO->currentState = nextAnimation;
				// TODO: ALLOW ROTATION
				break;
			case STRAFE_LEFT:
				::g_pThePlayerGO->currentState = nextAnimation;
				break;
			case STRAFE_RIGHT:
				::g_pThePlayerGO->currentState = nextAnimation;
				break;
			case MOVE_LEFT:
				::g_pThePlayerGO->currentState = nextAnimation;
				break;
			case MOVE_RIGHT:
				::g_pThePlayerGO->currentState = nextAnimation;
				break;
			case RUN:
				//if( nextAnimation == WALK_FORWARD ||
				//	nextAnimation == JUMP ||
				//	nextAnimation == NOTHING )
					::g_pThePlayerGO->currentState = nextAnimation;
				break;
			case WALK_FORWARD:
				::g_pThePlayerGO->currentState = nextAnimation;
				break;
			case WALK_BACKWARD:
				//if( nextAnimation != JUMP )	// CAN'T JUMP BACK!
					::g_pThePlayerGO->currentState = nextAnimation;
				break;
			case ACTION:
				::g_pThePlayerGO->currentState = nextAnimation;
				break;
			case NOTHING:
				::g_pThePlayerGO->currentState = nextAnimation;
				break;
			default:
				//::g_pThePlayerGO->currentState = nextAnimation;
				break;
		}

	}
	return;
}

void move_player()
{	
	switch( ::g_pThePlayerGO->currentState )
	{
		case BASE:
			std::cout << "Base ";
			updateCurrentAnimation( ::g_pThePlayerGO->currentState );
			break;
		case JUMP:
			std::cout << "Jump ";
			updateCurrentAnimation( ::g_pThePlayerGO->currentState, false );
			break;
		case STRAFE_LEFT:
			std::cout << "StrafeL ";
			updateCurrentAnimation( ::g_pThePlayerGO->currentState );
			break;
		case STRAFE_RIGHT:
			std::cout << "StrafeR ";
			updateCurrentAnimation( ::g_pThePlayerGO->currentState );
			break;
		case MOVE_LEFT:
			std::cout << "MoveL ";
			updateCurrentAnimation( ::g_pThePlayerGO->currentState );
			break;
		case MOVE_RIGHT:
			std::cout << "MoveR ";
			updateCurrentAnimation( ::g_pThePlayerGO->currentState );
			break;
		case RUN:
			std::cout << "Run ";
			updateCurrentAnimation( ::g_pThePlayerGO->currentState );
			break;
		case WALK_FORWARD:
			std::cout << "WalkF ";
			updateCurrentAnimation( ::g_pThePlayerGO->currentState );
			break;
		case WALK_BACKWARD:
			std::cout << "WalkB ";
			updateCurrentAnimation( ::g_pThePlayerGO->currentState );
			break;
		case ACTION:
			std::cout << "Action ";
			updateCurrentAnimation( ::g_pThePlayerGO->currentState, false );
			break;
		case NOTHING:
			std::cout << "None ";
			updateCurrentAnimation( ::g_pThePlayerGO->currentState );
			break;
		default:
			break;
	}


	float totalVelocity = 0.0f;
	glm::vec3 movement = glm::vec3( 0.0f );

	if( isnan( ::g_pThePlayerGO->vel.x ) ) ::g_pThePlayerGO->vel.x = 0.0f;
	if( isnan( ::g_pThePlayerGO->vel.y ) ) ::g_pThePlayerGO->vel.y = 0.0f;
	if( isnan( ::g_pThePlayerGO->vel.z ) ) ::g_pThePlayerGO->vel.z = 0.0f;
	
	if( ::g_pThePlayerGO->currentState == eAnimationType::MOVE_RIGHT ||
		::g_pThePlayerGO->currentState == eAnimationType::STRAFE_RIGHT )
	{
		float maxSpeed = ::g_pThePlayerGO->mySpeed.right * ::g_pThePlayerGO->scale;

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
	if( ::g_pThePlayerGO->currentState == eAnimationType::MOVE_LEFT ||
		::g_pThePlayerGO->currentState == eAnimationType::STRAFE_LEFT )
	{
		float maxSpeed = ::g_pThePlayerGO->mySpeed.right * ::g_pThePlayerGO->scale;

		movement = ::g_pThePlayerGO->getRightVector() * 0.01f;

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
	else if( ::g_pThePlayerGO->currentState == WALK_FORWARD )
	{	
		float maxSpeed = ::g_pThePlayerGO->mySpeed.forward * ::g_pThePlayerGO->scale;

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
	else if( ::g_pThePlayerGO->currentState == WALK_BACKWARD )
	{
		float maxSpeed = ::g_pThePlayerGO->mySpeed.backward * ::g_pThePlayerGO->scale;

		movement = glm::normalize( glm::vec3( ::g_pTheMouseCamera->Front.x, 0.0f, ::g_pTheMouseCamera->Front.z ) );
		movement *= 0.01f;
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

	//if( ::g_pThePlayerGO->currentState == TURN_RIGHT )
	//	::g_pThePlayerGO->adjustQOrientationFormDeltaEuler( glm::vec3( 0.0f, 0.05f, 0.0f ) );
	//if( ::g_pThePlayerGO->currentState == TURN_LEFT )
	//	::g_pThePlayerGO->adjustQOrientationFormDeltaEuler( glm::vec3( 0.0f, -0.05f, 0.0f ) );

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

void drawRange()
{
	glm::vec3 color = glm::vec3( 1.0f );

	glm::vec3 point1 = glm::vec3( maxX, 0.0f, maxZ );
	glm::vec3 point2 = glm::vec3( maxX, 0.0f, minZ );
	glm::vec3 point3 = glm::vec3( minX, 0.0f, minZ );
	glm::vec3 point4 = glm::vec3( minX, 0.0f, maxZ );
	
	glm::vec3 point5 = glm::vec3( maxX, 1.0f, maxZ );
	glm::vec3 point6 = glm::vec3( maxX, 1.0f, minZ );
	glm::vec3 point7 = glm::vec3( minX, 1.0f, minZ );
	glm::vec3 point8 = glm::vec3( minX, 1.0f, maxZ );

	::g_pDebugRenderer->addLine( point1, point2, color, true );
	::g_pDebugRenderer->addLine( point2, point3, color, true );
	::g_pDebugRenderer->addLine( point3, point4, color, true );
	::g_pDebugRenderer->addLine( point4, point1, color, true );
														
	::g_pDebugRenderer->addLine( point5, point6, color, true );
	::g_pDebugRenderer->addLine( point6, point7, color, true );
	::g_pDebugRenderer->addLine( point7, point8, color, true );
	::g_pDebugRenderer->addLine( point8, point5, color, true );
														
	::g_pDebugRenderer->addLine( point1, point5, color, true );
	::g_pDebugRenderer->addLine( point2, point6, color, true );
	::g_pDebugRenderer->addLine( point3, point7, color, true );
	::g_pDebugRenderer->addLine( point4, point8, color, true );
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

	::g_pLightManager->CreateLights( g_NUMBER_OF_LIGHTS );	// There are 10 lights in the shader

	// Change Other lights parameters==========================
	{
		::g_pLightManager->vecLights[0].position = glm::vec3( 50.0f, 50.0f, 50.0f );
		::g_pLightManager->vecLights[0].attenuation.y = 0.03f;	// Linear
		//::g_pLightManager->vecLights[0].attenuation.y = 0.6f;		// Change the linear attenuation
		//::g_pLightManager->vecLights[0].attenuation.z = 0.005f;

		::g_pLightManager->vecLights[1].position = glm::vec3( 50.0f, -50.0f, 50.0f );
		//::g_pLightManager->vecLights[1].attenuation.y = 0.02f;	// Linear

		::g_pLightManager->vecLights[2].position = glm::vec3( -50.0f, -50.0f, 50.0f );
		::g_pLightManager->vecLights[1].attenuation.y = 0.01f;	// Linear

		::g_pLightManager->vecLights[3].position = glm::vec3( -50.0f, 50.0f, 50.0f );
		//::g_pLightManager->vecLights[1].attenuation.y = 0.0f;	// Linear
		
	}
	//=========================================================
	::g_pLightManager->LoadShaderUniformLocations( currentProgID );

	//loadLightObjects();

	// Texture 
	::g_pTextureManager = new CTextureManager();

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
	::g_pTextureManager->Create2DTextureFromBMPFile( "scary.bmp", true );
	::g_pTextureManager->Create2DTextureFromBMPFile( "meeseeks.bmp", true );		
	::g_pTextureManager->Create2DTextureFromBMPFile( "moon.bmp", true );
	//::g_pTextureManager->Create2DTextureFromBMPFile( "red.bmp", true );	
	//::g_pTextureManager->Create2DTextureFromBMPFile( "blue.bmp", true );
	//::g_pTextureManager->Create2DTextureFromBMPFile( "white.bmp", true );
	//::g_pTextureManager->Create2DTextureFromBMPFile( "orange.bmp", true );
	//::g_pTextureManager->Create2DTextureFromBMPFile( "purple.bmp", true );
	//::g_pTextureManager->Create2DTextureFromBMPFile( "gray.bmp", true );
	//::g_pTextureManager->Create2DTextureFromBMPFile( "green.bmp", true );
	//::g_pTextureManager->Create2DTextureFromBMPFile( "yellow.bmp", true );
	
	//::g_pTextureManager->SetBasePath( "assets/textures/skybox/" );
	//if( !::g_pTextureManager->CreateNewCubeTextureFromBMPFiles(
	//	"space",
	//	"SpaceBox_right1_posX.bmp",
	//	"SpaceBox_left2_negX.bmp",
	//	"SpaceBox_top3_posY.bmp",
	//	"SpaceBox_bottom4_negY.bmp",
	//	"SpaceBox_front5_posZ.bmp",
	//	"SpaceBox_back6_negZ.bmp" ) )
	//{
	//	std::cout << "Didn't load skybox" << std::endl;
	//}
	//else
	//{
	//	std::cout << "Skybox Texture is loaded! Hazzah!" << std::endl;
	//}

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

	//::g_pTheCamera = new cCamera();
	//::g_pTheCamera->setCameraMode( cCamera::FLY_CAMERA );	
	//::g_pTheCamera->eye = glm::vec3( -5.0f, -5.0f, -25.0f );

	glm::vec3 camPos = ::g_pThePlayerGO->position + glm::vec3( 0.0f, 6.0f, 12.f );
	glm::vec3 camUp = glm::vec3( 0.0f, 1.0f, 0.0f );
	::g_pTheMouseCamera = new cMouseCamera( camPos, camUp ); // camYaw, camPitch );
	// Follow the player
	::g_pTheMouseCamera->setTarget( ::g_pThePlayerGO );
	::g_pTheMouseCamera->moveCamera();	

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

	setWindowFullScreenOrWindowed( ::g_pGLFWWindow, ::g_IsWindowFullScreen );


	// Gets the "current" time "tick" or "step"
	double lastTimeStep = glfwGetTime();

	drawRange();

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
			GLint bIsSecondPassLocID = glGetUniformLocation( sexyShaderID, "bIsSecondPass" );
			glUniform1i( bIsSecondPassLocID, GL_FALSE );
			glBindFramebuffer( GL_FRAMEBUFFER, 0 );
			// Clear colour AND depth buffer
			g_myFBO.clearBuffers();

			RenderScene( ::g_vecGameObjects, ::g_pGLFWWindow, deltaTime );
		}
		else
		{	// Direct everything to the FBO
		
			GLint bIsSecondPassLocID = glGetUniformLocation( sexyShaderID, "bIsSecondPass" );
			glUniform1i( bIsSecondPassLocID, GL_FALSE );
			glBindFramebuffer( GL_FRAMEBUFFER, g_myFBO.ID );
			
			// Clear colour AND depth buffer
			g_myFBO.clearBuffers();

			RenderScene( ::g_vecGameObjects, ::g_pGLFWWindow, deltaTime );

			// Render it again, but point the the FBO texture... 
			glBindFramebuffer( GL_FRAMEBUFFER, 0 );

			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

			::g_pShaderManager->useShaderProgram( "mySexyShader" );

			glUniform1i( bIsSecondPassLocID, GL_TRUE );

			GLint texFBOColour2DTextureUnitID = 10;
			GLint texFBOColour2DLocID = glGetUniformLocation( sexyShaderID, "texFBOColour2D" );
			GLint texFBONormal2DTextureUnitID = 11;
			GLint texFBONormal2DLocID = glGetUniformLocation( sexyShaderID, "texFBONormal2D" );
			GLint texFBOWorldPosition2DTextureUnitID = 12;
			GLint texFBOWorldPosition2DLocID = glGetUniformLocation( sexyShaderID, "texFBOVertexWorldPos2D" );

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

			// Set the sampler in the shader to the same texture unit (20)

			glfwGetFramebufferSize( ::g_pGLFWWindow, &width, &height );

			GLint screenWidthLocID = glGetUniformLocation( sexyShaderID, "screenWidth" );
			GLint screenHeightLocID = glGetUniformLocation( sexyShaderID, "screenHeight" );
			glUniform1f( screenWidthLocID, ( float )width );
			glUniform1f( screenHeightLocID, ( float )height );

			std::vector< cGameObject* >  vecCopy2ndPass;
			// Push back a SINGLE quad or GIANT triangle that fills the entire screen
			vecCopy2ndPass.push_back( ::g_vecGameObjects[0] );
			RenderScene( vecCopy2ndPass, ::g_pGLFWWindow, deltaTime );
		}

		//move_player( deltaTime );

		//::g_pSteeringManager->updateAll( deltaTime );

		//// Check for collisions with the player and update it's health
		//collisionCheck();

		// Update camera
		//ProcessCameraInput( window, deltaTime );
		// No need to update the camera if nothing has changed
		if( ::g_pThePlayerGO->position != ::g_pThePlayerGO->prevPosition ||
			::g_pThePlayerGO->qOrientation != ::g_pThePlayerGO->prevOrientation )
		{
			::g_pTheMouseCamera->moveCamera();
		}
		::g_pThePlayerGO->prevOrientation = ::g_pThePlayerGO->qOrientation;
		::g_pThePlayerGO->prevPosition = ::g_pThePlayerGO->position;


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

//void newPlayerGO()
//{
//	// Create a new GO
//	cGameObject* pTempGO3 = new cGameObject();
//
//	cMesh theBallMesh;
//	::g_pVAOManager->lookupMeshFromName( "rick", theBallMesh );
//	
//	pTempGO3->meshName = "rick";
//	pTempGO3->diffuseColour = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );
//	
//	pTempGO3->position = glm::vec3( 0.0f, 0.0f, 0.0f );
//	pTempGO3->prevPosition = pTempGO3->position;
//	pTempGO3->scale = 1.0f;
//	pTempGO3->overwrtiteQOrientationFromEuler( glm::vec3( 0.0f, 0.0f, 0.0f ) );
//	pTempGO3->vel = glm::vec3( 0.0f );
//	pTempGO3->friendlyName = "player";
//	pTempGO3->typeOfObject = SPHERE;
//	pTempGO3->bIsUpdatedInPhysics = true;
//	pTempGO3->mass = 0.1;
//	pTempGO3->inverseMass = 1.0f / pTempGO3->mass;
//
//	pTempGO3->radius = theBallMesh.maxExtent / 2;
//
//	//pTempGO3->textureBlend[0] = 1.0f;
//	//pTempGO3->textureNames[0] = "rick_texture.bmp";
//
//	pTempGO3->type = eTypeOfGO::CHARACTER;
//	pTempGO3->team = eTeam::PLAYER;
//	pTempGO3->enemyType = eEnemyType::UNAVAIABLE;
//	pTempGO3->range = 4.0f;
//	pTempGO3->maxVel = 0.05f;
//	pTempGO3->health = 100.0f;
//
//	::g_vecGameObjects.push_back( pTempGO3 );
//	::g_GameObjNumber = ::g_vecGameObjects.size() - 1;
//
//}

//Load objects.txt
void loadObjectsFile( std::string fileName )
{
	//sGOparameters sGOpar;
	std::vector <sGOparameters> allObjects;

	std::ifstream objectsFile( fileName );
	if( !objectsFile.is_open() )
	{	// File didn't open...
		std::cout << "Can't find config file" << std::endl;
		std::cout << "Using defaults" << std::endl;
	}
	else
	{	// File DID open, so loop through the file and pushback to structure
		while( !objectsFile.eof() && objectsFile.is_open() ) {
			allObjects.push_back( parseObjLine( objectsFile ) );
		}
		objectsFile.close();  //Closing "costfile.txt"
	}

	for( int index = 0; index != allObjects.size(); index++ )
	{
		// Check, Number of Objects must be at least 1
		if( allObjects[index].nObjects == 0 ) allObjects[index].nObjects = 1;

		// Create the number of gameObjects specified in the file for each line 
		for( int i = 0; i != allObjects[index].nObjects; i++ )
		{
			// Create a new GO
			cGameObject* pTempGO = new cGameObject();

			pTempGO->meshName = allObjects[index].meshname; // Set the name of the mesh

			//pTempGO->diffuseColour = glm::vec4( 0.5f, 0.5f, 0.5f, 1.0f );
			pTempGO->diffuseColour = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );
			//pTempGO->rotation = glm::vec3( 0.0f );
			pTempGO->overwrtiteQOrientationFromEuler( glm::vec3( 0.0f, 0.0f, 0.0f ) );


			// SOME OBJECTS ARE RANDOMLY PLACED WHEN RANDOM=TRUE ON FILE
			if( allObjects[index].random == "true" )
			{
				pTempGO->position.x = generateRandomNumber( -allObjects[index].rangeX, allObjects[index].rangeX );
				pTempGO->position.y = generateRandomNumber( -allObjects[index].rangeY, allObjects[index].rangeY );
				pTempGO->position.z = generateRandomNumber( -allObjects[index].rangeZ, allObjects[index].rangeZ );
				pTempGO->scale = allObjects[index].rangeScale;
			}
			else
			{   // position and scale are fixed
				pTempGO->position.x = allObjects[index].x;
				pTempGO->position.y = allObjects[index].y;
				pTempGO->position.z = allObjects[index].z;
				pTempGO->scale = allObjects[index].scale;
			}
			// NO VELOCITY
			pTempGO->vel = glm::vec3( 0.0f );

			if( pTempGO->meshName == "rick" )
			{
				//pTempGO->textureNames[0] = "rick_texture.bmp";
				//pTempGO->textureBlend[0] = 1.0f;
				pTempGO->type = eTypeOfGO::CHARACTER;
				pTempGO->team = eTeam::PLAYER;
				pTempGO->behaviour = eBehaviour::UNAVAIABLE;
			}

			else if( pTempGO->meshName == "scary" )
			{
				//pTempGO->textureNames[0] = "scary.bmp";
				//pTempGO->textureBlend[0] = 1.0f;
				pTempGO->type = eTypeOfGO::CHARACTER;
				pTempGO->team = eTeam::ENEMY;
				pTempGO->behaviour = eBehaviour::ANGRY;
				pTempGO->range = 6.0f;
				pTempGO->health = 100.0f;
				pTempGO->maxVel = 1.5f;
			}

			else if( pTempGO->meshName == "morty" )
			{
				//pTempGO->textureNames[0] = "morty.bmp";
				//pTempGO->textureBlend[0] = 1.0f;
				pTempGO->type = eTypeOfGO::CHARACTER;
				pTempGO->team = eTeam::ENEMY;
				pTempGO->behaviour = eBehaviour::FOLLOWER;
				pTempGO->range = 8.0f;
				pTempGO->health = 100.0f;
				pTempGO->maxVel = 1.0f;
			}
				
			else if( pTempGO->meshName == "meeseeks" )
			{
				//pTempGO->textureNames[0] = "meeseeks.bmp";
				//pTempGO->textureBlend[0] = 1.0f;
				pTempGO->type = eTypeOfGO::CHARACTER;
				pTempGO->team = eTeam::ENEMY;
				pTempGO->behaviour = eBehaviour::CURIOUS;
				pTempGO->range = 6.0f;
				pTempGO->health = 100.0f;
				pTempGO->maxVel = 1.5f;
			}
				
			else if( pTempGO->meshName == "circle" )
			{
				//pTempGO->textureNames[0] = "white.bmp";
				//pTempGO->textureBlend[0] = 1.0f;
				pTempGO->type = eTypeOfGO::OTHER;
				pTempGO->team = eTeam::NONE;
				pTempGO->behaviour = eBehaviour::UNAVAIABLE;
			}
				
			else if( pTempGO->meshName == "terrain" )
			{
				//pTempGO->textureNames[0] = "moon.bmp";
				//pTempGO->textureBlend[0] = 1.0f;
				pTempGO->type = eTypeOfGO::TERRAIN;
				pTempGO->team = eTeam::NONE;
				pTempGO->behaviour = eBehaviour::UNAVAIABLE;
			}
			

			
			
			::g_vecGameObjects.push_back( pTempGO );
		}
	}
}

// Parse the file line to fit into the structure
sGOparameters parseObjLine( std::ifstream &source ) {

	sGOparameters sGOpar;

	//Scanning a line from the file
	source >> sGOpar.meshname >> sGOpar.nObjects
		>> sGOpar.x >> sGOpar.y >> sGOpar.z >> sGOpar.scale
		>> sGOpar.random
		>> sGOpar.rangeX >> sGOpar.rangeY >> sGOpar.rangeZ
		>> sGOpar.rangeScale;


	return sGOpar;
}

//Load meshlist.txt
void loadMeshesFile( std::string fileName, GLint ShaderID )
{
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
}

// Parse the file line to fit into the structure
sMeshparameters parseMeshLine( std::ifstream &source ) {

	sMeshparameters sMeshpar;

	//Scanning a line from the file
	source >> sMeshpar.meshname >> sMeshpar.meshFilename;

	return sMeshpar;
}

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
	//::g_pTheMouseCamera->ProcessMouseMovement( xoffset, zoffset );	
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

	if( glfwGetKey( window, GLFW_KEY_W ) == GLFW_PRESS )
		::g_pTheMouseCamera->ProcessKeyboard( FORWARD, ( float )deltaTime );
	if( glfwGetKey( window, GLFW_KEY_S ) == GLFW_PRESS )
		::g_pTheMouseCamera->ProcessKeyboard( BACKWARD, ( float )deltaTime );
	if( glfwGetKey( window, GLFW_KEY_A ) == GLFW_PRESS )
		::g_pTheMouseCamera->ProcessKeyboard( LEFT, ( float )deltaTime );
	if( glfwGetKey( window, GLFW_KEY_D ) == GLFW_PRESS )
		::g_pTheMouseCamera->ProcessKeyboard( RIGHT, ( float )deltaTime );
	if( glfwGetKey( window, GLFW_KEY_Q ) == GLFW_PRESS )
		::g_pTheMouseCamera->ProcessKeyboard( UP, ( float )deltaTime );
	if( glfwGetKey( window, GLFW_KEY_E ) == GLFW_PRESS )
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