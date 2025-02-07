#ifndef cylinders_h
#define cylinders_h

#include "shader.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

class Cylinders {

public:
    int numPoints = 10;
    float radius = 1.0; // Change this if you want a different radius

    std::vector<std::pair<double, float>> points;

    float* arr = new float[numPoints*12];

    int* indices = new int[numPoints*6];
    void generateVertices() {
        for (int i = 0; i < numPoints; ++i) {
            float theta = i * (360.0 / numPoints); // Calculate theta in degrees
            float radians = theta * (3.1416 / 180.0); // Convert to radians

            // Calculate the x and y coordinates of the point
            float x = radius * cos(radians);
            float y = radius * sin(radians);

            points.push_back(std::make_pair(x, y));
        }
        
        int j = 0;
        // Print the generated points
        for (int i = 0; i < numPoints; ++i) {
            //std::cout << points[i].first << "f" << ", " << "1.0f" << ", " << points[i].second << "f" << " , 1.0f, 0.0f, 0.0f" << std::endl;
            arr[j++] = points[i].first;
            arr[j++] = 1.0;
            arr[j++] = points[i].second;
            arr[j++] = 1.0;
            arr[j++] = 0.0;
            arr[j++] = 0.0;

            //std::cout << points[i].first<<"f" << ", " << "-1.0f" << ", " << points[i].second<<"f"<< "1.0f, 0.0f, 0.0f"<< std::endl;
        }

        for (int i = 0; i < numPoints; ++i) {
            //std::cout << points[i].first<<"f" << ", " << "1.0f"<< ", " << points[i].second<<"f" << "1.0f, 0.0f, 0.0f"<< std::endl;
            //std::cout << points[i].first << "f" << ", " << "-1.0f" << ", " << points[i].second << "f" << " , 1.0f, 0.0f, 0.0f" << std::endl;
            arr[j++] = points[i].first;
            arr[j++] = -1.0;
            arr[j++] = points[i].second;
            arr[j++] = 1.0;
            arr[j++] = 0.0;
            arr[j++] = 0.0;
        }
        int x = 0, in = 0, y = numPoints + 1;
        for (int k = 0; k < numPoints * 6 - 6; ) {
            indices[k++] = x;
            indices[k++] = y;
            indices[k++] = x + 1;

            indices[k++] = y;
            indices[k++] = x;
            indices[k++] = y - 1;
            x++;
            y++;
            in = k;
        }
        indices[in++] = 9;
        indices[in++] = 10;
        indices[in++] = 0;
        indices[in++] = 10;
        indices[in++] = 9;
        indices[in++] = 19;

        
    }
    /*
	std::vector<glm::mat4> modelMatrices;
	float tox, toy, toz;
	Cylinders(float x = 0, float y = 0, float z = 0) {
		tox = x;
		toy = y;
		toz = z;
	}
	glm::mat4 transforamtion(float tx, float ty, float tz, float rx, float ry, float rz, float sx, float sy, float sz) {
		tx += tox;
		ty += toy;
		tz += toz;
		glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
		glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
		translateMatrix = glm::translate(identityMatrix, glm::vec3(tx, ty, tz));
		rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rx), glm::vec3(1.0f, 0.0f, 0.0f));
		rotateYMatrix = glm::rotate(identityMatrix, glm::radians(ry), glm::vec3(0.0f, 1.0f, 0.0f));
		rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rz), glm::vec3(0.0f, 0.0f, 1.0f));
		scaleMatrix = glm::scale(identityMatrix, glm::vec3(sx, sy, sz));
		model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
		return model;
	}*/
    /*
	Shader local_rotation(Shader ourShader, unsigned int VAO, unsigned int VAO2, unsigned int VAO3, unsigned int VAO4, unsigned int VAO5, float angle = 0) {
        glm::mat4 model;
        float rotateAngle_X = 0;
        float rotateAngle_Y = 0;
        float rotateAngle_Z = 0;
        model = transforamtion(0, 0, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 2.5, 0.2, 1.75);
        modelMatrices.push_back(model);
        model = transforamtion(0, 0, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.2, -1.5, 0.2);
        modelMatrices.push_back(model);
        model = transforamtion(1.15, 0, 0, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.2, -1.5, 0.2);
        modelMatrices.push_back(model);
        model = transforamtion(1.15, 0, .75, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.2, -1.5, 0.2);
        modelMatrices.push_back(model);
        model = transforamtion(0, 0, .75, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.2, -1.5, 0.2);
        modelMatrices.push_back(model);
        model = transforamtion(0.4, -.35, .8, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 1, 0.1, 1);
        modelMatrices.push_back(model);
        model = transforamtion(0.4, -.35, .8, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.1, -.8, 0.1);
        modelMatrices.push_back(model);
        model = transforamtion(.85, -.35, .8, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.1, -.8, 0.1);
        modelMatrices.push_back(model);
        model = transforamtion(.85, -.35, 1.25, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.1, -.8, 0.1);
        modelMatrices.push_back(model);
        model = transforamtion(0.4, -.35, 1.25, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.1, -.8, 0.1);
        modelMatrices.push_back(model);
        model = transforamtion(0.75, -.3, 1.2, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.1, .3, 0.1);
        modelMatrices.push_back(model);
        model = transforamtion(0.525, -.3, 1.2, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.1, .3, 0.1);
        modelMatrices.push_back(model);
        model = transforamtion(0.475, .1, 1.175, rotateAngle_X, rotateAngle_Y, rotateAngle_Z, 0.8, -.6, 0.2);
        modelMatrices.push_back(model);
        unsigned int vertex_array[] = { VAO, VAO2, VAO2, VAO2, VAO2, VAO3, VAO3, VAO3, VAO3, VAO3, VAO4, VAO4, VAO5 };
        glm::vec3 averagePosition(0.0f);
        for (const glm::mat4& model : modelMatrices) {
            averagePosition += glm::vec3(model[3]);
        }
        averagePosition /= modelMatrices.size();

        glm::mat4 moveToOrigin = glm::translate(glm::mat4(1.0f), -averagePosition);

        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

        glm::mat4 moveToOriginalPosition = glm::translate(glm::mat4(1.0f), averagePosition);

        glm::mat4 groupTransform = moveToOriginalPosition * rotation * moveToOrigin;

        int i = 0;
        for (glm::mat4& model : modelMatrices) {

            model = groupTransform * model;
            ourShader.setMat4("model", model);
            glBindVertexArray(vertex_array[i]);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            i++;
        }
		return ourShader;
	}*/
    /*
	Shader ret_shader(Shader ourShader, unsigned int VAO, unsigned int VAO2, unsigned int VAO3, unsigned int VAO4, unsigned int VAO5) {
		
		return ourShader;
	}*/
    //std::vector<glm::mat4> modelMatrices;
    // Define a typedef for the vector type
    
    /*
    typedef std::vector<float> FloatVector;
    int sectorCount;
    float height;
    float radius;
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<int> indices;


    // Use the typedef in your member function declaration
    FloatVector getUnitCircleVertices()
    {
        const float PI = 3.1415926f;
        float sectorStep = (2 * PI * radius) / sectorCount; //r
        float sectorAngle;  // radian

        std::vector<float> unitCircleVertices;
        for (int i = 0; i <= sectorCount; ++i)
        {
            sectorAngle = i * sectorStep;
            unitCircleVertices.push_back(cos(sectorAngle)); // x
            unitCircleVertices.push_back(sin(sectorAngle)); // y
            unitCircleVertices.push_back(0);                // z
        }
        return unitCircleVertices;
    }

        // generate vertices for a cylinder
    void buildVerticesSmooth()
    {
        // clear memory of prev arrays
        std::vector<float>().swap(vertices);
        std::vector<float>().swap(normals);
        std::vector<float>().swap(texCoords);

        // get unit circle vectors on XY-plane
        std::vector<float> unitVertices = getUnitCircleVertices();

        // put side vertices to arrays
        for (int i = 0; i < 2; ++i)
        {
            float h = -height / 2.0f + i * height;           // z value; -h/2 to h/2
            float t = 1.0f - i;                              // vertical tex coord; 1 to 0

            for (int j = 0, k = 0; j <= sectorCount; ++j, k += 3)
            {
                float ux = unitVertices[k];
                float uy = unitVertices[k + 1];
                float uz = unitVertices[k + 2];
                // position vector
                vertices.push_back(ux * radius);             // vx
                vertices.push_back(uy * radius);             // vy
                vertices.push_back(h);                       // vz
                // normal vector
                normals.push_back(ux);                       // nx
                normals.push_back(uy);                       // ny
                normals.push_back(uz);                       // nz
                // texture coordinate
                texCoords.push_back((float)j / sectorCount); // s
                texCoords.push_back(t);                      // t
            }
        }

        // the starting index for the base/top surface
        //NOTE: it is used for generating indices later
        //int baseCenterIndex = (int)vertices.size() / 3;
        //int topCenterIndex = baseCenterIndex + sectorCount + 1; // include center vertex

        int baseCenterIndex = 0;
        int topCenterIndex = 0; // include center vertex


        // put base and top vertices to arrays
        for (int i = 0; i < 2; ++i)
        {
            float h = -height / 2.0f + i * height;           // z value; -h/2 to h/2
            float nz = -1 + i * 2;                           // z value of normal; -1 to 1

            // center point
            vertices.push_back(0);     vertices.push_back(0);     vertices.push_back(h);
            normals.push_back(0);      normals.push_back(0);      normals.push_back(nz);
            texCoords.push_back(0.5f); texCoords.push_back(0.5f);

            for (int j = 0, k = 0; j < sectorCount; ++j, k += 3)
            {
                float ux = unitVertices[k];
                float uy = unitVertices[k + 1];
                // position vector
                vertices.push_back(ux * radius);             // vx
                vertices.push_back(uy * radius);             // vy
                vertices.push_back(h);                       // vz
                // normal vector
                normals.push_back(0);                        // nx
                normals.push_back(0);                        // ny
                normals.push_back(nz);                       // nz
                // texture coordinate
                texCoords.push_back(-ux * 0.5f + 0.5f);      // s
                texCoords.push_back(-uy * 0.5f + 0.5f);      // t
            }
        }
        int k1 = 0;                         // 1st vertex index at base
        int k2 = sectorCount + 1;           // 1st vertex index at top

        // indices for the side surface
        for (int i = 0; i < sectorCount; ++i, ++k1, ++k2)
        {
            // 2 triangles per sector
            // k1 => k1+1 => k2
            indices.push_back(k1);
            indices.push_back(k1 + 1);
            indices.push_back(k2);

            // k2 => k1+1 => k2+1
            indices.push_back(k2);
            indices.push_back(k1 + 1);
            indices.push_back(k2 + 1);
        }

        // indices for the base surface
        //NOTE: baseCenterIndex and topCenterIndices are pre-computed during vertex generation
        //      please see the previous code snippet
        for (int i = 0, k = baseCenterIndex + 1; i < sectorCount; ++i, ++k)
        {
            if (i < sectorCount - 1)
            {
                indices.push_back(baseCenterIndex);
                indices.push_back(k);
                indices.push_back(k + 1);
                
            }
            else // last triangle
            {
                indices.push_back(baseCenterIndex);
                indices.push_back(baseCenterIndex + 1);
                indices.push_back(k);
            }
        }

        // indices for the top surface
        for (int i = 0, k = topCenterIndex + 1; i < sectorCount; ++i, ++k)
        {
            if (i < sectorCount - 1)
            {
                indices.push_back(topCenterIndex);
                indices.push_back(k);
                indices.push_back(k + 1);
            }
            else // last triangle
            {
                indices.push_back(topCenterIndex);
                indices.push_back(k);
                indices.push_back(topCenterIndex + 1);
            }
        }
    }*/
        // generate CCW index list of cylinder triangles
    /*void generateCylinderIndices(int baseCenterIndex, int topCenterIndex)
    {
        
    }*/
};

#endif