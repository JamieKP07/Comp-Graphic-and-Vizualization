///////////////////////////////////////////////////////////////////////////////
// shadermanager.cpp
// ============
// manage the loading and rendering of 3D scenes
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//	Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <glm/gtx/transform.hpp>

// declaration of global variables
namespace
{
	const int NUM_SCENE_LIGHTS = 2;

	const std::string SCENE_LIGHT_NAMES[NUM_SCENE_LIGHTS] =
	{
		"light1",
		"light2"
	};

	const glm::vec3 SCENE_LIGHT_POSITIONS[NUM_SCENE_LIGHTS] =
	{
		glm::vec3(5.0f, 8.0f, 5.0f),
		glm::vec3(-5.0f, 6.0f, -3.0f)
	};

	const glm::vec3 SCENE_LIGHT_COLORS[NUM_SCENE_LIGHTS] =
	{
		glm::vec3(0.55f, 0.55f, 0.55f),
		glm::vec3(0.30f, 0.30f, 0.30f)
	};

	const glm::vec3 AMBIENT_LIGHT_COLOR = glm::vec3(0.22f, 0.22f, 0.22f);

	const char* g_ModelName = "model";
	const char* g_ColorValueName = "objectColor";
	const char* g_TextureValueName = "objectTexture";
	const char* g_UseTextureName = "bUseTexture";
	const char* g_UseLightingName = "bUseLighting";

	// Named texture tags improve readability and make the code easier to maintain
	const std::string TEXTURE_FLOOR = "floorTexture";
	const std::string TEXTURE_STONE = "stoneTexture";
	const std::string TEXTURE_ROOF = "roofTexture";
	const std::string TEXTURE_WOOD = "woodTexture";
	const std::string TEXTURE_SCROLL = "scrollTexture";
	const std::string TEXTURE_DICE_TRAY = "diceTrayTexture";
	const std::string TEXTURE_TOWER_BASE = "towerBaseTexture";
	const std::string TEXTURE_CORK = "corkTexture";
	const std::string TEXTURE_BOOK = "bookTexture";

	// Named material tags improve readability and make the code easier to maintain
	const std::string MATERIAL_FLOOR = "floorMaterial";
	const std::string MATERIAL_STONE = "stoneMaterial";
	const std::string MATERIAL_ROOF = "roofMaterial";
	const std::string MATERIAL_WOOD = "woodMaterial";

	// Scene object scales
	const glm::vec3 FLOOR_SCALE = glm::vec3(20.0f, 1.0f, 10.0f);
	const glm::vec3 TOWER_SCALE = glm::vec3(0.5f, 1.55f, 0.55f);
	const glm::vec3 ROOF_SCALE = glm::vec3(0.70f, 1.1f, 0.85f);
	const glm::vec3 TRAY_SCALE = glm::vec3(1.2f, 0.18f, 2.0f);
	const glm::vec3 BOOK_SCALE = glm::vec3(3.8f, 0.22f, 5.4f);
	const glm::vec3 SCROLL_SCALE = glm::vec3(2.5f, 0.02f, 1.8f);
	const glm::vec3 SCROLL_ROLL_SCALE = glm::vec3(0.5f, 3.50f, 0.5f);
	const glm::vec3 POTION_SCALE = glm::vec3(0.32f, 1.7f, 0.32f);
	const glm::vec3 CORK_SCALE = glm::vec3(0.22f, 0.18f, 0.22f);
	const glm::vec3 CUBE_DIE_SCALE = glm::vec3(0.25f, 0.25f, 0.25f);
	const glm::vec3 D4_SCALE = glm::vec3(0.25f, 0.25f, 0.25f);
	const glm::vec3 DICE_TRAY_BASE_SCALE = glm::vec3(3.2f, 0.06f, 2.8f);
	const glm::vec3 DICE_TRAY_WALL_SCALE = glm::vec3(0.08f, 0.22f, 2.8f);
	const glm::vec3 DICE_TRAY_FRONTBACK_SCALE = glm::vec3(3.2f, 0.22f, 0.08f);

	// Scene object positions
	const glm::vec3 FLOOR_POSITION = glm::vec3(0.0f, 0.0f, 0.0f);
	const glm::vec3 TOWER_POSITION = glm::vec3(-2.8f, 0.0f, 2.8f);
	const glm::vec3 ROOF_POSITION = glm::vec3(-2.8f, 1.35f, 2.8f);
	const glm::vec3 TRAY_POSITION = glm::vec3(-2.8f, 0.09f, 3.55f);
	const glm::vec3 BOOK_POSITION = glm::vec3(-8.5f, 0.18f, 3.0f);
	const glm::vec3 SCROLL_POSITION = glm::vec3(8.2f, 0.05f, 2.0f);
	const glm::vec3 SCROLL_ROLL_POSITION = glm::vec3(11.15f, 0.55f, 0.95f);
	const glm::vec3 POTION_POSITION = glm::vec3(3.8f, 0.0f, -3.8f);
	const glm::vec3 CORK_POSITION = glm::vec3(3.8f, 1.70f, -3.8f);
	const glm::vec3 CUBE_DIE_POSITION = glm::vec3(5.2f, 0.13f, 0.0f);
	const glm::vec3 D4_POSITION = glm::vec3(4.2f, 0.13f, 0.2f);

	const glm::vec3 DICE_TRAY_BASE_POSITION = glm::vec3(1.2f, 0.03f, 2.2f);
	const glm::vec3 DICE_TRAY_LEFT_WALL_POSITION = glm::vec3(-0.2f, 0.14f, 2.2f);
	const glm::vec3 DICE_TRAY_RIGHT_WALL_POSITION = glm::vec3(2.6f, 0.14f, 2.2f);
	const glm::vec3 DICE_TRAY_FRONT_WALL_POSITION = glm::vec3(1.2f, 0.14f, 1.0f);
	const glm::vec3 DICE_TRAY_BACK_WALL_POSITION = glm::vec3(1.2f, 0.14f, 3.4f);

	// Texture tiling values
	const float FLOOR_UV_X = 10.0f;
	const float FLOOR_UV_Y = 10.0f;
	const float TOWER_UV_X = 3.0f;
	const float TOWER_UV_Y = 4.0f;
	const float ROOF_UV_X = 3.0f;
	const float ROOF_UV_Y = 3.0f;
	const float TRAY_UV_X = 3.0f;
	const float TRAY_UV_Y = 2.0f;
}

/***********************************************************
 *  SceneManager()
 *
 *  The constructor for the class
 ***********************************************************/
SceneManager::SceneManager(ShaderManager* pShaderManager)
{
	m_pShaderManager = pShaderManager;
	m_basicMeshes = new ShapeMeshes();
	m_loadedTextures = 0;
}

/***********************************************************
 *  ~SceneManager()
 *
 *  The destructor for the class
 ***********************************************************/
SceneManager::~SceneManager()
{
	m_pShaderManager = NULL;
	delete m_basicMeshes;
	m_basicMeshes = NULL;
}

/***********************************************************
 *  CreateGLTexture()
 *
 *  This method is used for loading textures from image files,
 *  configuring the texture mapping parameters in OpenGL,
 *  generating the mipmaps, and loading the read texture into
 *  the next available texture slot in memory.
 ***********************************************************/
bool SceneManager::CreateGLTexture(const char* filename, std::string tag)
{
	int width = 0;
	int height = 0;
	int colorChannels = 0;
	GLuint textureID = 0;

	// indicate to always flip images vertically when loaded
	stbi_set_flip_vertically_on_load(true);

	// try to parse the image data from the specified image file
	unsigned char* image = stbi_load(
		filename,
		&width,
		&height,
		&colorChannels,
		0);

	// if the image was successfully read from the image file
	if (image)
	{
		std::cout << "Successfully loaded image:" << filename << ", width:" << width << ", height:" << height << ", channels:" << colorChannels << std::endl;

		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);

		// Prevent row-alignment issues when loading JPG textures
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		// set the texture wrapping parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// set texture filtering parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// if the loaded image is in RGB format
		if (colorChannels == 3)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		// if the loaded image is in RGBA format - it supports transparency
		else if (colorChannels == 4)
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
		else
		{
			std::cout << "Not implemented to handle image with " << colorChannels << " channels" << std::endl;
			return false;
		}

		// generate the texture mipmaps for mapping textures to lower resolutions
		glGenerateMipmap(GL_TEXTURE_2D);

		// free the image data from local memory
		stbi_image_free(image);
		glBindTexture(GL_TEXTURE_2D, 0); // Unbind the texture

		// register the loaded texture and associate it with the special tag string
		m_textureIDs[m_loadedTextures].ID = textureID;
		m_textureIDs[m_loadedTextures].tag = tag;
		m_loadedTextures++;

		return true;
	}

	std::cout << "Could not load image:" << filename << std::endl;

	// Error loading the image
	return false;
}

/***********************************************************
 *  BindGLTextures()
 *
 *  This method is used for binding the loaded textures to
 *  OpenGL texture memory slots.  There are up to 16 slots.
 ***********************************************************/
void SceneManager::BindGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		// bind textures on corresponding texture units
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  DestroyGLTextures()
 *
 *  This method is used for freeing the memory in all the
 *  used texture memory slots.
 ***********************************************************/
void SceneManager::DestroyGLTextures()
{
	for (int i = 0; i < m_loadedTextures; i++)
	{
		glDeleteTextures(1, &m_textureIDs[i].ID);
	}
}

/***********************************************************
 *  FindTextureID()
 *
 *  This method is used for getting an ID for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureID(std::string tag)
{
	int textureID = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureID = m_textureIDs[index].ID;
			bFound = true;
		}
		else
			index++;
	}

	return(textureID);
}

/***********************************************************
 *  FindTextureSlot()
 *
 *  This method is used for getting a slot index for the previously
 *  loaded texture bitmap associated with the passed in tag.
 ***********************************************************/
int SceneManager::FindTextureSlot(std::string tag)
{
	int textureSlot = -1;
	int index = 0;
	bool bFound = false;

	while ((index < m_loadedTextures) && (bFound == false))
	{
		if (m_textureIDs[index].tag.compare(tag) == 0)
		{
			textureSlot = index;
			bFound = true;
		}
		else
			index++;
	}

	return(textureSlot);
}

/***********************************************************
 *  FindMaterial()
 *
 *  This method is used for getting a material from the previously
 *  defined materials list that is associated with the passed in tag.
 ***********************************************************/
bool SceneManager::FindMaterial(std::string tag, OBJECT_MATERIAL& material)
{
	if (m_objectMaterials.size() == 0)
	{
		return(false);
	}

	int index = 0;
	bool bFound = false;
	while ((index < m_objectMaterials.size()) && (bFound == false))
	{
		if (m_objectMaterials[index].tag.compare(tag) == 0)
		{
			bFound = true;
			material.ambientColor = m_objectMaterials[index].ambientColor;
			material.ambientStrength = m_objectMaterials[index].ambientStrength;
			material.diffuseColor = m_objectMaterials[index].diffuseColor;
			material.specularColor = m_objectMaterials[index].specularColor;
			material.shininess = m_objectMaterials[index].shininess;
		}
		else
		{
			index++;
		}
	}

	return(bFound);
}

/***********************************************************
 *  SetTransformations()
 *
 *  This method is used for setting the transform buffer
 *  using the passed in transformation values.
 ***********************************************************/
void SceneManager::SetTransformations(
	glm::vec3 scaleXYZ,
	float XrotationDegrees,
	float YrotationDegrees,
	float ZrotationDegrees,
	glm::vec3 positionXYZ)
{
	// variables for this method
	glm::mat4 modelView;
	glm::mat4 scale;
	glm::mat4 rotationX;
	glm::mat4 rotationY;
	glm::mat4 rotationZ;
	glm::mat4 translation;

	// set the scale value in the transform buffer
	scale = glm::scale(scaleXYZ);
	// set the rotation values in the transform buffer
	rotationX = glm::rotate(glm::radians(XrotationDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
	rotationY = glm::rotate(glm::radians(YrotationDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
	rotationZ = glm::rotate(glm::radians(ZrotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
	// set the translation value in the transform buffer
	translation = glm::translate(positionXYZ);

	modelView = translation * rotationX * rotationY * rotationZ * scale;

	if (m_pShaderManager != nullptr)
	{
		m_pShaderManager->setMat4Value(g_ModelName, modelView);
	}
}

/***********************************************************
 *  SetShaderColor()
 *
 *  This method is used for setting the passed in color
 *  into the shader for the next draw command
 ***********************************************************/
void SceneManager::SetShaderColor(
	float redColorValue,
	float greenColorValue,
	float blueColorValue,
	float alphaValue)
{
	// variables for this method
	glm::vec4 currentColor;

	currentColor.r = redColorValue;
	currentColor.g = greenColorValue;
	currentColor.b = blueColorValue;
	currentColor.a = alphaValue;

	if (m_pShaderManager != nullptr)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, false);
		m_pShaderManager->setVec4Value(g_ColorValueName, currentColor);
	}
}

/***********************************************************
 *  SetShaderTexture()
 *
 *  This method is used for setting the texture data
 *  associated with the passed in ID into the shader.
 ***********************************************************/
void SceneManager::SetShaderTexture(
	std::string textureTag)
{
	if (m_pShaderManager != nullptr)
	{
		m_pShaderManager->setIntValue(g_UseTextureName, true);

		int textureID = -1;
		textureID = FindTextureSlot(textureTag);
		m_pShaderManager->setSampler2DValue(g_TextureValueName, textureID);
	}
}

/***********************************************************
 *  SetTextureUVScale()
 *
 *  This method is used for setting the texture UV scale
 *  values into the shader.
 ***********************************************************/
void SceneManager::SetTextureUVScale(float u, float v)
{
	if (m_pShaderManager != nullptr)
	{
		m_pShaderManager->setVec2Value("UVscale", glm::vec2(u, v));
	}
}

/***********************************************************
 *  SetShaderMaterial()
 *
 *  This method is used for passing the material values
 *  into the shader.
 ***********************************************************/
void SceneManager::SetShaderMaterial(
	std::string materialTag)
{
	if (m_objectMaterials.size() > 0)
	{
		OBJECT_MATERIAL material;
		bool bReturn = false;

		bReturn = FindMaterial(materialTag, material);
		if (bReturn == true)
		{
			m_pShaderManager->setVec3Value("material.ambientColor", material.ambientColor);
			m_pShaderManager->setFloatValue("material.ambientStrength", material.ambientStrength);
			m_pShaderManager->setVec3Value("material.diffuseColor", material.diffuseColor);
			m_pShaderManager->setVec3Value("material.specularColor", material.specularColor);
			m_pShaderManager->setFloatValue("material.shininess", material.shininess);
		}
	}
}

/**************************************************************/
/*** STUDENTS CAN MODIFY the code in the methods BELOW for  ***/
/*** preparing and rendering their own 3D replicated scenes.***/
/*** Please refer to the code in the OpenGL sample project  ***/
/*** for assistance.                                        ***/
/**************************************************************/


/***********************************************************
 *  PrepareScene()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene
 *  rendering
 ***********************************************************/
void SceneManager::PrepareScene()
{
	// Load the meshes used to build the dice tower object
	// Only one instance of each mesh needs to be loaded in memory
	m_basicMeshes->LoadPlaneMesh();
	m_basicMeshes->LoadCylinderMesh();
	m_basicMeshes->LoadConeMesh();
	m_basicMeshes->LoadBoxMesh();
	m_basicMeshes->LoadPyramid3Mesh();

	// Load texture images for the scene
	// Each texture load is checked so the program can report a failure clearly
	if (!CreateGLTexture("Textures/cobble_floor.jpg", TEXTURE_FLOOR))
	{
		std::cout << "Failed to load texture: cobble_floor.jpg" << std::endl;
	}

	if (!CreateGLTexture("Textures/stone_blocks.jpg", TEXTURE_STONE))
	{
		std::cout << "Failed to load texture: stone_blocks.jpg" << std::endl;
	}

	if (!CreateGLTexture("Textures/roof_shingles.jpg", TEXTURE_ROOF))
	{
		std::cout << "Failed to load texture: roof_shingles.jpg" << std::endl;
	}

	if (!CreateGLTexture("Textures/wood_planks.jpg", TEXTURE_WOOD))
	{
		std::cout << "Failed to load texture: wood_planks.jpg" << std::endl;
	}
	if (!CreateGLTexture("Textures/parchment_paper.jpg", TEXTURE_SCROLL))
	{
		std::cout << "Failed to load texture: parchment_paper.jpg" << std::endl;
	}

	if (!CreateGLTexture("Textures/velvet_interior.jpg", TEXTURE_DICE_TRAY))
	{
		std::cout << "Failed to load texture: velvet_interior.jpg" << std::endl;
	}

	if (!CreateGLTexture("Textures/floor_bricks.jpg", TEXTURE_TOWER_BASE))
	{
		std::cout << "Failed to load texture: floor_bricks.jpg" << std::endl;
	}

	if (!CreateGLTexture("Textures/cork_topper.jpg", TEXTURE_CORK))
	{
		std::cout << "Failed to load texture: cork_topper.jpg" << std::endl;
	}

	if (!CreateGLTexture("Textures/leather_cover.jpg", TEXTURE_BOOK))
	{
		std::cout << "Failed to load texture: leather_cover.jpg" << std::endl;
	}

	// Define object materials for Phong lighting
	// The tabletop has a slightly shinier finish so it reflects lighting more clearly
	OBJECT_MATERIAL floorMaterial;
	floorMaterial.ambientStrength = 0.25f;
	floorMaterial.ambientColor = glm::vec3(0.6f, 0.6f, 0.6f);
	floorMaterial.diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
	floorMaterial.specularColor = glm::vec3(0.08f, 0.08f, 0.08f);
	floorMaterial.shininess = 8.0f;
	floorMaterial.tag = MATERIAL_FLOOR;
	m_objectMaterials.push_back(floorMaterial);

	// The stone tower is less reflective than the ground plane
	OBJECT_MATERIAL stoneMaterial;
	stoneMaterial.ambientStrength = 0.25f;
	stoneMaterial.ambientColor = glm::vec3(0.6f, 0.6f, 0.6f);
	stoneMaterial.diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
	stoneMaterial.specularColor = glm::vec3(0.05f, 0.05f, 0.05f);
	stoneMaterial.shininess = 6.0f;
	stoneMaterial.tag = MATERIAL_STONE;
	m_objectMaterials.push_back(stoneMaterial);

	// The roof has a moderate highlight so texture detail still shows
	OBJECT_MATERIAL roofMaterial;
	roofMaterial.ambientStrength = 0.25f;
	roofMaterial.ambientColor = glm::vec3(0.6f, 0.6f, 0.6f);
	roofMaterial.diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
	roofMaterial.specularColor = glm::vec3(0.06f, 0.06f, 0.06f);
	roofMaterial.shininess = 6.0f;
	roofMaterial.tag = MATERIAL_ROOF;
	m_objectMaterials.push_back(roofMaterial);

	// The wooden tray has a soft highlight to keep it from looking too glossy
	OBJECT_MATERIAL woodMaterial;
	woodMaterial.ambientStrength = 0.25f;
	woodMaterial.ambientColor = glm::vec3(0.6f, 0.6f, 0.6f);
	woodMaterial.diffuseColor = glm::vec3(1.0f, 1.0f, 1.0f);
	woodMaterial.specularColor = glm::vec3(0.04f, 0.04f, 0.04f);
	woodMaterial.shininess = 5.0f;
	woodMaterial.tag = MATERIAL_WOOD;
	m_objectMaterials.push_back(woodMaterial);

	// Bind the loaded textures to OpenGL texture slots
	BindGLTextures();
}

/***********************************************************
 *  RenderScene()
 *
 *  This method is used for rendering the complete 3D scene
 *  by calling helper methods for each major object.
 ***********************************************************/
void SceneManager::RenderScene()
{
	// Enable lighting so the shader applies Phong lighting calculations
	if (NULL != m_pShaderManager)
	{
		m_pShaderManager->setIntValue("bUseLighting", true);

		for (int i = 0; i < NUM_SCENE_LIGHTS; i++)
		{
			m_pShaderManager->setVec3Value(
				SCENE_LIGHT_NAMES[i] + ".position",
				SCENE_LIGHT_POSITIONS[i]);

			m_pShaderManager->setVec3Value(
				SCENE_LIGHT_NAMES[i] + ".color",
				SCENE_LIGHT_COLORS[i]);
		}

		m_pShaderManager->setVec3Value("ambientLight", AMBIENT_LIGHT_COLOR);
	}

	// Render each part of the scene in a clean, organized order
	RenderGround();

	RenderRulebook();
	RenderScroll();
	RenderPotionBottle();

	RenderDieCube();
	RenderD4();
	RenderDiceTray();

	RenderTowerBody();
	RenderRoof();
	RenderTray();
}
/***********************************************************
 *  RenderGround()
 *
 *  This method renders the textured ground plane that
 *  supports the rest of the 3D scene.
 ***********************************************************/
void SceneManager::RenderGround()
{
	glm::vec3 scaleXYZ = FLOOR_SCALE;
	glm::vec3 positionXYZ = FLOOR_POSITION;

	// Scale and position the floor plane
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);

	// Apply the floor material so the plane reflects light more clearly
	SetShaderMaterial(MATERIAL_FLOOR);

	// Apply the cobblestone floor texture with tiling
	SetShaderTexture(TEXTURE_WOOD);
	SetTextureUVScale(FLOOR_UV_X, FLOOR_UV_Y);

	// Draw the plane mesh
	m_basicMeshes->DrawPlaneMesh();
}

/***********************************************************
 *  RenderTowerBody()
 *
 *  This method renders the main tower body using a textured
 *  cylinder mesh.
 ***********************************************************/
void SceneManager::RenderTowerBody()
{
	glm::vec3 scaleXYZ = TOWER_SCALE;
	glm::vec3 positionXYZ = TOWER_POSITION;

	// Scale and position the main stone tower body
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);

	// Apply the stone material so the tower responds to light
	SetShaderMaterial(MATERIAL_STONE);

	// Apply the stone texture with tiling 
	SetShaderTexture(TEXTURE_STONE);
	SetTextureUVScale(TOWER_UV_X, TOWER_UV_Y);

	// Draw the cylinder mesh
	m_basicMeshes->DrawCylinderMesh();
}

/***********************************************************
 *  RenderRoof()
 *
 *  This method renders the pointed roof using a textured
 *  cone mesh.
 ***********************************************************/
void SceneManager::RenderRoof()
{
	glm::vec3 scaleXYZ = ROOF_SCALE;
	glm::vec3 positionXYZ = ROOF_POSITION;

	// Scale and position the roof on top of the tower body
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);

	// Apply the roof material so it responds to the scene lighting
	SetShaderMaterial(MATERIAL_ROOF);

	// Apply the roof texture with tiling
	SetShaderTexture(TEXTURE_ROOF);
	SetTextureUVScale(ROOF_UV_X, ROOF_UV_Y);

	// Draw the cone mesh
	m_basicMeshes->DrawConeMesh();
}

/***********************************************************
 *  RenderTray()
 *
 *  This method renders the wooden tray attached to the
 *  front of the tower.
 ***********************************************************/
void SceneManager::RenderTray()
{
	glm::vec3 scaleXYZ = TRAY_SCALE;
	glm::vec3 positionXYZ = TRAY_POSITION;

	// Scale and position the tray at the base of the tower
	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);

	// Apply material and texture so the tray responds to light
	SetShaderMaterial(MATERIAL_WOOD);
	SetShaderTexture(TEXTURE_TOWER_BASE);
	SetTextureUVScale(1.0f, 1.0f);

	// Draw the box mesh
	m_basicMeshes->DrawBoxMesh();
}

/***********************************************************
 *  RenderRulebook()
 *
 *  This method renders the tabletop rulebook using a box mesh.
 ***********************************************************/
void SceneManager::RenderRulebook()
{
	// Scale to look like a thick book
	// Position it on the left side of the table
	glm::vec3 scaleXYZ = BOOK_SCALE;
	glm::vec3 positionXYZ = BOOK_POSITION;

	// Slight rotation to make it look natural
	SetTransformations(scaleXYZ, 0.0f, 18.0f, 0.0f, positionXYZ);

	// Apply leather texture
	SetShaderTexture(TEXTURE_BOOK);
	SetTextureUVScale(1.0f, 1.0f);

	// Draw the box mesh
	m_basicMeshes->DrawBoxMesh();
}

/***********************************************************
 *  RenderScroll()
 *
 *  This method renders the scroll map using a plane and cylinder.
 ***********************************************************/
void SceneManager::RenderScroll()
{
	// -------- FLAT PART OF SCROLL --------
	glm::vec3 scaleXYZ = SCROLL_SCALE;
	glm::vec3 positionXYZ = SCROLL_POSITION;

	SetTransformations(scaleXYZ, 0.0f, -15.0f, 0.0f, positionXYZ);

	SetShaderTexture(TEXTURE_SCROLL);
	SetTextureUVScale(1.0f, 1.0f);

	m_basicMeshes->DrawPlaneMesh();

	// -------- ROLLED PART OF SCROLL --------
	glm::vec3 rollScale = SCROLL_ROLL_SCALE;
	glm::vec3 rollPosition = SCROLL_ROLL_POSITION;

	// Rotate so cylinder lays sideways
	SetTransformations(rollScale, 0.0f, 75.0f, 90.0f, rollPosition);

	SetShaderTexture(TEXTURE_SCROLL);
	SetTextureUVScale(1.0f, 1.0f);

	m_basicMeshes->DrawCylinderMesh();
}

/***********************************************************
 *  RenderPotionBottle()
 *
 *  This method renders a potion bottle using cylinders.
 ***********************************************************/
void SceneManager::RenderPotionBottle()
{
	// -------- GLASS BOTTLE --------
	glm::vec3 scaleXYZ = POTION_SCALE;
	glm::vec3 positionXYZ = POTION_POSITION;

	SetTransformations(scaleXYZ, 0.0f, 0.0f, 0.0f, positionXYZ);

	// Slight red tint for health potion
	SetShaderColor(0.7f, 0.2f, 0.2f, 1.0f);

	m_basicMeshes->DrawCylinderMesh();

	// -------- CORK --------
	glm::vec3 corkScale = CORK_SCALE;
	glm::vec3 corkPosition = CORK_POSITION;

	SetTransformations(corkScale, 0.0f, 0.0f, 0.0f, corkPosition);

	// Cork texture added for lid
	SetShaderTexture(TEXTURE_CORK);
	SetTextureUVScale(1.0f, 1.0f);

	m_basicMeshes->DrawCylinderMesh();
}

/***********************************************************
 *  RenderDieCube()
 *
 *  This method renders a cube-shaped die using a box mesh.
 ***********************************************************/
void SceneManager::RenderDieCube()
{
	glm::vec3 scaleXYZ = CUBE_DIE_SCALE;
	glm::vec3 positionXYZ = CUBE_DIE_POSITION;

	SetTransformations(scaleXYZ, 0.0f, 25.0f, 0.0f, positionXYZ);

	// Cobalt blue dice color added
	SetShaderColor(0.0f, 0.28f, 0.67f, 1.0f);

	m_basicMeshes->DrawBoxMesh();
}

/***********************************************************
 *  RenderD4()
 *
 *  This method renders a four-sided die using a pyramid mesh.
 ***********************************************************/
void SceneManager::RenderD4()
{
	glm::vec3 scaleXYZ = D4_SCALE;
	glm::vec3 positionXYZ = D4_POSITION;

	SetTransformations(scaleXYZ, 0.0f, 35.0f, 0.0f, positionXYZ);

	// Neon green dice color added
	SetShaderColor(0.22f, 1.0f, 0.08f, 1.0f);

	m_basicMeshes->DrawPyramid3Mesh();
}

/***********************************************************
 *  RenderDiceTray()
 *
 *  This method renders a simple dice tray using box meshes.
 ***********************************************************/
void SceneManager::RenderDiceTray()
{
	// -------- TRAY BASE --------
	glm::vec3 baseScale = DICE_TRAY_BASE_SCALE;
	glm::vec3 basePosition = DICE_TRAY_BASE_POSITION;

	SetTransformations(baseScale, 0.0f, 0.0f, 0.0f, basePosition);
	SetShaderTexture(TEXTURE_DICE_TRAY);
	SetTextureUVScale(1.0f, 1.0f);
	m_basicMeshes->DrawBoxMesh();

	// -------- LEFT WALL --------
	glm::vec3 wallScale = DICE_TRAY_WALL_SCALE;
	glm::vec3 leftWallPosition = DICE_TRAY_LEFT_WALL_POSITION;

	SetTransformations(wallScale, 0.0f, 0.0f, 0.0f, leftWallPosition);
	SetShaderColor(0.05f, 0.05f, 0.05f, 1.0f);
	m_basicMeshes->DrawBoxMesh();

	// -------- RIGHT WALL --------
	glm::vec3 rightWallPosition = DICE_TRAY_RIGHT_WALL_POSITION;

	SetTransformations(wallScale, 0.0f, 0.0f, 0.0f, rightWallPosition);
	SetShaderColor(0.05f, 0.05f, 0.05f, 1.0f);
	m_basicMeshes->DrawBoxMesh();

	// -------- FRONT WALL --------
	glm::vec3 frontWallScale = DICE_TRAY_FRONTBACK_SCALE;
	glm::vec3 frontWallPosition = DICE_TRAY_FRONT_WALL_POSITION;

	SetTransformations(frontWallScale, 0.0f, 0.0f, 0.0f, frontWallPosition);
	SetShaderColor(0.05f, 0.05f, 0.05f, 1.0f);
	m_basicMeshes->DrawBoxMesh();

	// -------- BACK WALL --------
	glm::vec3 backWallPosition = DICE_TRAY_BACK_WALL_POSITION;

	SetTransformations(frontWallScale, 0.0f, 0.0f, 0.0f, backWallPosition);
	SetShaderColor(0.05f, 0.05f, 0.05f, 1.0f);
	m_basicMeshes->DrawBoxMesh();
}