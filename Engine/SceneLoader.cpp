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

void LoadModelsIntoScene( void )
{

	{	// Skinned mesh  model
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "Morty";
		// This assigns the game object to the particular skinned mesh type 
		pTempGO->pSimpleSkinnedMesh = ::g_pRPGSkinnedMesh2;
		// Add a default animation 
		pTempGO->pAniState = new cAnimationState();
		pTempGO->pAniState->defaultAnimation.name = "assets/morty/Idle.fbx";

		pTempGO->pAniState->defaultAnimation.frameStepTime = 0.01f;
		// Get the total time of the entire animation
		pTempGO->pAniState->defaultAnimation.totalTime = pTempGO->pSimpleSkinnedMesh->GetDuration();

		pTempGO->position = glm::vec3( -2.0f, 0.0, 0.0f );
		//cPhysicalProperties physState;
		//physState.position = glm::vec3( -20.0f, 0.0, 0.0f ); //physState.position = glm::vec3(-50.0f, 0.0, 0.0f);
		//pTempGO->SetPhysState( physState );
		sMeshDrawInfo meshInfo;
		meshInfo.scale = 0.05f;
		meshInfo.setMeshOrientationEulerAngles( glm::vec3( 0.0f, 0.0f, 0.0f ) );
		meshInfo.debugDiffuseColour = glm::vec4( 1.0f, 1.0f, 0.0f, 1.0f );
		//meshInfo.bDrawAsWireFrame = true;		
		meshInfo.name = ::g_pRPGSkinnedMesh2->friendlyName;
		meshInfo.vecMehs2DTextures.push_back( sTextureBindBlendInfo( "morty.bmp", 1.0f ) );
		pTempGO->vecMeshes.push_back( meshInfo );
		::g_vecGameObjects.push_back( pTempGO );		// Fastest way to add
	}


	{	// Skinned mesh  model
		cGameObject* pTempGO = new cGameObject();
		pTempGO->friendlyName = "player";
		// This assigns the game object to the particular skinned mesh type 
		pTempGO->pSimpleSkinnedMesh = ::g_pRPGSkinnedMesh;
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
		meshInfo.scale = 0.05f;
		meshInfo.setMeshOrientationEulerAngles( glm::vec3( 0.0f, 0.0f, 0.0f ) );
		meshInfo.debugDiffuseColour = glm::vec4( 1.0f, 1.0f, 0.0f, 1.0f );
		//meshInfo.bDrawAsWireFrame = true;
		meshInfo.name = ::g_pRPGSkinnedMesh->friendlyName;
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