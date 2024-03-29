// Fragment shader
#version 400


in vec4 fColor;					
in vec3 fVertNormal;			// Also in "world" (no view or projection)
in vec3 fVecWorldPosition;		// 
in vec4 fUV_X2;					// Added: UV 1 and 2 to fragment
								// UV #1 is .xy 
								// UV #2 is .zw
in vec3 fTangent;		// For bump (or normal) mapping
in vec3 fBitangent;		// For bump (or normal) mapping



//uniform vec4 diffuseColour;		// New variable

// gl_FragColor is deprecated after version 120
// Now we specify a specific variable.
// If there is only 1, then GL will assume it's the colour 
//out vec4 fragColourOut;			// Assumes it GL_COLOR_ATTACHMENT0

//out vec4 fragOut[0];			// Assumes it GL_COLOR_ATTACHMENT0
struct sFBOout
{
	vec4 colour;			// GL_COLOR_ATTACHMENT0
	vec4 normal;			// GL_COLOR_ATTACHMENT1
	vec4 vertexWorldPos;	// GL_COLOR_ATTACHMENT2
};
out sFBOout fragOut;

//out vec4 fragColour[3];		// [0] is colour  GL_COLOR_ATTACHMENT0
//                              // [1] is normal  GL_COLOR_ATTACHMENT1
//                              // [2] is vertex world position
//#define COLOUR_LAYER 0
//#define NORMAL_LAYER 1
//#define VERT_WORLD_LAYEER 2
//fragColour[COLOUR_LAYER]...
//fragColour[NORMAL_LAYER]...
// ... and so on


// The values our OBJECT material
uniform vec4 materialDiffuse;	
//uniform vec4 materialAmbient;   		// 0.2
uniform float ambientToDiffuseRatio; 	// Maybe	// 0.2 or 0.3
uniform vec4 materialSpecular;  // rgb = colour of HIGHLIGHT only
								// w = shininess of the 


layout(std140) uniform NUB_perFrame
{
	vec3 eyePosition;	// Camera position
} perFramNUB;

/*****************************************************/
struct sLightDesc {
	vec4 position;
	vec4 diffuse;
	vec4 ambient;
	vec4 specular;		// Colour (xyz), intensity (w)
	vec4 attenuation;	// x = constant, y = linear, z = quadratic
	vec4 direction;
	vec4 typeParams;	// x = type
						// 		0 = point
						// 		1 = directional
						// 		2 = spot
						// y = distance cut-off
	                    // z angle1, w = angle2		- only for spot
};
const int NUMBEROFLIGHTS = 100;
//layout (std140) uniform NUB_lighting
//{
//	sLightDesc myLights[NUMBEROFLIGHTS];
//} lightingNUB;

uniform sLightDesc myLight[NUMBEROFLIGHTS];

uniform int theLightId;

//uniform vec3 eyePosition;	// Camera position

uniform bool bIsDebugWireFrameObject;

// For Drunk Effect
uniform float DrunkOffset;
uniform int DrunkEffect;

// For Outline
uniform bool useOutline;

// Note: this CAN'T be an array (sorry). See 3D texture array
uniform sampler2D texSamp2D00;		// Represents a 2D image
uniform sampler2D texSamp2D01;		// Represents a 2D image
uniform sampler2D texSamp2D02;		// Represents a 2D image		
uniform sampler2D texSamp2D03;		// Represents a 2D image
uniform sampler2D texSamp2D04;		// Represents a 2D image
uniform sampler2D texSamp2D05;		// Represents a 2D image
uniform sampler2D texSamp2D06;		// Represents a 2D image
uniform sampler2D texSamp2D07;		// Represents a 2D image
// ... and so on...


uniform float texBlend00;		
uniform float texBlend01;		
uniform float texBlend02;	
uniform float texBlend03;		
uniform float texBlend04;
uniform float texBlend05;		
uniform float texBlend06;
uniform float texBlend07;
// .... and so on... 

uniform bool isASkyBox;				// True samples the skybox texture
uniform samplerCube texSampCube00;
uniform samplerCube texSampCube01;
uniform samplerCube texSampCube02;
uniform samplerCube texSampCube03;
uniform float texCubeBlend00;
uniform float texCubeBlend01;
uniform float texCubeBlend02;
uniform float texCubeBlend03;

// For env. mapping (reflection and refraction)
uniform bool isReflectRefract;
uniform float reflectBlendRatio;		// How much reflection (0-1)
uniform float refractBlendRatio;		// How much refraction (0-1)
uniform float coefficientRefract; 		// coefficient of refraction 


//uniform bool bIsSecondPass;			// True if the render is doing 2nd pass  
uniform int renderPassNumber;			// Now there are 3 passes, 0 to 2

//uniform sampler2D tex2ndPassSamp2D;		// Offscreen texture for 2nd pass
uniform sampler2D texFBOColour2D;
uniform sampler2D texFBONormal2D;
//uniform sampler2D texFBODepth2D;	//ADDED
uniform sampler2D texFBOVertexWorldPos2D;

uniform sampler2D fullRenderedImage2D;

uniform float screenWidth;
uniform float screenHeight;


// This function has now been moved to the bottom of the file...
// Calculate the contribution of a light at a vertex
vec3 calcLightColour( in vec3 vecNormal, 
                      in vec3 vecWorldPosition, 
                      in int lightID, 
					  in vec3 matDiffuse, 
                      in vec4 matSpecular );
/*****************************************************/

const float CALCULATE_LIGHTING = 1.0f;
const float DONT_CALCULATE_LIGHTING = 0.25f;

const int PASS_0_G_BUFFER_PASS = 0;
const int PASS_1_DEFERRED_RENDER_PASS = 1;
const int PASS_2_FULL_SCREEN_EFFECT_PASS = 2;

void main()
{	
	fragOut.colour = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	fragOut.normal = vec4( 0.0f, 0.0f, 0.0f, DONT_CALCULATE_LIGHTING );
	fragOut.vertexWorldPos = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	
	//if ( renderPassNumber == 0 )	// THE FIRST PASS
	//{
	switch (renderPassNumber)
	{
	case PASS_0_G_BUFFER_PASS:	 // =0
	
		// Is this a 'debug' wireframe object, i.e. no lighting, just use diffuse
		if ( bIsDebugWireFrameObject )
		{
			fragOut.colour.rgb = materialDiffuse.rgb;			//gl_FragColor.rgb
			fragOut.colour.a = materialDiffuse.a;				//gl_FragColor.a = 1.0f	

			fragOut.vertexWorldPos.xyz = fVecWorldPosition.xyz;
			fragOut.normal.a = DONT_CALCULATE_LIGHTING;

			return;		// Immediate return
		}


		if ( isASkyBox )
		{	// Sample from skybox texture and exit
			// I pass texture coords to 'sample' 
			//	returning a colour (at that point in the texture)
			// Note we are using the normals of our skybox object
			//	to determine the point on the inside of the box
			vec4 skyRGBA = texture( texSampCube00, fVertNormal.xyz );
		
			fragOut.colour = vec4(skyRGBA.rgb, 1.0f);

			fragOut.vertexWorldPos.xyz = fVecWorldPosition.xyz;
			fragOut.normal.a = DONT_CALCULATE_LIGHTING;

			return;	
		}
	
	// uniform bool isReflectRefract;
	// uniform float reflectBlendRatio;		// How much reflection (0-1)
	// uniform float refractBlendRatio;		// How much refraction (0-1)
	// uniform float coefficientRefract; 	// coefficient of refraction 
		if ( isReflectRefract )
		{			
			// Have "eyePosition" (camera eye) in WORLD space
		
			// reFLECTion value 
			vec3 vecReflectEyeToVertex = fVecWorldPosition - perFramNUB.eyePosition;
			vecReflectEyeToVertex = normalize(vecReflectEyeToVertex);
			vec3 vecReflect = reflect( vecReflectEyeToVertex, fVertNormal.xyz );
			// Look up colour for reflection
			vec4 rgbReflection = texture( texSampCube00, fVertNormal.xyz );
	
			vec3 vecReFRACT_EyeToVertex = perFramNUB.eyePosition - fVecWorldPosition;
			vecReFRACT_EyeToVertex = normalize(vecReFRACT_EyeToVertex);				
			vec3 vecRefract = refract( vecReFRACT_EyeToVertex, fVertNormal.xyz, 
									   coefficientRefract );
			// Look up colour for reflection
			vec4 rgbRefraction = texture( texSampCube00, vecRefract );
			
			// Mix the two, based on how reflective the surface is
			fragOut.colour = (rgbReflection * reflectBlendRatio) + 
							 (rgbRefraction * refractBlendRatio);
		
			fragOut.vertexWorldPos.xyz = fVecWorldPosition.xyz;
			fragOut.normal.a = DONT_CALCULATE_LIGHTING;
			return;	
		}	
	
		// ***********************************************************************
		// This is the start of our "normal" "things that are lit" 1st pass
		// (the "geometry" pass - writing the geometry and other info)

		vec3 matDiffuse = vec3(0.0f, 0.0f, 0.0f);
	
		// ****************************************************************/
		//uniform sampler2D myAmazingTexture00;
		vec2 theUVCoords = fUV_X2.xy;		// use UV #1 of vertex
		
		vec4 texCol00 = texture( texSamp2D00, theUVCoords.xy );
		vec4 texCol01 = texture( texSamp2D01, theUVCoords.xy );
		vec4 texCol02 = texture( texSamp2D02, theUVCoords.xy );
		vec4 texCol03 = texture( texSamp2D03, theUVCoords.xy );
		vec4 texCol04 = texture( texSamp2D04, theUVCoords.xy );
		vec4 texCol05 = texture( texSamp2D05, theUVCoords.xy );
		vec4 texCol06 = texture( texSamp2D06, theUVCoords.xy );
		vec4 texCol07 = texture( texSamp2D07, theUVCoords.xy );
		//... and so on (to how many textures you are using)
	//	
		// use the blend value to combine textures
		matDiffuse.rgb += (texCol00.rgb * texBlend00) + 
						  (texCol01.rgb * texBlend01) + 
						  (texCol02.rgb * texBlend02) + 
						  (texCol03.rgb * texBlend03) +
						  (texCol04.rgb * texBlend04) +
						  (texCol05.rgb * texBlend05) +
						  (texCol06.rgb * texBlend06) +
						  (texCol07.rgb * texBlend07);
		
	
		// We will look at specular or gloss maps later, 
		// 	but making the specular white is fine
		vec4 matSpecular = vec4(1.0f, 1.0f, 1.0f, 64.0f);


		// Add the ambient here (AFTER the lighting)
		// We have materialAmbient, but ambient is often 
		//	just a percentage ratio of the diffuse
		vec3 ambientContribution = matDiffuse.rgb * ambientToDiffuseRatio;
		//fragColourOut[0].rgb += ambientContribution.rgb;
		fragOut.colour.rgb += ambientContribution.rgb;
	
		// Transparency value (for alpha blending)
		fragOut.colour.a = materialDiffuse.a;

		fragOut.normal.rgb = fVertNormal.xyz;

		fragOut.vertexWorldPos.xyz = fVecWorldPosition.xyz;

		// HACK TO HAVE ONLY THE COLOR AT THE SCREEN
		//fragOut.normal.a = CALCULATE_LIGHTING;
		fragOut.normal.a = DONT_CALCULATE_LIGHTING;
		fragOut.colour.rgb = matDiffuse.rgb;

		////NO lighting pass here (there WOULD be if this were "forward renderered")
		////****************************************************************/	
		//for ( int index = 0; index < NUMBEROFLIGHTS; index++ )
		//{
		//	fragOut.colour.rgb += calcLightColour( fVertNormal, 					
		//		                                    fVecWorldPosition, 
		//		                                    index, 
		//		                                    matDiffuse, 
		//		                                    materialSpecular );
		//}

		//return;
		break;
	//}

	//else if ( renderPassNumber == 1 )		// THIS IS THE SECOND PASS
	//{
	case PASS_1_DEFERRED_RENDER_PASS:		// (1)
	
		vec2 textCoords = vec2( gl_FragCoord.x / screenWidth, gl_FragCoord.y / screenHeight );

		vec3 theColourAtThisPixel = texture( texFBOColour2D, textCoords).rgb;
		vec4 theNormalAtThisPixel = texture( texFBONormal2D, textCoords).rgba;
		vec3 theVertLocWorldAtThisPixel = texture( texFBOVertexWorldPos2D, textCoords).rgb;

		//vec4 theDepthAtThisPixel = texture( texFBODepth2D, textCoords).rgba;		//ADDED

		if ( theNormalAtThisPixel.a != CALCULATE_LIGHTING )
		{
			// Return the colour as it is on the colour FBO
			fragOut.colour.rgb = theColourAtThisPixel.rgb;
			fragOut.colour.a = 1.0f;

			// "2nd pass effects"		
			// ****************************************************************

			// ==============  Edge Detection  ===============
			if( useOutline == true )
			{
				float dx = 0.1 / screenWidth;
				float dy = 0.1 / screenHeight;

				//vec3 center = sampleNrm( uTexNormals, vec2(0.0, 0.0) );
				vec3 center = texture( texFBONormal2D, vec2(textCoords.x, textCoords.y )).rgb;

				// sampling just these 3 neighboring fragments keeps the outline thin.				
				vec3 top = texture( texFBONormal2D, vec2(textCoords.x, textCoords.y + dy )).rgb;
				vec3 topRight = texture( texFBONormal2D, vec2(textCoords.x + dx, textCoords.y + dy )).rgb;
				vec3 right = texture( texFBONormal2D, vec2(textCoords.x + dx, textCoords.y )).rgb;

				vec3 t = center - top;
				vec3 r = center - right;
				vec3 tr = center - topRight;

				t = abs( t );
				r = abs( r );
				tr = abs( tr );

				float n;
				n = max( n, t.x );
				n = max( n, t.y );
				n = max( n, t.z );
				n = max( n, r.x );
				n = max( n, r.y );
				n = max( n, r.z );
				n = max( n, tr.x );
				n = max( n, tr.y );
				n = max( n, tr.z );

				// threshold and scale.
				n = 1.0 - clamp( clamp((n * 2.0) - 0.8, 0.0, 1.0) * 1.5, 0.0, 1.0 );

				fragOut.colour.rgb = texture(texFBOColour2D, textCoords).rgb * (0.1 + 0.9*n);
			}
			// ==============  Edge Detection  ===============
			

			// ****************************************************************
			// Drunk Offset		
			if( DrunkEffect == 1 )
			{
				vec3 FPixel = texture( texFBOColour2D, 
									  vec2(textCoords.x + 0, textCoords.y + DrunkOffset) ).rgb;
		
							  
				vec3 SPixel = texture( texFBOColour2D, 
									  vec2(textCoords.x + DrunkOffset, textCoords.y + DrunkOffset) ).rgb;
							  
				vec3 TPixel = texture( texFBOColour2D, 
									  vec2(textCoords.x - DrunkOffset, textCoords.y - DrunkOffset) ).rgb;
		
				fragOut.colour.rgb = fragOut.colour.rgb * 0.25f +
									 FPixel.rgb * 0.25f +
									 SPixel.rgb * 0.25f +
									 TPixel.rgb * 0.25f;
			}
			return;
		}
		else
		{	// ELSE: do the lighting...
			// ****************************************************************
			// NOW, in the 2nd pass, we do the lighting!!! 
			for ( int index = 0; index < NUMBEROFLIGHTS; index++ )
			{
				fragOut.colour.rgb += calcLightColour( theNormalAtThisPixel.xyz, 					
														theVertLocWorldAtThisPixel, 
														index, 
														theColourAtThisPixel, 
														materialSpecular );
			}

			//fragOut.colour.rgb *= 1.5f;		// dim projector
			fragOut.colour.a = 1.0f;

			//return;
		}
		break;
	//}
	
	//else			// The Third Pass
	//{
	case PASS_2_FULL_SCREEN_EFFECT_PASS:	

		//fragOut.colour.rgb = vec3( 0.0f );
		//fragOut.colour.r = 1.0f;

		//vec2 textCoords2 = vec2( gl_FragCoord.x / screenWidth, gl_FragCoord.y / screenHeight );

		//vec3 theColourAtThisPixel2 = texture( fullRenderedImage2D, fUV_X2.xy).rgb;
		//vec4 theNormalAtThisPixel2 = texture( texFBONormal2D, fUV_X2.xy).rgba;
		//vec3 theVertLocWorldAtThisPixel2 = texture( texFBOVertexWorldPos2D, fUV_X2.xy).rgb;

		//if ( theNormalAtThisPixel2.a != CALCULATE_LIGHTING )
		//{
		//	// Return the colour as it is on the colour FBO
		//	fragOut.colour.rgb = theColourAtThisPixel2.rgb;
		//	fragOut.colour.a = 1.0f;
		//}
		//else
		//{
		//	// ELSE: do the lighting...
		//	for ( int index = 0; index < NUMBEROFLIGHTS; index++ )
		//	{
		//		fragOut.colour.rgb += calcLightColour( theNormalAtThisPixel2.xyz, 					
		//											   theVertLocWorldAtThisPixel2, 
		//											   index, 
		//											   theColourAtThisPixel2, 
		//											   materialSpecular );
		//	}
		//}// if ( theNormalAtThisPixel.a != CALCULATE_LIGHTING )

		////fragOut.colour.rgb = texture( fullRenderedImage2D, fUV_X2.xy ).rgb;
		////fragOut.colour.a = 1.0f;

		////Make it  black and white (well, "greyscale"
		//float Y = (0.2126 * fragOut.colour.r) + 
		//          (0.7152 * fragOut.colour.g) + 
		//		  (0.0722 * fragOut.colour.b);
		//fragOut.colour.rgb = vec3(Y,Y,Y);
	
		break;	// end of pass PASS3

	case 3:
	
		// In this example, there is a single quad, that
		//	is being drawn with the full, rendered buffer from the previous pass
		fragOut.colour.rgb = texture( fullRenderedImage2D, fUV_X2.xy ).rgb;
		fragOut.colour.a = 1.0f;
	}
	
}	// void main()

// Inspired by Mike Bailey's Graphics Shader book
// (when you see "half-vector", that's eye space)
// Technically, a "per-pixel" or a form of Phong shading
// Calcualte the contribution of a light at a vertex
vec3 calcLightColour( in vec3 vecNormal, 
                      in vec3 vecWorldPosition, 
                      in int lightID, 
                      in vec3 matDiffuse, 	// ADDED
                      in vec4 matSpecular )	// ADDED
{
	vec3 colour = vec3( 0.0f, 0.0f, 0.0f );
	
	vec3 outDiffuse = vec3( 0.0f, 0.0f, 0.0f );
	vec3 outSpecular = vec3( 0.0f, 0.0f, 0.0f );
	

	// Distance between light and vertex (in world)
	vec3 vecToLight = myLight[lightID].position.xyz - vecWorldPosition;
	// How long is this vector?
	float lightDistance = length(vecToLight);
	// Short circuit distance
	if ( myLight[lightID].typeParams.y <  lightDistance )
	{
		return colour;
	}
	
	// The "normalized" vector to the light (don't care about distance, just direction)
	vec3 lightVector = normalize( vecToLight );
	
	// The normalized version of the vertex normal
	vecNormal = normalize(vecNormal);
	
	// How much diffuse light is being "reflected" off the surface 
	float diffFactor = max(0.0f, dot( lightVector, vecNormal ));
	
	
	outDiffuse.rgb = myLight[lightID].diffuse.rgb 		// Light contribution
	                 * matDiffuse.rgb				// Material contribution
					 * diffFactor;						// Factor based on direction
					 

// Simple linear attenuation
//	float attenuation = 1.0f / lightDistance; 
	
	// From the "Shader" book article
	// 1.0 dividedBy ( constant 
	//                + linear * distance 
	//                + quadratic * distance * distance)
	// Our light description is: 
	// 	vec4 attenuation;	// x = constant, 
	//                         y = linear, 
	//                         z = quadratic
	float attenuation = 1.0f / 
	   ( myLight[lightID].attenuation.x // 0  
	   + myLight[lightID].attenuation.y * lightDistance			// Linear
	   + myLight[lightID].attenuation.z * lightDistance * lightDistance );	// Quad
	outDiffuse.rgb *= attenuation;

	
	// Vector from vertex to eye 
	// i.e. this makes the vector base effectively at zero.
	vec3 viewVector = normalize( perFramNUB.eyePosition - vecWorldPosition );
	vec3 vecLightReflection = reflect( normalize(lightVector), vecNormal );
	
	float specularShininess = matSpecular.w;	// 64.0f
	vec3 specMatColour = matSpecular.rgb;		// vec3(1.0f, 1.0f, 1.0f);
	
	outSpecular.rgb = pow( max(0.0f, dot(viewVector,vecLightReflection)), 
	                  specularShininess)
			          * specMatColour
				      * myLight[lightID].specular.rgb;// // myLightSpecular;
				   
	outSpecular *= attenuation;
	
	
// For now, to simplify, eliminate the specular
	colour = outDiffuse + outSpecular;
//	colour = outDiffuse;


	// Now we have the colour if this was a point light 
	// See if it's a spot light...
	if ( myLight[lightID].typeParams.x == 2.0f ) 			// x = type
	{	// Then it's a spot light! 
		// 		0 = point
		// 		1 = directional
		// 		2 = spot
	    // z angle1, w = angle2		- only for spot
		
		// Vector from the vertex to the light... 
		vec3 vecVertexToLight = vecWorldPosition - myLight[lightID].position.xyz;
		// Normalize to unit length vector
		vec3 vecVtoLDirection = normalize(vecVertexToLight);
		
		float vertSpotAngle = max(0.0f, dot( vecVtoLDirection, myLight[lightID].direction.xyz ));
		// Is this withing the angle1?
		
		float angleInsideCutCos = cos(myLight[lightID].typeParams.z);		// z angle1
		float angleOutsideCutCos = cos(myLight[lightID].typeParams.w);		// z angle2
		
		if ( vertSpotAngle <= angleOutsideCutCos )
		{	// NO, it's outside this angle1
			colour = vec3(0.0f, 0.0f, 0.0f );
		}
		else if ( (vertSpotAngle > angleOutsideCutCos ) && 
		          (vertSpotAngle <= angleInsideCutCos) )
		{	// inside the 'penumbra' region
			// Use smoothstep to get the gradual drop off in brightness
			float penumbraRatio = smoothstep(angleOutsideCutCos, 
			                                 angleInsideCutCos, 
										     vertSpotAngle );          
			
			colour *= penumbraRatio;
		}
//		else if ( vertSpotAngle <= angleInsideCutCos )
//		{
//			// Full brightness .... do nothing
//		}

//		colour.rgb*= 0.0001f;
//		colour.r = 1.0f;

	}//if ( typeParams.x

	
	return colour;
}// vec3 calcLightColour(...) 