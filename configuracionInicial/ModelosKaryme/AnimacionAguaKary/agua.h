#pragma once
#include <glad/glad.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <shader_m.h>

/*
	AguaAnimada: malla plana subdividida (para que las olas se vean bien)
	+ draw() que manda todos los uniforms al shader agua.vs / agua.fs
	+ moverConTeclas() para acomodar el estanque mientras corre el programa
*/
class AguaAnimada
{
public:
	unsigned int VAO = 0, VBO = 0, EBO = 0;
	unsigned int indexCount = 0;

	// ---- Parámetros que puedes cambiar ----
	float amplitud = 0.5f;		// altura de las olas
	float frecuencia = 0.6f;	// qué tan juntas están las olas
	float velocidad = 2.0f;		// qué tan rápido se mueven
	glm::vec3 colorAgua = glm::vec3(0.05f, 0.35f, 0.55f);
	float transparencia = 0.85f;	// 1.0 = opaca
	glm::vec3 luzDir = glm::vec3(-0.5f, -0.7f, -0.5f);
	glm::vec3 luzColor = glm::vec3(1.0f, 0.97f, 0.9f);

	// Textura opcional (0 = sin textura). Debe estar cargada con GL_REPEAT.
	unsigned int textura = 0;
	float tiling = 6.0f;
	glm::vec2 scrollTextura = glm::vec2(0.03f, 0.015f);

	// size = largo/ancho total del estanque, divisiones = celdas por lado
	void init(float size = 24.0f, int divisiones = 120)
	{
		std::vector<float> v;
		std::vector<unsigned int> idx;
		float mitad = size * 0.5f;

		for (int z = 0; z <= divisiones; z++)
		{
			for (int x = 0; x <= divisiones; x++)
			{
				float u = (float)x / divisiones;
				float w = (float)z / divisiones;
				// posición
				v.push_back(-mitad + size * u);
				v.push_back(0.0f);
				v.push_back(-mitad + size * w);
				// normal (hacia arriba)
				v.push_back(0.0f);
				v.push_back(1.0f);
				v.push_back(0.0f);
				// coordenadas de textura
				v.push_back(u);
				v.push_back(w);
			}
		}

		for (int z = 0; z < divisiones; z++)
		{
			for (int x = 0; x < divisiones; x++)
			{
				unsigned int i0 = z * (divisiones + 1) + x;
				unsigned int i1 = i0 + 1;
				unsigned int i2 = i0 + (divisiones + 1);
				unsigned int i3 = i2 + 1;
				idx.push_back(i0); idx.push_back(i2); idx.push_back(i1);
				idx.push_back(i1); idx.push_back(i2); idx.push_back(i3);
			}
		}
		indexCount = (unsigned int)idx.size();

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(float), v.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(unsigned int), idx.data(), GL_STATIC_DRAW);

		// layout igual al de los Model: 0 = posición, 1 = normal, 2 = texcoords
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);
	}

	// Llamar una vez por frame, de preferencia al final (usa transparencia)
	void draw(Shader& shader, const glm::mat4& model, const glm::mat4& view,
		const glm::mat4& projection, const glm::vec3& viewPos, float tiempo)
	{
		shader.use();
		shader.setMat4("model", model);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		shader.setVec3("viewPos", viewPos);

		shader.setFloat("uTime", tiempo);
		shader.setFloat("waveAmplitude", amplitud);
		shader.setFloat("waveFrequency", frecuencia);
		shader.setFloat("waveSpeed", velocidad);

		shader.setVec3("lightDir", luzDir);
		shader.setVec3("lightColor", luzColor);
		shader.setVec3("colorAgua", colorAgua);
		shader.setFloat("alpha", transparencia);

		shader.setVec2("waterSpeed", scrollTextura);
		shader.setFloat("tiling", tiling);
		shader.setInt("useTexture", textura != 0 ? 1 : 0);
		if (textura != 0)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, textura);
			shader.setInt("texture_diffuse1", 0);
		}

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glDisable(GL_BLEND);
	}

	// Flechas = mover en X/Z,  RePág / AvPág = subir / bajar.  Imprime la posición en consola.
	// Llamar desde my_input (el callback de teclado).
	bool moverConTeclas(int key, int action, glm::vec3& pos)
	{
		if (action == GLFW_RELEASE) return false;

		if (key == GLFW_KEY_UP)             pos.z -= 1.0f;
		else if (key == GLFW_KEY_DOWN)      pos.z += 1.0f;
		else if (key == GLFW_KEY_LEFT)      pos.x -= 1.0f;
		else if (key == GLFW_KEY_RIGHT)     pos.x += 1.0f;
		else if (key == GLFW_KEY_PAGE_UP)   pos.y += 0.25f;
		else if (key == GLFW_KEY_PAGE_DOWN) pos.y -= 0.25f;
		else return false;

		std::cout << "posAgua = (" << pos.x << ", " << pos.y << ", " << pos.z << ")" << std::endl;
		return true;
	}
};
