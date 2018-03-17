// This file is used to laod the models
#include "cGameObject.h"
#include <vector>
#include "Utilities.h"		// getRandInRange()
#include <glm/glm.hpp>
#include "globalGameStuff.h"

// For the cSimpleAssimpSkinnedMeshLoader class
#include "assimp/cSimpleAssimpSkinnedMeshLoader_OneMesh.h"

#include "cAnimationState.h"

extern std::vector< cGameObject* >  g_vecGameObjects;
extern cGameObject* g_pTheDebugSphere;

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
	
	{ // Mr. Meeseeks
	cGameObject* pTempGO = new cGameObject();
	pTempGO->friendlyName = "Meeseeks";

	pTempGO->type = eTypeOfGO::CHARACTER;
	pTempGO->team = eTeam::PLAYER;
	pTempGO->behaviour = eBehaviour::FOLLOWER;
	pTempGO->range = 4.0f;
	pTempGO->maxVel = 1.0f;
	pTempGO->health = 100.0f;
	pTempGO->scale = 0.05f;
	pTempGO->mySpeed = sGOSpeed( 1.0f, 0.4f, 0.5f, 0.5f );

	pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::BASE, "assets/models/meeseeks/Bored.fbx" ) );
	pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::WALK_FORWARD, "assets/models/meeseeks/Happy Walk.fbx" ) );
	pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::WALK_BACKWARD, "assets/models/meeseeks/Happy Walk Backward.fbx" ) );
	pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::ACTION, "assets/models/meeseeks/Walking Turn 180.fbx" ) );

	// This assigns the game object to the particular skinned mesh type 
	pTempGO->pSimpleSkinnedMesh = createSkinnedMesh( pTempGO->myAnimations );

	createMeshFromSkinnedMesh( pTempGO->pSimpleSkinnedMesh, shaderID, pVAOManager );

	// Add a default animation 
	pTempGO->pAniState = new cAnimationState();
	pTempGO->pAniState->defaultAnimation.name = "assets/models/meeseeks/Bored.fbx";

	pTempGO->pAniState->defaultAnimation.frameStepTime = 0.01f;
	// Get the total time of the entire animation
	pTempGO->pAniState->defaultAnimation.totalTime = pTempGO->pSimpleSkinnedMesh->GetDuration();

	pTempGO->position = glm::vec3( -2.0f, 0.0, 0.0f );
	sMeshDrawInfo meshInfo;
	meshInfo.scale = pTempGO->scale;
	meshInfo.setMeshOrientationEulerAngles( glm::vec3( 0.0f, 0.0f, 0.0f ) );
	meshInfo.debugDiffuseColour = glm::vec4( 1.0f, 1.0f, 0.0f, 1.0f );
	meshInfo.name = pTempGO->pSimpleSkinnedMesh->friendlyName;
	meshInfo.vecMehs2DTextures.push_back( sTextureBindBlendInfo( "meeseeks.bmp", 1.0f ) );
	pTempGO->vecMeshes.push_back( meshInfo );
	::g_vecGameObjects.push_back( pTempGO );		// Fastest way to add
	}

	//{	// Scary Terry
	//	cGameObject* pTempGO = new cGameObject();
	//pTempGO->friendlyName = "Scary";

	//pTempGO->type = eTypeOfGO::CHARACTER;
	//pTempGO->team = eTeam::PLAYER;
	//pTempGO->behaviour = eBehaviour::FOLLOWER;
	//pTempGO->range = 4.0f;
	//pTempGO->maxVel = 1.0f;
	//pTempGO->health = 100.0f;
	//pTempGO->scale = 0.05f;
	//pTempGO->mySpeed = sGOSpeed( 1.0f, 0.4f, 0.5f, 0.5f );

	//pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::BASE, "assets/models/scaryterry/Idle.fbx" ) );
	//pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::STRAFE_LEFT, "assets/models/scaryterry/Left Strafe Walking.fbx" ) );
	//pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::STRAFE_RIGHT, "assets/models/scaryterry/Right Strafe Walking.fbx" ) );
	//pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::WALK_FORWARD, "assets/models/scaryterry/Walking.fbx" ) );
	//pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::WALK_BACKWARD, "assets/models/scaryterry/Walk Backwards.fbx" ) );
	//pTempGO->myAnimations.push_back( sAnimDesc( eAnimationType::ACTION, "assets/models/scaryterry/Mutant Swiping.fbx" ) );

	//// This assigns the game object to the particular skinned mesh type 
	//pTempGO->pSimpleSkinnedMesh = createSkinnedMesh( pTempGO->myAnimations );

	//createMeshFromSkinnedMesh( pTempGO->pSimpleSkinnedMesh, shaderID, pVAOManager );

	//// Add a default animation 
	//pTempGO->pAniState = new cAnimationState();
	//pTempGO->pAniState->defaultAnimation.name = "assets/models/scaryterry/Idle.fbx";

	//pTempGO->pAniState->defaultAnimation.frameStepTime = 0.01f;
	//// Get the total time of the entire animation
	//pTempGO->pAniState->defaultAnimation.totalTime = pTempGO->pSimpleSkinnedMesh->GetDuration();

	//pTempGO->position = glm::vec3( -2.0f, 0.0, 0.0f );
	//sMeshDrawInfo meshInfo;
	//meshInfo.scale = pTempGO->scale;
	//meshInfo.setMeshOrientationEulerAngles( glm::vec3( 0.0f, 0.0f, 0.0f ) );
	//meshInfo.debugDiffuseColour = glm::vec4( 1.0f, 1.0f, 0.0f, 1.0f );
	//meshInfo.name = pTempGO->pSimpleSkinnedMesh->friendlyName;
	//meshInfo.vecMehs2DTextures.push_back( sTextureBindBlendInfo( "scary.bmp", 1.0f ) );
	//pTempGO->vecMeshes.push_back( meshInfo );
	//::g_vecGameObjects.push_back( pTempGO );		// Fastest way to add
	//}

	{	// Skinned mesh  model
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "Morty";

		pTempGO->type = eTypeOfGO::CHARACTER;
		pTempGO->team = eTeam::PLAYER;
		pTempGO->behaviour = eBehaviour::FOLLOWER;
		pTempGO->range = 4.0f;
		pTempGO->maxVel = 1.0f;
		pTempGO->health = 100.0f;
		pTempGO->scale = 0.05f;
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
		pTempGO->friendlyName = "player";

		pTempGO->type = eTypeOfGO::CHARACTER;
		pTempGO->team = eTeam::PLAYER;
		pTempGO->behaviour = eBehaviour::UNAVAIABLE;
		pTempGO->range = 4.0f;
		pTempGO->health = 100.0f;
		pTempGO->maxVel = 1.0f;
		pTempGO->health = 100.0f;
		pTempGO->scale = 0.05f;
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
		//pTempGO->pAniState->defaultAnimation.name = "assets/rick/rick_anim_walk.fbx";
		pTempGO->pAniState->defaultAnimation.name = "assets/models/rick/Idle.fbx";
		pTempGO->pAniState->defaultAnimation.frameStepTime = 0.01f;
		// Get the total time of the entire animation
		pTempGO->pAniState->defaultAnimation.totalTime = pTempGO->pSimpleSkinnedMesh->GetDuration();

		pTempGO->position = glm::vec3( 0.0f, 0.0, 0.0f );
		//cPhysicalProperties physState;
		//physState.position = glm::vec3( +20.0f, 0.0, 0.0f );
		//pTempGO->SetPhysState( physState );
		sMeshDrawInfo meshInfo;
		meshInfo.scale = pTempGO->scale;
		meshInfo.setMeshOrientationEulerAngles( glm::vec3( 0.0f, 0.0f, 0.0f ) );
		meshInfo.debugDiffuseColour = glm::vec4( 1.0f, 1.0f, 0.0f, 1.0f );
		//meshInfo.bDrawAsWireFrame = true;
		meshInfo.name = pTempGO->pSimpleSkinnedMesh->friendlyName;
		meshInfo.vecMehs2DTextures.push_back( sTextureBindBlendInfo( "rick_texture.bmp", 1.0f ) );
		pTempGO->vecMeshes.push_back( meshInfo );

		::g_vecGameObjects.push_back( pTempGO );		// Fastest way to add
	}

	//{	// THIS IS THE REFLECTIVE BUNNY
	//	cGameObject* pTempGO = new cGameObject();
	//	cPhysicalProperties physState;
	//	physState.integrationUpdateType = cPhysicalProperties::EXCLUDED_FROM_INTEGRATION;
	//	physState.mass = physState.inverseMass = 0.0f;	// Infinite
	//	physState.position.x = 75.0f;
	//	physState.position.y = 25.0f;
	//	//		physState.position.y = -100.0f;
	//	physState.setRotationalSpeedEuler( glm::vec3( 0.0f, 0.5f, 0.0f ) );
	//	pTempGO->SetPhysState( physState );


	//	sMeshDrawInfo meshInfo;
	//	meshInfo.debugDiffuseColour = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	//	meshInfo.scale = 350.0f;
	//	meshInfo.name = "bunny";

	//	// Make bunny reflective...
	//	meshInfo.bIsEnvirMapped = true;
	//	meshInfo.reflectBlendRatio = 0.5f;
	//	meshInfo.refractBlendRatio = 0.5f;
	//	meshInfo.coefficientRefract = 0.1f;

	//	pTempGO->friendlyName = "bugs";

	//	meshInfo.vecMehs2DTextures.push_back( sTextureBindBlendInfo( "barberton_etm_2001121_lrg.bmp", 1.0f ) );
	//	meshInfo.vecMehs2DTextures.push_back( sTextureBindBlendInfo( "height_map_norway-height-map-aster-30m.bmp", 0.0f ) );
	//	pTempGO->vecMeshes.push_back( meshInfo );

	//	::g_vecGameObjects.push_back( pTempGO );		// Fastest way to add
	//}
	
	//{	// THIS IS THE TERRAIN
	//	cGameObject* pTempGO = new cGameObject();
	//	cPhysicalProperties physState;
	//	physState.integrationUpdateType = cPhysicalProperties::EXCLUDED_FROM_INTEGRATION;
	//	physState.mass = physState.inverseMass = 0.0f;	// Infinite
	//	physState.position.y = -100.0f;
	//	pTempGO->SetPhysState( physState );

	//	sMeshDrawInfo meshInfo;
	//	meshInfo.scale = 1.0f;
	//	meshInfo.debugDiffuseColour = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	//	meshInfo.name = "MeshLabTerrain_xyz_n_uv";
	//	meshInfo.vecMehs2DTextures.push_back( sTextureBindBlendInfo( "GuysOnSharkUnicorn.bmp", 1.0f ) );
	//	meshInfo.vecMehs2DTextures.push_back( sTextureBindBlendInfo( "Utah_Teapot_xyz_n_uv_Enterprise.bmp", 0.0f ) );
	//	pTempGO->vecMeshes.push_back( meshInfo );

	//	::g_vecGameObjects.push_back( pTempGO );		// Fastest way to add
	//}

	//// Our skybox object
	//{
	//	//cGameObject* pTempGO = new cGameObject();
	//	::g_pSkyBoxObject = new cGameObject();
	//	cPhysicalProperties physState;
	//	::g_pSkyBoxObject->SetPhysState( physState );
	//	sMeshDrawInfo meshInfo;
	//	meshInfo.scale = 10000.0f;
	//	meshInfo.debugDiffuseColour = glm::vec4( 1.0f, 1.0f, 1.0f, 1.0f );
	//	meshInfo.name = "SmoothSphereRadius1InvertedNormals";
	//	meshInfo.vecMeshCubeMaps.push_back( sTextureBindBlendInfo( "space", 1.0f ) );
	//	meshInfo.bIsSkyBoxObject = true;
	//	::g_pSkyBoxObject->vecMeshes.push_back( meshInfo );
	//	// IS SKYBOX
	//	::g_vecGameObjects.push_back( ::g_pSkyBoxObject );		// Fastest way to add
	//}

	return;
}