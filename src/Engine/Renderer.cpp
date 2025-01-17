#include "Renderer.h"

#include <iostream>
#include <vector>
#include <cstdio>
#include "Vertex.h"
#include "Camera.h"
#include "Consts.h"
#include "Lights.h"

#define TEX_ARRAY_SLOT 0
#define BRIGHT_TEXTURE_SLOT 1 // slot to go into blur shader and final bloom shader
#define SCENE_TEXTURE_SLOT 2 // slot to go into final bloom shader
#define OBJECT_INFO_TEXTURE_BUFFER_SLOT 3
#define POINTLIGHT_TEXTURE_BUFFER_SLOT 4
#define DIRLIGHT_TEXTURE_BUFFER_SLOT 5
#define SPOTLIGHT_TEXTURE_BUFFER_SLOT 6

// quad filling entire screen
ViewportVertex viewportVertices[] = {
	{-1.0f, -1.0f, 0.0f, 0.0f, 0.0f},
	{1.0f, -1.0f, 0.0f, 1.0f, 0.0f},
	{1.0f, 1.0f, 0.0f, 1.0f, 1.0f},
	{1.0f, 1.0f, 0.0f, 1.0f, 1.0f},
	{-1.0f, 1.0f, 0.0f, 0.0f, 1.0f},
	{-1.0f, -1.0f, 0.0f, 0.0f, 0.0f}
};

void Renderer::drawAxis(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection) {
	const AxisVertex vertices[] = {
		// x
		{-100.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},
		{100.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f},

		// y
		{0.0f, -100.0f, 0.0f, 0.0f, 1.0f, 0.0f},
		{0.0f, 100.0f, 0.0f, 0.0f, 1.0f, 0.0f},

		// z
		{0.0f, 0.0f, -100.0f, 0.0f, 0.0f, 1.0f},
		{0.0f, 0.0f, 100.0f, 0.0f, 0.0f, 1.0f}
	};

	// bind VAO, VBO
	GLCall(glBindVertexArray(this->VAO_axis));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer_axis));

	// load vertices
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

	axisShader.use();
	// axisShader.setMat4("u_Model", model);
	// axisShader.setMat4("u_View", view);
	// axisShader.setMat4("u_Projection", projection);
	axisShader.setMat4("u_MVP", projection * view * model);

	GLCall(glDrawArrays(GL_LINES, 0, 6)); // 6 pontos, 3 linhas
}

void Renderer::drawCurves(const glm::mat4 &view, const glm::mat4 &projection,
                          const std::vector<Engine_Object_Curve>& points) {

    std::vector<AxisVertex> curvePoints = std::vector<AxisVertex>();
    std::vector<AxisVertex> normalPoints = std::vector<AxisVertex>();

    // bind VAO, VBO
    GLCall(glBindVertexArray(this->VAO_axis));
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer_axis));

    axisShader.use();
	constexpr glm::mat4 model = glm::mat4(1.0f);
    axisShader.setMat4("u_MVP", projection * view * model);

    // TODO Try drawing only one time
    for(int i = 0; i < points.size(); i++){
        // pair.first -> curve points
        // pair.second -> each 2 points form the normals of the curve

        Engine_Object_Curve pair = points[i];

        curvePoints.clear();
        normalPoints.clear();

        for(auto& p : pair.points){
            // Draw lines white
            glm::vec4 coords = p.coords;

            coords = pair.transform.transformMatrix * coords;

            curvePoints.emplace_back(coords.x, coords.y, coords.z, 1.0f, 1.0f, 1.0f);
        }

        GLuint size = curvePoints.size();

        // load vertices
        GLCall(glBufferData(GL_ARRAY_BUFFER, size * sizeof(AxisVertex), curvePoints.data(), GL_STATIC_DRAW));

        GLCall(glDrawArrays(GL_LINE_LOOP, 0, size));

        if (showCurveNormals) {
            // If there are no curve points, there won't be derivates to draw
            if(size == 0) continue;

            // cursed
            std::vector<Vertex>::const_iterator iter = pair.deriv.begin();

            for(int iter = 0; iter < pair.deriv.size() - 1; iter ++) {
                // Draw normals
                Vertex &v1 = pair.deriv[iter++];
                Vertex &v2 = pair.deriv[iter];

                v1.coords = pair.transform.transformMatrix * v1.coords;
                v2.coords = pair.transform.transformMatrix * v2.coords;

                normalPoints.emplace_back(v1.coords.x, v1.coords.y, v1.coords.z, 1.0f, 0.0f, 0.0f);
                const glm::vec3 new_coords = v1.coords + glm::normalize(v2.coords - v1.coords);
                normalPoints.emplace_back(new_coords.x, new_coords.y, new_coords.z, 0.0f, 0.0f, 1.0f);
            }

            GLuint size = normalPoints.size();

            // load vertices
            GLCall(glBufferData(GL_ARRAY_BUFFER, size * sizeof(AxisVertex), normalPoints.data(), GL_STATIC_DRAW));

            GLCall(glDrawArrays(GL_LINES, 0, size));
        }
    }
}

void Renderer::drawNormals(const glm::mat4 &view, const glm::mat4 &projection, const std::vector<Vertex> &vertices) {

	// VAO and VBO are the same as the normal ones, so I will not rebind them

	normalsShader.use();
	// normalsShader.setMat4("u_Model", model);
	normalsShader.setMat4("u_View", view);
	normalsShader.setMat4("u_Projection", projection);

	// only doing this to setup TBO is risky but should work
	normalsShader.setInt("u_ObjectInfoTBO", OBJECT_INFO_TEXTURE_BUFFER_SLOT);


	GLCall(glDrawArrays(GL_TRIANGLES, 0, vertices.size()));
}

Renderer::Renderer(GLsizei viewport_width, GLsizei viewport_height)
: viewport_width(viewport_width), viewport_height(viewport_height), VAO(0), vertexBuffer(0),
  lightingShader("shaders/lighting_extract_brightness_with_gs.vert", "shaders/lighting_extract_brightness_with_gs.frag", "shaders/explode.gs"),
  sunShader("shaders/sun.vert", "shaders/sun.frag", "shaders/sun.gs"),
  axisShader("shaders/basic.vert", "shaders/basic.frag"),
  normalsShader("shaders/normals.vert", "shaders/normals.frag", "shaders/normals.gs"),
  blurShader("shaders/blur.vert", "shaders/blur.frag"),
  hdrBbloomMergeShader("shaders/hdrBloomMerge.vert", "shaders/hdrBloomMerge.frag")
{
	//////////////////////////// LOADING VAO ////////////////////////////
	GLCall(glGenVertexArrays(1, &this->VAO));
	GLCall(glBindVertexArray(this->VAO));

	//////////////////////////// LOADING VBO ////////////////////////////////
	GLCall(glGenBuffers(1, &this->vertexBuffer));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer));
	// GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
	{
		GLuint vertex_position_layout = 0;
		GLCall(glEnableVertexAttribArray(vertex_position_layout));					// size appart				// offset
		GLCall(glVertexAttribPointer(vertex_position_layout, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, coords)));
		// GLCall(glVertexAttribDivisor(vertex_position_layout, 0)); // values are per vertex

		GLuint vertex_normal_layout = 1;
		GLCall(glEnableVertexAttribArray(vertex_normal_layout));					// size appart				// offset
		GLCall(glVertexAttribPointer(vertex_normal_layout, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, normal)));
		// GLCall(glVertexAttribDivisor(vertex_normal_layout, 0)); // values are per vertex

		GLuint vertex_texcoord_layout = 2;
		GLCall(glEnableVertexAttribArray(vertex_texcoord_layout));					// size appart				// offset
		GLCall(glVertexAttribPointer(vertex_texcoord_layout, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void *)offsetof(Vertex, tex_coord)));
		// GLCall(glVertexAttribDivisor(vertex_color_layout, 0)); // values are per vertex

		GLuint vertex_matid_layout = 3;
		GLCall(glEnableVertexAttribArray(vertex_matid_layout));					// size appart				// offset
		GLCall(glVertexAttribIPointer(vertex_matid_layout, 1, GL_INT, sizeof(Vertex), (const void *)offsetof(Vertex, object_id)));
		// GLCall(glVertexAttribDivisor(vertex_color_layout, 0)); // values are per triangle, but I am not using instancing
	}

	//////////////////////////// LOADING VAO  FOR SUN ////////////////////////////
	GLCall(glGenVertexArrays(1, &this->VAO_sun));
	GLCall(glBindVertexArray(this->VAO_sun));

	//////////////////////////// LOADING VBO ////////////////////////////////
	GLCall(glGenBuffers(1, &this->vertexBuffer_sun));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer_sun));
	// GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
	{
		GLuint vertex_position_layout = 0;
		GLCall(glEnableVertexAttribArray(vertex_position_layout));					// size appart				// offset
		GLCall(glVertexAttribPointer(vertex_position_layout, 4, GL_FLOAT, GL_FALSE, sizeof(SunVertex), (const void *)offsetof(SunVertex, coords)));
		// GLCall(glVertexAttribDivisor(vertex_position_layout, 0)); // values are per vertex

		GLuint vertex_texcoord_layout = 1;
		GLCall(glEnableVertexAttribArray(vertex_texcoord_layout));					// size appart				// offset
		GLCall(glVertexAttribPointer(vertex_texcoord_layout, 2, GL_FLOAT, GL_FALSE, sizeof(SunVertex), (const void *)offsetof(SunVertex, tex_coord)));
		// GLCall(glVertexAttribDivisor(vertex_color_layout, 0)); // values are per vertex
	}

	//////////////////////////// LOADING VAO FOR AXIS ////////////////////////////
	GLCall(glGenVertexArrays(1, &this->VAO_axis));
	GLCall(glBindVertexArray(this->VAO_axis));

	//////////////////////////// LOADING VBO FOR AXIS ////////////////////////////////
	GLCall(glGenBuffers(1, &this->vertexBuffer_axis));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer_axis));
	// GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
	{
		GLuint vertex_position_layout = 0;
		GLCall(glEnableVertexAttribArray(vertex_position_layout));					// size appart				// offset
		GLCall(glVertexAttribPointer(vertex_position_layout, 4, GL_FLOAT, GL_FALSE, sizeof(AxisVertex), (const void *)offsetof(AxisVertex, coords)));
		// GLCall(glVertexAttribDivisor(vertex_position_layout, 0)); // values are per vertex

		GLuint vertex_color_layout = 1;
		GLCall(glEnableVertexAttribArray(vertex_color_layout));					// size appart				// offset
		GLCall(glVertexAttribPointer(vertex_color_layout, 3, GL_FLOAT, GL_FALSE, sizeof(AxisVertex), (const void *)offsetof(AxisVertex, color)));
		// GLCall(glVertexAttribDivisor(vertex_color_layout, 0)); // values are per vertex
	}

	//////////////////////////// LOADING VAO FOR HDR ////////////////////////////
	GLCall(glGenVertexArrays(1, &this->VAO_viewport));
	GLCall(glBindVertexArray(this->VAO_viewport));

	//////////////////////////// LOADING VBO FOR HDR ////////////////////////////
	GLCall(glGenBuffers(1, &this->vertexBuffer_viewport));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer_viewport));
	// GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
	{
		GLuint vertex_position_layout = 0;
		GLCall(glEnableVertexAttribArray(vertex_position_layout));					// size appart				// offset
		GLCall(glVertexAttribPointer(vertex_position_layout, 4, GL_FLOAT, GL_FALSE, sizeof(ViewportVertex), (const void *)offsetof(ViewportVertex, coords)));
		// GLCall(glVertexAttribDivisor(vertex_position_layout, 0)); // values are per vertex

		GLuint vertex_texcoord_layout = 1;
		GLCall(glEnableVertexAttribArray(vertex_texcoord_layout));					// size appart				// offset
		GLCall(glVertexAttribPointer(vertex_texcoord_layout, 2, GL_FLOAT, GL_FALSE, sizeof(ViewportVertex), (const void *)offsetof(ViewportVertex, tex_coord)));
		// GLCall(glVertexAttribDivisor(vertex_normal_layout, 0)); // values are per vertex
	}

	//////////////////////////// LOADING SHADER UNIFORMS ///////////////////////////
	lightingShader.use();
	lightingShader.setInt("u_TextureArraySlot", TEX_ARRAY_SLOT);
	lightingShader.setMat4("u_View", glm::mat4(1.0f)); // load identity just for safety
	lightingShader.setMat4("u_Projection", glm::mat4(1.0f)); // load identity just for safety


	GLCall(glGenBuffers(1, &objectInfoBuffer));
	GLCall(glBindBuffer(GL_TEXTURE_BUFFER, objectInfoBuffer));
	GLCall(glGenTextures(1, &objectInfoTBO));
	GLCall(glBindTexture(GL_TEXTURE_BUFFER, objectInfoTBO));
	GLCall(glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, objectInfoBuffer)); // bind the buffer to the texture

	GLCall(glGenBuffers(1, &pointLightBuffer));
	GLCall(glBindBuffer(GL_TEXTURE_BUFFER, pointLightBuffer));
	GLCall(glGenTextures(1, &pointLightTBO));
	GLCall(glBindTexture(GL_TEXTURE_BUFFER, pointLightTBO));
	GLCall(glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, pointLightBuffer)); // bind the buffer to the texture

	GLCall(glGenBuffers(1, &dirLightBuffer));
	GLCall(glBindBuffer(GL_TEXTURE_BUFFER, dirLightBuffer));
	GLCall(glGenTextures(1, &dirLightTBO));
	GLCall(glBindTexture(GL_TEXTURE_BUFFER, dirLightTBO));
	GLCall(glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, dirLightBuffer)); // bind the buffer to the texture

	GLCall(glGenBuffers(1, &spotLightBuffer));
	GLCall(glBindBuffer(GL_TEXTURE_BUFFER, spotLightBuffer));
	GLCall(glGenTextures(1, &spotLightTBO));
	GLCall(glBindTexture(GL_TEXTURE_BUFFER, spotLightTBO));
	GLCall(glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, spotLightBuffer)); // bind the buffer to the texture


	// for axis shader
	axisShader.use();
	// axisShader.setMat4("u_Model", glm::mat4(1.0f)); // load identity just for safety
	// axisShader.setMat4("u_View", glm::mat4(1.0f)); // load identity just for safety
	// axisShader.setMat4("u_Projection", glm::mat4(1.0f)); // load identity just for safety
	axisShader.setMat4("u_MVP", glm::mat4(1.0f));

	//////////////////////////// TEXTURE ARRAY ////////////////////////////
	this->textureArray = std::make_unique<TextureArray>(TEXTURE_WIDTH, TEXTURE_HEIGHT, TEXTURE_LAYERS);
	// textureArray.get()->addTexture("textures/grass.png");
	// this->textureArray.get()->setTextureArrayToSlot(TEX_ARRAY_SLOT);

	//////////////////////////// LOADING FRAMEBUFFERS AND TEXTURE ATTACHMENTS ////////////////////////////
	GLCall(glGenFramebuffers(1, &lightingFBO));
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, lightingFBO));
		generate_FBO_depth_buffer(&lightingFBODepthBuffer);
		generate_FBO_texture(&lightingTexture, GL_COLOR_ATTACHMENT0);
		generate_FBO_texture(&brightTexture, GL_COLOR_ATTACHMENT1);
		GLCall(checkFrameBuffer());
	GLCall(glGenFramebuffers(2, pingpongFBO));
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[0]));
		generate_FBO_texture(&pingpongTextures[0], GL_COLOR_ATTACHMENT0);
		GLCall(checkFrameBuffer());
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[1]));
		generate_FBO_texture(&pingpongTextures[1], GL_COLOR_ATTACHMENT0);
		GLCall(checkFrameBuffer());
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));



	//////////////////////////// CLEANUP ///////////////////////////
	GLCall(glUseProgram(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	GLCall(glBindVertexArray(0));
}

Renderer::~Renderer() {
	GLCall(glDeleteBuffers(1, &vertexBuffer));
	GLCall(glDeleteBuffers(1, &objectInfoBuffer));
	GLCall(glDeleteBuffers(1, &vertexBuffer_axis));
	GLCall(glDeleteBuffers(1, &vertexBuffer_viewport));

	GLCall(glDeleteVertexArrays(1, &VAO));
	GLCall(glDeleteVertexArrays(1, &VAO_axis));
	GLCall(glDeleteVertexArrays(1, &VAO_viewport));

	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	GLCall(glDeleteTextures(1, &lightingTexture));
	GLCall(glDeleteTextures(2, pingpongTextures));

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, lightingFBO));
	GLCall(glDeleteRenderbuffers(1, &lightingFBODepthBuffer));

	GLCall(glDeleteFramebuffers(1, &lightingFBO));
	GLCall(glDeleteFramebuffers(2, pingpongFBO));

	// delete TBOs??????????
}

// could be optimized, whatever
GLfloat Renderer::getTextureID(const std::string &name) {

	// roto mas caguei ns se ir pela exception era melhor
	if (texture_id_map.find(name) == texture_id_map.end()) {
		// not found, load
		texture_id_map[name] = textureArray.get()->addTexture(("textures/" + name).c_str()); // cursed 
	}
	return texture_id_map[name];

	// TextureArray *tex = textureArray.get();
	// tex->addTexture("textures/missing_texture.png"); // 0
	// tex->addTexture("textures/black.png"); // 1
	// tex->addTexture("textures/grass.png"); // 2
	// tex->addTexture("textures/oak_log.png"); // 3
	// tex->addTexture("textures/white.png"); // 4
	// tex->addTexture("test_files_phase_4/earth.jpg"); // 5
	// tex->addTexture("textures/rainbow.png"); // 6
	// tex->addTexture("textures/stripes.png"); // 7
    // tex->addTexture("test_files_phase_4/cone.jpg"); // 8
    // tex->addTexture("test_files_phase_4/teapot.jpg"); // 9
}

void Renderer::prepareFrame(Camera &camera, GLfloat deltaTime, GLfloat physicsDeltaTime, GLfloat physicsProcessingDeltaTime) {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Debug");
	// ImGui::ShowDemoWindow();
	ImGui::Text("FPS: %lf", 1.0f / deltaTime);
	ImGui::Text("Physics FPS: %lf", 1.0f / physicsDeltaTime);
	ImGui::Text("Physics processing FPS: %lf", 1.0f / physicsProcessingDeltaTime);
	ImGui::Text("Facing x:%f y:%f z:%f", camera.Front.x, camera.Front.y, camera.Front.z);
	ImGui::InputFloat3("Position", glm::value_ptr(camera.Position));
	ImGui::SliderFloat("##Camera_speed", &camera.MovementSpeed, 0.0f, 1000.0f, "Camera speed = %.3f");
	ImGui::SameLine();
	ImGui::InputFloat("Camera speed", &camera.MovementSpeed, 1.0f, 10.0f);
	ImGui::SliderFloat("gamma", &gamma, 0.0f, 10.0f, "gamma = %.3f");
	ImGui::SliderFloat("exposure", &exposure, 0.0f, 10.0f, "exposure = %.3f");
	ImGui::InputInt("bloomPasses", &bloomBlurPasses, 1, 1); if (bloomBlurPasses < 0) bloomBlurPasses = 0;
	// ImGui::InputInt("bloomPasses", &bloomBlurPasses, 1, 1, "bloomPasses = %d");
	ImGui::SliderFloat("bloomThreshold", &bloomThreshold, 0.0f, 5.0f);
	// texOffsetCoeff = static_cast<GLfloat>(rand()) / static_cast<GLfloat>(RAND_MAX) * 10.0f;
	ImGui::SliderFloat("bloomOffset", &bloomOffset, 0.0f, 10.0f, "bloomOffset = %.3f");
	ImGui::Checkbox("Show axis", &showAxis);
	ImGui::Checkbox("Show normals", &showNormals);
	ImGui::Checkbox("Show curves", &showCurves);
	ImGui::Checkbox("Show curve normals", &showCurveNormals);
	ImGui::Checkbox("Explode", &explode);
	if (explode) {
		ImGui::SliderFloat("explodeCoeff", &explodeCoeff, 0.0f, 10.0f, "explodeCoeff = %.3f");
	}
	ImGui::SliderFloat("##Engine speed", &engine_speed, 0.0f, 100.0f, "engine speed = %.3f");
	ImGui::SameLine();
	ImGui::InputFloat("Engine speed", &engine_speed, 1.0f, 100.0f);
	ImGui::Checkbox("Wireframe", &wireframe);
}

void Renderer::drawLighting(const std::vector<Vertex> &verts, const std::vector<RendererObjectInfo> &objectInfo, const glm::mat4 &projection, const glm::mat4 &view, const Camera &camera, const SceneLights &lights) {
	// constexpr glm::mat4 model = glm::mat4(1.0f);
	// const glm::mat4 MVP = projection * view * model;
	if (wireframe) {
		GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
	}

	//////////////////////////////////////////////// the normal scene is drawn into the lighting framebuffer, where the bright colors are then separated
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, lightingFBO));
    	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		GLCall(glBindVertexArray(this->VAO));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer));

		// load vertices
		GLCall(glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), verts.data(), GL_STATIC_DRAW));

		// bind program, load uniforms
		lightingShader.use();

		// load MVP, texture array and view
		this->textureArray.get()->setTextureArrayToSlot(TEX_ARRAY_SLOT);
		lightingShader.setInt("u_TextureArraySlot", TEX_ARRAY_SLOT);
		lightingShader.setMat4("u_View", view);
		lightingShader.setMat4("u_Projection", projection);

		lightingShader.setFloat("u_BloomThreshold", bloomThreshold);

		GLCall(glBindBuffer(GL_TEXTURE_BUFFER, objectInfoBuffer));
		GLCall(glBufferData(GL_TEXTURE_BUFFER, sizeof(RendererObjectInfo) * objectInfo.size(), objectInfo.data(), GL_STATIC_DRAW));
		GLCall(glActiveTexture(GL_TEXTURE0 + OBJECT_INFO_TEXTURE_BUFFER_SLOT));
		GLCall(glBindTexture(GL_TEXTURE_BUFFER, objectInfoTBO));
		// GLCall(glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, materialBuffer)); // bind the buffer to the texture (has been done while setting up)
		lightingShader.setInt("u_ObjectInfoTBO", OBJECT_INFO_TEXTURE_BUFFER_SLOT);

		GLCall(glBindBuffer(GL_TEXTURE_BUFFER, pointLightBuffer));
		GLCall(glBufferData(GL_TEXTURE_BUFFER, lights.pointLights.size() * sizeof(PointLight), lights.pointLights.data(), GL_STATIC_DRAW));
		GLCall(glActiveTexture(GL_TEXTURE0 + POINTLIGHT_TEXTURE_BUFFER_SLOT));
		GLCall(glBindTexture(GL_TEXTURE_BUFFER, pointLightTBO));
		// GLCall(glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, pointLightBuffer)); // bind the buffer to the texture (has been done while setting up)
		lightingShader.setInt("u_PointLightTBO", POINTLIGHT_TEXTURE_BUFFER_SLOT);
		lightingShader.setInt("u_NumPointLights", lights.pointLights.size());

		GLCall(glBindBuffer(GL_TEXTURE_BUFFER, dirLightBuffer));
		GLCall(glBufferData(GL_TEXTURE_BUFFER, lights.dirLights.size() * sizeof(DirLight), lights.dirLights.data(), GL_STATIC_DRAW));
		GLCall(glActiveTexture(GL_TEXTURE0 + DIRLIGHT_TEXTURE_BUFFER_SLOT));
		GLCall(glBindTexture(GL_TEXTURE_BUFFER, dirLightTBO));
		// GLCall(glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, pointLightBuffer)); // bind the buffer to the texture (has been done while setting up)
		lightingShader.setInt("u_DirLightTBO", DIRLIGHT_TEXTURE_BUFFER_SLOT);
		lightingShader.setInt("u_NumDirLights", lights.dirLights.size());


		GLCall(glBindBuffer(GL_TEXTURE_BUFFER, spotLightBuffer));
		GLCall(glBufferData(GL_TEXTURE_BUFFER, lights.spotLights.size() * sizeof(SpotLight), lights.spotLights.data(), GL_STATIC_DRAW));
		GLCall(glActiveTexture(GL_TEXTURE0 + SPOTLIGHT_TEXTURE_BUFFER_SLOT));
		GLCall(glBindTexture(GL_TEXTURE_BUFFER, spotLightTBO));
		// GLCall(glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, spotLightBuffer)); // bind the buffer to the texture (has been done while setting up)
		lightingShader.setInt("u_SpotLightTBO", SPOTLIGHT_TEXTURE_BUFFER_SLOT);
		lightingShader.setInt("u_NumSpotLights", lights.spotLights.size());

		// bind the render buffer to this FBO (maybe this is missing actualy binding it, idk, but it gets regenerated automatically when screen is resized)
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, lightingFBODepthBuffer);


		// specify 2 attachments
		constexpr GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		GLCall(glDrawBuffers(2, attachments));

		if (explode) {
			lightingShader.setFloat("u_ExplodeCoeff", explodeCoeff);
		} else {
			if (explodeCoeff > 0.0f) {
				lightingShader.setFloat("u_ExplodeCoeff", 0.0f);
			}
		}

		// lightingShader.validate();
		GLCall(glDrawArrays(GL_TRIANGLES, 0, verts.size()));
	
		if (showNormals) {
			drawNormals(view, projection, verts);
		}

		if (showAxis) {
			drawAxis(glm::mat4(1.0f), view, projection);
		}

		if (wireframe) {
			GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));
		}
}

void Renderer::drawSun(const std::vector<SunVertex> &verts, const RendererObjectInfo &sunInfo, const glm::mat4 &projection, const glm::mat4 &view, GLfloat totalTime) {
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, lightingFBO));
		GLCall(glBindVertexArray(this->VAO_sun));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer_sun));

		// load vertices
		GLCall(glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(Vertex), verts.data(), GL_STATIC_DRAW));

		// bind program, load uniforms
		sunShader.use();


		this->textureArray.get()->setTextureArrayToSlot(TEX_ARRAY_SLOT);
		sunShader.setInt("u_TextureArraySlot", TEX_ARRAY_SLOT);
		sunShader.setMat4("u_View", view);
		sunShader.setMat4("u_Projection", projection);

		sunShader.setFloat("u_BloomThreshold", bloomThreshold);

		GLCall(glBindBuffer(GL_TEXTURE_BUFFER, objectInfoBuffer));
		GLCall(glBufferData(GL_TEXTURE_BUFFER, sizeof(RendererObjectInfo) * 1, &sunInfo, GL_STATIC_DRAW));
		GLCall(glActiveTexture(GL_TEXTURE0 + OBJECT_INFO_TEXTURE_BUFFER_SLOT));
		GLCall(glBindTexture(GL_TEXTURE_BUFFER, objectInfoTBO));
		// GLCall(glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, materialBuffer)); // bind the buffer to the texture (has been done while setting up)
		sunShader.setInt("u_ObjectInfoTBO", OBJECT_INFO_TEXTURE_BUFFER_SLOT);

		sunShader.setFloat("u_Time", totalTime);


		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, lightingFBODepthBuffer);

		constexpr GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
		GLCall(glDrawBuffers(2, attachments));

		if (explode) {
			sunShader.setFloat("u_ExplodeCoeff", explodeCoeff);
		} else {
			if (explodeCoeff > 0.0f) {
				lightingShader.setFloat("u_ExplodeCoeff", 0.0f);
			}
		}

		GLCall(glDrawArrays(GL_TRIANGLES, 0, verts.size()));
}

void Renderer::bloomBlur(int passes) {
	//////////////////////////////////////////////// run the ping pong gaussian blur several times
	blurShader.use();
	GLCall(glBindVertexArray(this->VAO_viewport));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer_viewport));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(viewportVertices), viewportVertices, GL_STATIC_DRAW));

	if (passes == 0) {
		// if this happens, instead of switching verything just clear pingpongTextures[1], which will be used in merging the textures
		// bind FBO, attach texture, clear color buffer
		// the tone mapping shader expects the result from blur to be placed in pingpongTextures[1]
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[1]));
		GLCall(glActiveTexture(GL_TEXTURE0 + BRIGHT_TEXTURE_SLOT));
		GLCall(glActiveTexture(GL_TEXTURE0 + BRIGHT_TEXTURE_SLOT));
		GLCall(glBindTexture(GL_TEXTURE_2D, pingpongTextures[1])); // use texture from pong

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		return;
	}


	// manually doing the first passes, since I need to get the texture from the scene
	// horizontal pass
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[0]));
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // isto devia ser chamado sequer???????????????????????????????????? acho que a imagem e 100% overwritten
		blurShader.setInt("u_Horizontal", 0);
		GLCall(glActiveTexture(GL_TEXTURE0 + BRIGHT_TEXTURE_SLOT));
		GLCall(glBindTexture(GL_TEXTURE_2D, this->brightTexture)); // use texture from scene
		blurShader.setInt("u_BlurBuffer", BRIGHT_TEXTURE_SLOT);
		GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));

	// vertical pass
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[1]));
    	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		blurShader.setInt("u_Horizontal", 1);
		GLCall(glActiveTexture(GL_TEXTURE0 + BRIGHT_TEXTURE_SLOT));
		GLCall(glBindTexture(GL_TEXTURE_2D, pingpongTextures[0])); // use texture from ping
		blurShader.setInt("u_BlurBuffer", BRIGHT_TEXTURE_SLOT);
		GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));

	for (GLint i = 0; i < passes - 1; i++) {
		blurShader.setFloat("u_TexOffsetCoeff", bloomOffset);

		// horizontal pass
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[0]));
			blurShader.setInt("u_Horizontal", 0);
			GLCall(glActiveTexture(GL_TEXTURE0 + BRIGHT_TEXTURE_SLOT));
			GLCall(glBindTexture(GL_TEXTURE_2D, pingpongTextures[1])); // use texture from pong
			blurShader.setInt("u_BlurBuffer", BRIGHT_TEXTURE_SLOT);
			GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));

		// vertical pass
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[1]));
			blurShader.setInt("u_Horizontal", 1);
			GLCall(glActiveTexture(GL_TEXTURE0 + BRIGHT_TEXTURE_SLOT));
			GLCall(glBindTexture(GL_TEXTURE_2D, pingpongTextures[0])); // use texture from ping
			blurShader.setInt("u_BlurBuffer", BRIGHT_TEXTURE_SLOT);
			GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
	}
}

void Renderer::merge() {
	//////////////////////////////////////////////// join the blur to the scene and apply gamma and exposure
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// GLCall(glBindVertexArray(this->VAO_viewport));
		// GLCall(glBindBuffer(GL_ARRAY_BUFFER, this->vertexBuffer_viewport));
		// GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(viewportVertices), viewportVertices, GL_STATIC_DRAW));

		hdrBbloomMergeShader.use();
		GLCall(glActiveTexture(GL_TEXTURE0 + SCENE_TEXTURE_SLOT));
		GLCall(glBindTexture(GL_TEXTURE_2D, this->lightingTexture));
		GLCall(glActiveTexture(GL_TEXTURE0 + BRIGHT_TEXTURE_SLOT));
		GLCall(glBindTexture(GL_TEXTURE_2D, pingpongTextures[1]));

		hdrBbloomMergeShader.setInt("u_SceneBuffer", SCENE_TEXTURE_SLOT);
		hdrBbloomMergeShader.setInt("u_BrightBuffer", BRIGHT_TEXTURE_SLOT);
		hdrBbloomMergeShader.setFloat("u_Gamma", gamma);
		hdrBbloomMergeShader.setFloat("u_Exposure", exposure);

		// hdrBbloomMergeShader.validate();
		GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
}

void Renderer::endFrame(GLFWwindow * window) {
	ImGui::End();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    glfwSwapBuffers(window);
}

std::vector<RendererObjectInfo> Renderer::translateEngineObjectInfo(const std::vector<Engine_Object_Info> &engineObjectInfo) {
	std::vector<RendererObjectInfo> objectInfo;
	for (const Engine_Object_Info & engineObjInfo: engineObjectInfo) {
		objectInfo.emplace_back(getTextureID(engineObjInfo.texture), engineObjInfo);
	}
	return objectInfo;
}

void Renderer::draw(const std::vector<Engine_Object_Curve>& curvePoints, const std::vector<Vertex> &verts, const std::vector<RendererObjectInfo> &objectInfo,
		// const std::vector<SunVertex> &sun_verts, const RendererObjectInfo &sunInfo, GLfloat totalTime,
		const glm::mat4 &projection, Camera &camera, GLFWwindow * window, GLfloat deltaTime, GLfloat physicsDeltaTime, GLfloat physicsProcessingDeltaTime,
		const SceneLights &lights) {

	prepareFrame(camera, deltaTime, physicsDeltaTime, physicsProcessingDeltaTime);
	const glm::mat4 view = camera.GetViewMatrix();
	drawLighting(verts, objectInfo, projection, view, camera, lights);
	// drawSun(sun_verts, sunInfo, projection, view, totalTime);

	if (showCurves) {
    	drawCurves(view, projection, curvePoints);
	}
	bloomBlur(this->bloomBlurPasses);
	merge();

	// pq e que isto ta feito aqui?????????????
	ImGui::Checkbox("Limit FPS", &limitFPS);
	if (limitFPS) {
		const double f64_min = 0.0, f64_max = 240.0;
		ImGui::SliderScalar("Target FPS", ImGuiDataType_Double, &fps, &f64_min, &f64_max, "Target FPS = %.0f");
	}


	endFrame(window);
}

// make sure fbo is bound before calling this
void Renderer::generate_FBO_depth_buffer(GLuint *depthBuffer) const {

	if (*depthBuffer > 0) {
		glDeleteRenderbuffers(1, depthBuffer);
	}

	glGenRenderbuffers(1, depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, *depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, this->viewport_width, this->viewport_height);
}

// this does NOT take into acount currently used textures slots etc, only here for organisation
// make sure fbo is bound before calling this
void Renderer::generate_FBO_texture(GLuint *textureID, GLenum attachmentID) {
	// delete existing texture, if needed
	if (*textureID != 0) { // for safety, delete the texture entirely. maybe does not need to be done
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));
		GLCall(glDeleteTextures(1, textureID));
	}

	GLCall(glGenTextures(1, textureID));
	GLCall(glBindTexture(GL_TEXTURE_2D, *textureID));
	// you can get the default behaviour by either not using the framebuffer or setting this to GL_RGBA
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, this->viewport_width, this->viewport_height, 0, GL_RGBA, GL_FLOAT, NULL));  // change to 32float if needed

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	// attach to fbo
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, attachmentID, GL_TEXTURE_2D, *textureID, 0));
}

// regenerate the textures for all the FBOs
void Renderer::resizeViewport(GLsizei viewport_width, GLsizei viewport_height) {
	this->viewport_width = viewport_width;
	this->viewport_height = viewport_height;

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, lightingFBO));
	generate_FBO_depth_buffer(&lightingFBODepthBuffer);
	generate_FBO_texture(&lightingTexture, GL_COLOR_ATTACHMENT0);
	generate_FBO_texture(&brightTexture, GL_COLOR_ATTACHMENT1);
	GLCall(checkFrameBuffer());

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[0]));
	generate_FBO_texture(&pingpongTextures[0], GL_COLOR_ATTACHMENT0);
	GLCall(checkFrameBuffer());
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, pingpongFBO[1]));
	generate_FBO_texture(&pingpongTextures[1], GL_COLOR_ATTACHMENT0);
	GLCall(checkFrameBuffer());

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

// needs to be improved
void Renderer::checkFrameBuffer() {
	GLCall(GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER));

	if (status != GL_FRAMEBUFFER_COMPLETE) {
    	puts("Error in fbo");
		exit(1);
	}
}
