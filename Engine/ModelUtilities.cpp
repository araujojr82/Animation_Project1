#include "ModelUtilities.h" 
#include "cVAOMeshManager.h"
#include "cMesh.h"
#include <sstream>
#include <vector>
#include "cGameObject.h"
#include "cModelAssetLoader.h"
#include "assimp/cAssimpBasic.h"
#include <iostream>
#include "assimp/cSimpleAssimpSkinnedMeshLoader_OneMesh.h"

// Declared in globalStuff.h
cSimpleAssimpSkinnedMesh* g_pRPGSkinnedMesh = NULL;
cSimpleAssimpSkinnedMesh* g_pRPGSkinnedMesh2 = NULL;

// Returns 0 or NULL if not found
cGameObject* findObjectByFriendlyName( std::string friendlyName, std::vector<cGameObject*> &vec_pGameObjects )
{
	// Linear search, baby!
	unsigned int numObjects = ( unsigned int )vec_pGameObjects.size();
	for( unsigned int index = 0; index != numObjects; index++ )
	{
		if( vec_pGameObjects[index]->friendlyName == friendlyName )
		{
			return vec_pGameObjects[index];
		}
	}
	// Didn't find it
	return NULL;
}

cGameObject* findObjectByUniqueID( unsigned int ID, std::vector<cGameObject*> &vec_pGameObjects )
{
	// Linear search, baby!
	unsigned int numObjects = ( unsigned int )vec_pGameObjects.size();
	for( unsigned int index = 0; index != numObjects; index++ )
	{
		if( ID = vec_pGameObjects[index]->getUniqueID() )
		{
			return vec_pGameObjects[index];
		}
	}
	// Didn't find it
	return NULL;
}



bool Load3DModelsIntoMeshManager( int shaderID,
								  cVAOMeshManager* pVAOManager,
								  cModelAssetLoader* pModelAssetLoader,
								  std::string &error )
{
	std::stringstream ssError;
	bool bAllGood = true;


	//// *******************************************************
	//    ___  _    _                      _  __  __           _     
	//   / __|| |__(_) _ _   _ _   ___  __| ||  \/  | ___  ___| |_   
	//   \__ \| / /| || ' \ | ' \ / -_)/ _` || |\/| |/ -_)(_-<| ' \  
	//   |___/|_\_\|_||_||_||_||_|\___|\__,_||_|  |_|\___|/__/|_||_| 
	//                                                               
	::g_pRPGSkinnedMesh = new cSimpleAssimpSkinnedMesh();

	if( !::g_pRPGSkinnedMesh->LoadMeshFromFile( "assets/models/rick/Idle.fbx" ) )
	{
		std::cout << "Error: problem loading the skinned mesh" << std::endl;
	}
	// Now load another animation file...
	::g_pRPGSkinnedMesh->LoadMeshAnimation( "assets/models/rick/Breathing Idle.fbx" );
	::g_pRPGSkinnedMesh->LoadMeshAnimation( "assets/models/rick/Crouched Sneaking Left.fbx" );
	::g_pRPGSkinnedMesh->LoadMeshAnimation( "assets/models/rick/Crouched Sneaking Right.fbx" );
	::g_pRPGSkinnedMesh->LoadMeshAnimation( "assets/models/rick/Gangnam Style.fbx" );
	::g_pRPGSkinnedMesh->LoadMeshAnimation( "assets/models/rick/Jumping.fbx" );
	::g_pRPGSkinnedMesh->LoadMeshAnimation( "assets/models/rick/Left Strafe Walk.fbx" );
	::g_pRPGSkinnedMesh->LoadMeshAnimation( "assets/models/rick/Right Strafe Walk.fbx" );
	::g_pRPGSkinnedMesh->LoadMeshAnimation( "assets/models/rick/Running.fbx" );
	::g_pRPGSkinnedMesh->LoadMeshAnimation( "assets/models/rick/Walking Backward.fbx" );
	::g_pRPGSkinnedMesh->LoadMeshAnimation( "assets/models/rick/Walking.fbx" );


	::g_pRPGSkinnedMesh->friendlyName = "Rick";

	cMesh* pTheMesh = ::g_pRPGSkinnedMesh->CreateMeshObjectFromCurrentModel();

	if( pTheMesh )
	{
		if( !pVAOManager->loadMeshIntoVAO( *pTheMesh, shaderID, false ) )
		{
			std::cout << "Could not load skinned mesh model into new VAO" << std::endl;
		}
	}
	else
	{
		std::cout << "Could not create a cMesh object from skinned mesh file" << std::endl;
	}
	// Delete temporary mesh if still around
	if( pTheMesh )
	{
		delete pTheMesh;
	}

	//****************************************************************************************

	// Second Skinned Mesh (Just for now)
	::g_pRPGSkinnedMesh2 = new cSimpleAssimpSkinnedMesh();

	if( !::g_pRPGSkinnedMesh2->LoadMeshFromFile( "assets/models/morty/Idle.fbx" ) )
	{
		std::cout << "Error: problem loading the skinned mesh" << std::endl;
	}
	::g_pRPGSkinnedMesh2->LoadMeshAnimation( "assets/models/morty/Walking.fbx" );
	
	::g_pRPGSkinnedMesh2->friendlyName = "Morty";

	cMesh* pTheMesh2 = ::g_pRPGSkinnedMesh2->CreateMeshObjectFromCurrentModel();

	if( pTheMesh2 )
	{
		if( !pVAOManager->loadMeshIntoVAO( *pTheMesh2, shaderID, false ) )
		{
			std::cout << "Could not load skinned mesh model into new VAO" << std::endl;
		}
	}
	else
	{
		std::cout << "Could not create a cMesh object from skinned mesh file" << std::endl;
	}
	// Delete temporary mesh if still around
	if( pTheMesh2 )
	{
		delete pTheMesh2;
	}


	//{
	//	cMesh testMesh;
	//	testMesh.name = "ReallyBigShip";
	//	if ( ! pModelAssetLoader->LoadPlyFileIntoMeshWith_Normals_and_UV( "basestar_REALLY_BIG.ply", testMesh ) )
	//	{ 
	//		//std::cout << "Didn't load model" << std::endl;
	//		ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
	//		bAllGood = false;
	//	}
	//	// ***********************************************************************
	//	// NOTE the TRUE so that it keeps the mesh!!!
	//	else if ( ! pVAOManager->loadMeshIntoVAO( testMesh, shaderID, true ) )
	//	{
	//		//std::cout << "Could not load mesh into VAO" << std::endl;
	//		ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
	//		bAllGood = false;
	//	}
	//	// ***********************************************************************
	//}	
	//{
	//	cMesh testMesh;
	//	testMesh.name = "SmoothSphereRadius1";
	//	if ( ! pModelAssetLoader->LoadPlyFileIntoMeshWithNormals( "SmoothSphere_xyz_n.ply", testMesh ) )
	//	{ 
	//		//std::cout << "Didn't load model" << std::endl;
	//		ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
	//		bAllGood = false;
	//	}
	//	// ***********************************************************************
	//	// NOTE the TRUE so that it keeps the mesh!!!
	//	else if ( ! pVAOManager->loadMeshIntoVAO( testMesh, shaderID, true ) )
	//	{
	//		//std::cout << "Could not load mesh into VAO" << std::endl;
	//		ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
	//		bAllGood = false;
	//	}
	//	// ***********************************************************************
	//}	
	//{
	//	cMesh testMesh;
	//	testMesh.name = "SmoothSphereRadius1InvertedNormals";
	//	if( !pModelAssetLoader->LoadPlyFileIntoMeshWithNormals( "SmoothSphere_Inverted_Normals_xyz_n.ply", testMesh ) )
	//	{
	//		//std::cout << "Didn't load model" << std::endl;
	//		ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
	//		bAllGood = false;
	//	}
	//	// ***********************************************************************
	//	// NOTE the TRUE so that it keeps the mesh!!!
	//	else if( !pVAOManager->loadMeshIntoVAO( testMesh, shaderID, true ) )
	//	{
	//		//std::cout << "Could not load mesh into VAO" << std::endl;
	//		ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
	//		bAllGood = false;
	//	}
	//	// ***********************************************************************
	//}
	//{
	//	cMesh testMesh;
	//	testMesh.name = "MeshLabTerrain_xyz_n_uv";
	//	if( !pModelAssetLoader->LoadPlyFileIntoMeshWith_Normals_and_UV( "MeshLabTerrain_xyz_n_uv.ply", testMesh ) )
	//		//if ( ! pModelAssetLoader->LoadPlyFileIntoMeshWith_Normals_and_UV( "MeshLabTerrain_FLAT_xyz_n_uv.ply", testMesh ) )
	//	{
	//		//std::cout << "Didn't load model" << std::endl;
	//		ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
	//		bAllGood = false;
	//	}
	//	// ***********************************************************************
	//	// NOTE the TRUE so that it keeps the mesh!!!
	//	else if( !pVAOManager->loadMeshIntoVAO( testMesh, shaderID, true ) )
	//	{
	//		//std::cout << "Could not load mesh into VAO" << std::endl;
	//		ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
	//		bAllGood = false;
	//	}
	//	// ***********************************************************************
	//}
	//{
	//	cMesh testMesh;
	//	testMesh.name = "teapotUV";
	//	if( !pModelAssetLoader->LoadPlyFileIntoMeshWith_Normals_and_UV( "Utah_Teapot_xyz_n_uv.ply", testMesh ) )
	//	{
	//		//std::cout << "Didn't load model" << std::endl;
	//		ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
	//		bAllGood = false;
	//	}
	//	else if( !pVAOManager->loadMeshIntoVAO( testMesh, shaderID ) )
	//	{
	//		//std::cout << "Could not load mesh into VAO" << std::endl;
	//		ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
	//		bAllGood = false;
	//	}
	//}

	//{
	//	cMesh testMesh;
	//	testMesh.name = "bunny";
	//	if( !pModelAssetLoader->LoadPlyFileIntoMeshXYZOnly( "bun_zipper_res2_xyz.ply", testMesh ) )
	//	{
	//		//std::cout << "Didn't load model" << std::endl;
	//		ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
	//		bAllGood = false;
	//	}
	//	else if( !pVAOManager->loadMeshIntoVAO( testMesh, shaderID ) )
	//	{
	//		//std::cout << "Could not load mesh into VAO" << std::endl;
	//		ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
	//		bAllGood = false;
	//	}
	//}
	//{
	//	cMesh testMesh;
	//	testMesh.name = "teapot";
	//	if( !pModelAssetLoader->LoadPlyFileIntoMeshXYZOnly( "Utah_Teapot_1Unit_xyz.ply", testMesh ) )
	//	{
	//		//std::cout << "Didn't load model" << std::endl;
	//		ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
	//		bAllGood = false;
	//	}
	//	else if( !pVAOManager->loadMeshIntoVAO( testMesh, shaderID ) )
	//	{
	//		//std::cout << "Could not load mesh into VAO" << std::endl;
	//		ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
	//		bAllGood = false;
	//	}
	//}
	//{
	//	cMesh testMesh;
	//	testMesh.name = "dolphin";
	//	if( !pModelAssetLoader->LoadPlyFileIntoMeshXYZOnly( "dolphin_xyz.ply", testMesh ) )
	//	{
	//		//std::cout << "Didn't load model" << std::endl;
	//		ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
	//		bAllGood = false;
	//	}
	//	else if( !pVAOManager->loadMeshIntoVAO( testMesh, shaderID ) )
	//	{
	//		//std::cout << "Could not load mesh into VAO" << std::endl;
	//		ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
	//		bAllGood = false;
	//	}
	//}
	//{
	//	cMesh testMesh;
	//	testMesh.name = "PlaneXZ";
	//	if( !pModelAssetLoader->LoadPlyFileIntoMeshXYZOnly( "Flat_XZ_Plane_xyz.ply", testMesh ) )
	//	{
	//		//std::cout << "Didn't load model" << std::endl;
	//		ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
	//		bAllGood = false;
	//	}
	//	else if( !pVAOManager->loadMeshIntoVAO( testMesh, shaderID ) )
	//	{
	//		//std::cout << "Could not load mesh into VAO" << std::endl;
	//		ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
	//		bAllGood = false;
	//	}
	//}
	//{
	//	cMesh testMesh;
	//	testMesh.name = "SphereRadius1";
	//	//		if ( ! LoadPlyFileIntoMesh( "Sphereply_xyz.ply", testMesh ) )
	//	if( !pModelAssetLoader->LoadPlyFileIntoMeshWithNormals( "Sphereply_xyz_n.ply", testMesh ) )
	//	{
	//		//std::cout << "Didn't load model" << std::endl;
	//		ssError << "Didn't load model >" << testMesh.name << "<" << std::endl;
	//		bAllGood = false;
	//	}
	//	else if( !pVAOManager->loadMeshIntoVAO( testMesh, shaderID ) )
	//	{
	//		//std::cout << "Could not load mesh into VAO" << std::endl;
	//		ssError << "Could not load mesh >" << testMesh.name << "< into VAO" << std::endl;
	//		bAllGood = false;
	//	}
	//}	// ENDOF: load models

	if( !bAllGood )
	{
		// Copy the error string stream into the error string that
		//	gets "returned" (through pass by reference)
		error = ssError.str();
	}

	return bAllGood;
}