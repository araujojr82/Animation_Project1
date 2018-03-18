// This file is used to laod the models
#include "cGameObject.h"
#include <vector>
#include "Utilities.h"		// getRandInRange()
#include <glm/glm.hpp>
#include "globalGameStuff.h"

// For the cSimpleAssimpSkinnedMeshLoader class
#include "assimp/cSimpleAssimpSkinnedMeshLoader_OneMesh.h"
#include "assimp/cAssimpBasic.h"

#include "cAnimationState.h"

#include <iostream>

extern std::vector< cGameObject* >  g_vecGameObjects;
extern cGameObject* g_pTheDebugSphere;
extern cGameObject* g_pSkyBoxObject;
extern cGameObject* g_pMirrorObject;

cSimpleAssimpSkinnedMesh* createSkinnedMesh( std::string meshFilename )
{
	cSimpleAssimpSkinnedMesh* theSkinnedMesh = new cSimpleAssimpSkinnedMesh();

	// Load the animation AND the mesh to be used for this model
	if( !theSkinnedMesh->LoadMeshFromFile( meshFilename ) )
	{
		std::cout << "Error: problem loading the skinned mesh" << std::endl;
	}

	return theSkinnedMesh;
}

cSimpleAssimpSkinnedMesh* createSkinnedMesh( std::vector<sAnimDesc> theAnimations )
{
	cSimpleAssimpSkinnedMesh* theSkinnedMesh = new cSimpleAssimpSkinnedMesh();

	for( int i = 0; i != theAnimations.size(); i++ )
	{
		switch( theAnimations[i].type )
		{			
			case eAnimationType::BASE :				
				// Load the animation AND the mesh to be used for this model
				if( !theSkinnedMesh->LoadMeshFromFile( theAnimations[i].filePath ) )
				{
					std::cout << "Error: problem loading the skinned mesh" << std::endl;
				}
				break;
			default:
				// Load just the animation
				if( !theSkinnedMesh->LoadMeshAnimation( theAnimations[i].filePath ) )
				{
					std::cout << "Error: problem loading the skinned mesh" << std::endl;
				}
				break;
		}		
	}

	return theSkinnedMesh;
}

bool createMeshFromSkinnedMesh( cSimpleAssimpSkinnedMesh* theSkinnedMesh, int shaderID, cVAOMeshManager* pVAOManager )
{
	cMesh* pTheMesh = theSkinnedMesh->CreateMeshObjectFromCurrentModel();

	if( pTheMesh )
	{
		if( !pVAOManager->loadMeshIntoVAO( *pTheMesh, shaderID, false ) )
		{
			std::cout << "Could not load skinned mesh model into new VAO" << std::endl;
			return false;
		}
	}
	else
	{
		std::cout << "Could not create a cMesh object from skinned mesh file" << std::endl;
		return false;
	}
	// Delete temporary mesh if still around
	if( pTheMesh )
	{
		delete pTheMesh;
	}
	return true;
}

void LoadModelsIntoScene( int shaderID, cVAOMeshManager* pVAOManager )
{
	cAssimpBasic myAssimpLoader;
	std::string error;

	{ // Mirror
		::g_pMirrorObject = new cGameObject();
		::g_pMirrorObject->friendlyName = "Mirror";
		::g_pMirrorObject->scale = 3.0f;
		::g_pMirrorObject->position = glm::vec3( 0.0f, 0.0, 6.0f );

		cMesh theMesh;

		if( !myAssimpLoader.loadModelA( "assets/models/mirror.ply", theMesh, error ) )
		{
			std::cout << "All is lost! Forever lost!! Assimp didn't load the Model" << error << std::endl;
		}
		else
		{
			theMesh.name = "mirror";
			if( !pVAOManager->loadMeshIntoVAO( theMesh, shaderID, false ) )
			{
				std::cout << "Assimp loaded mesh didn't load into VAO" << std::endl;
			}

			sMeshDrawInfo meshInfo;
			meshInfo.scale = ::g_pMirrorObject->scale;
			meshInfo.setMeshOrientationEulerAngles( glm::vec3( 0.0f, 0.0f, 0.0f ) );
			meshInfo.debugDiffuseColour = glm::vec4( 1.0f, 1.0f, 0.0f, 1.0f );
			meshInfo.name = theMesh.name;
			meshInfo.vecMehs2DTextures.push_back( sTextureBindBlendInfo( "purple.bmp", 1.0f ) );
			::g_pMirrorObject->vecMeshes.push_back( meshInfo );
			//::g_vecGameObjects.push_back( ::g_pMirrorObject );		// Fastest way to add
		}
	}

	{	// Our skybox object
		::g_pSkyBoxObject = new cGameObject();
		::g_pSkyBoxObject->type = eTypeOfGO::SKYBOX;

		cMesh theMesh;

		if( !myAssimpLoader.loadModelA( "assets/models/skybox_sphere.ply", theMesh, error ) )
		{
			std::cout << "All is lost! Forever lost!! Assimp didn't load the Model" << error << std::endl;
		}
		else
		{
			theMesh.name = "skybox_sphere";
			if( !pVAOManager->loadMeshIntoVAO( theMesh, shaderID, false ) )
			{
				std::cout << "Assimp loaded mesh didn't load into VAO" << std::endl;
			}
			sMeshDrawInfo meshInfo;
			meshInfo.scale = 10000.0f;
			meshInfo.debugDiffuseColour = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );
			meshInfo.name = theMesh.name;
			meshInfo.vecMeshCubeMaps.push_back( sTextureBindBlendInfo( "space", 1.0f ) );
			meshInfo.bIsSkyBoxObject = true;
			::g_pSkyBoxObject->vecMeshes.push_back( meshInfo );
			// IS SKYBOX
			::g_vecGameObjects.push_back( ::g_pSkyBoxObject );		// Fastest way to add			
		}
	}

	{ // Terrain
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "Terrain";
		pTempGO->type = eTypeOfGO::TERRAIN;
		pTempGO->scale = 1.0f;

		cMesh theMesh;

		if (!myAssimpLoader.loadModelA("assets/models/ground_terrain.ply", theMesh, error))
		{
			std::cout << "All is lost! Forever lost!! Assimp didn't load the Model" << error << std::endl;
		}
		else
		{
			theMesh.name = "terrain";
			if( !pVAOManager->loadMeshIntoVAO( theMesh, shaderID, false ) )
			{
				std::cout << "Assimp loaded mesh didn't load into VAO" << std::endl;
			}

			pTempGO->position = glm::vec3( 0.0f, 0.0, 0.0f );
			sMeshDrawInfo meshInfo;
			meshInfo.scale = pTempGO->scale;
			meshInfo.setMeshOrientationEulerAngles( glm::vec3( 0.0f, 0.0f, 0.0f ) );
			meshInfo.debugDiffuseColour = glm::vec4( 1.0f, 1.0f, 0.0f, 1.0f );
			meshInfo.name = theMesh.name;
			meshInfo.vecMehs2DTextures.push_back( sTextureBindBlendInfo( "grass1.bmp", 1.0f ) );
			pTempGO->vecMeshes.push_back( meshInfo );
			::g_vecGameObjects.push_back( pTempGO );		// Fastest way to add
		}
	}

	{	// Skinned mesh  model
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "Morty";

		pTempGO->type = eTypeOfGO::CHARACTER;
		pTempGO->team = eTeam::PLAYER;
		pTempGO->behaviour = eBehaviour::FOLLOWER;
		pTempGO->range = 4.0f;
		pTempGO->maxVel = 1.0f;
		pTempGO->health = 100.0f;
		pTempGO->scale = 0.02f;
		pTempGO->mySpeed = sGOSpeed( 1.0f, 0.4f, 0.5f, 0.5f );

		pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::BASE, "assets/models/morty/Idle.fbx" ) );
		pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::JUMP, "assets/models/morty/Jumping.fbx" ) );
		pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::FORWARD_JUMP, "assets/models/morty/Forward Jump.fbx" ) );
		pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::STRAFE_LEFT, "assets/models/morty/Walk Strafe Left.fbx" ) );
		pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::STRAFE_RIGHT, "assets/models/morty/Walk Strafe Right.fbx" ) );
		pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::MOVE_LEFT, "assets/models/morty/Crouched Sneaking Left.fbx" ) );
		pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::MOVE_RIGHT, "assets/models/morty/Crouched Sneaking Right.fbx" ) );
		pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::TURN_LEFT, "assets/models/morty/Left Turn.fbx" ) );
		pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::TURN_RIGHT, "assets/models/morty/Right Turn.fbx" ) );
		pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::RUN, "assets/models/morty/Run Forward.fbx" ) );
		pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::WALK_FORWARD, "assets/models/morty/Walking.fbx" ) );
		pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::WALK_BACKWARD, "assets/models/morty/Walk Backwards.fbx" ) );
		pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::ACTION, "assets/models/morty/Shuffling.fbx" ) );

		// This assigns the game object to the particular skinned mesh type 
		pTempGO->pSimpleSkinnedMesh = createSkinnedMesh( pTempGO->myAnimations );

		createMeshFromSkinnedMesh( pTempGO->pSimpleSkinnedMesh, shaderID, pVAOManager );

		// Add a default animation 
		pTempGO->pAniState = new cAnimationState();
		pTempGO->pAniState->defaultAnimation.name = "assets/models/morty/Idle.fbx";

		pTempGO->pAniState->defaultAnimation.frameStepTime = 0.01f;
		// Get the total time of the entire animation
		pTempGO->pAniState->defaultAnimation.totalTime = pTempGO->pSimpleSkinnedMesh->GetDuration();

		pTempGO->position = glm::vec3( -3.0f, 0.0, 0.0f );
		sMeshDrawInfo meshInfo;
		meshInfo.scale = pTempGO->scale;
		meshInfo.setMeshOrientationEulerAngles( glm::vec3( 0.0f, 0.0f, 0.0f ) );
		meshInfo.debugDiffuseColour = glm::vec4( 1.0f, 1.0f, 0.0f, 1.0f );
		meshInfo.name = pTempGO->pSimpleSkinnedMesh->friendlyName;
		meshInfo.vecMehs2DTextures.push_back( sTextureBindBlendInfo( "morty.bmp", 1.0f ) );
		pTempGO->vecMeshes.push_back( meshInfo );		
		::g_vecGameObjects.push_back( pTempGO );		// Fastest way to add
	}


	{	// Skinned mesh  model
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "Rick";

		pTempGO->type = eTypeOfGO::CHARACTER;
		pTempGO->team = eTeam::PLAYER;
		pTempGO->behaviour = eBehaviour::UNAVAIABLE;
		pTempGO->range = 4.0f;
		pTempGO->health = 100.0f;
		pTempGO->maxVel = 1.0f;
		pTempGO->health = 100.0f;
		pTempGO->scale = 0.02f;
		pTempGO->mySpeed = sGOSpeed( 1.5f, 0.8f, 1.0f, 1.0f );


		pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::BASE, "assets/models/rick/Breathing Idle.fbx" ) );
		pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::JUMP, "assets/models/rick/Jumping.fbx" ) );
		pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::FORWARD_JUMP, "assets/models/rick/Forward Jump.fbx" ) );
		pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::STRAFE_LEFT, "assets/models/rick/Left Strafe Walk.fbx" ) );
		pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::STRAFE_RIGHT, "assets/models/rick/Right Strafe Walk.fbx" ) );
		pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::MOVE_LEFT, "assets/models/rick/Crouched Sneaking Left.fbx" ) );
		pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::MOVE_RIGHT, "assets/models/rick/Crouched Sneaking Right.fbx" ) );
		pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::TURN_LEFT, "assets/models/rick/Left Turn.fbx" ) );
		pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::TURN_RIGHT, "assets/models/rick/Right Turn.fbx" ) );
		pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::RUN, "assets/models/rick/Running.fbx" ) );
		pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::WALK_FORWARD, "assets/models/rick/Walking.fbx" ) );
		pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::WALK_BACKWARD, "assets/models/rick/Walking Backward.fbx" ) );
		pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::ACTION, "assets/models/rick/Gangnam Style.fbx" ) );
		
		// This assigns the game object to the particular skinned mesh type 
		pTempGO->pSimpleSkinnedMesh = createSkinnedMesh( pTempGO->myAnimations );

		createMeshFromSkinnedMesh( pTempGO->pSimpleSkinnedMesh, shaderID, pVAOManager );

		// Add a default animation 
		pTempGO->pAniState = new cAnimationState();
		pTempGO->pAniState->defaultAnimation.name = "assets/models/rick/Idle.fbx";
		pTempGO->pAniState->defaultAnimation.frameStepTime = 0.01f;
		// Get the total time of the entire animation
		pTempGO->pAniState->defaultAnimation.totalTime = pTempGO->pSimpleSkinnedMesh->GetDuration();

		pTempGO->position = glm::vec3( 0.0f, 0.0, 0.0f );
		sMeshDrawInfo meshInfo;
		meshInfo.scale = pTempGO->scale;
		meshInfo.setMeshOrientationEulerAngles( glm::vec3( 0.0f, 0.0f, 0.0f ) );
		meshInfo.debugDiffuseColour = glm::vec4( 1.0f, 1.0f, 0.0f, 1.0f );
		meshInfo.name = pTempGO->pSimpleSkinnedMesh->friendlyName;
		meshInfo.vecMehs2DTextures.push_back( sTextureBindBlendInfo( "rick_texture.bmp", 1.0f ) );
		pTempGO->vecMeshes.push_back( meshInfo );

		::g_vecGameObjects.push_back( pTempGO );		// Fastest way to add
	}

	{	// THIS IS THE REFLECTIVE BUNNY
		cGameObject* pTempGO = new cGameObject();
		pTempGO->position = glm::vec3( 0.0f, 0.0f, -3.0f );

		cMesh theMesh;

		if( !myAssimpLoader.loadModelA( "assets/models/bunny.obj", theMesh, error ) )
		{
			std::cout << "All is lost! Forever lost!! Assimp didn't load the Model" << error << std::endl;
		}
		else
		{
			theMesh.name = "bunny";
			if( !pVAOManager->loadMeshIntoVAO( theMesh, shaderID, false ) )
			{
				std::cout << "Assimp loaded mesh didn't load into VAO" << std::endl;
			}

			sMeshDrawInfo meshInfo;
			meshInfo.debugDiffuseColour = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );
			meshInfo.scale = 1.0f;
			meshInfo.name = theMesh.name;

			// Make bunny reflective...
			meshInfo.bIsEnvirMapped = true;
			meshInfo.reflectBlendRatio = 0.5f;
			meshInfo.refractBlendRatio = 0.5f;
			meshInfo.coefficientRefract = 0.1f;

			pTempGO->friendlyName = "theBunny";

			meshInfo.vecMehs2DTextures.push_back( sTextureBindBlendInfo( "teste1.bmp", 1.0f ) );
			pTempGO->vecMeshes.push_back( meshInfo );

			::g_vecGameObjects.push_back( pTempGO );		// Fastest way to add
		}
	}
	
	return;
}