#ifndef SHAPE_MANAGER_H
#define SHAPE_MANAGER_H

#include <glad/glad.h>
#include <vector>
#include <iostream>

/**
 * @class ShapeManager
 * @brief Manages the creation, rendering, and cleanup of shapes using OpenGL VAOs, VBOs, and EBOs.
 */
class ShapeManager {
public:
    /**
     * @struct Shape
     * @brief Represents a shape with its OpenGL buffers and vertex/index counts.
     */
    struct Shape {
        unsigned int VAO;         /* Vertex Array Object */
        unsigned int VBO;         /* Vertex Buffer Object */
        unsigned int EBO;         /* Element Buffer Object */
        unsigned int vertexCount; /* Number of vertices */
        unsigned int indexCount;  /* Number of indices */
    };

    /**
     * @brief Default constructor for ShapeManager.
     */
    ShapeManager() = default;

    /**
     * @brief Creates a new shape and sets up its VAO, VBO, and optional EBO.
     *
     * @param vertices: Pointer to the array of vertices.
     * @param vertexCount: Number of vertices in the array.
     * @param indices: Pointer to the array of indices (optional).
     * @param indexCount: Number of indices in the array (default is 0).
     * @return Index of the created shape in the internal shape list.
     */
    int createShape(const float* vertices, unsigned int vertexCount, const unsigned int* indices = nullptr, unsigned int indexCount = 0) {
        Shape shape{};

        // Generate Buffers
        glGenVertexArrays(1, &shape.VAO);
        glGenBuffers(1, &shape.VBO);

        // Shape setup
        glBindVertexArray(shape.VAO);
        glBindBuffer(GL_ARRAY_BUFFER, shape.VBO);
        glBufferData(GL_ARRAY_BUFFER, vertexCount, vertices, GL_STATIC_DRAW);

        // Generate EBO if needed
        if (indices != nullptr) {
            glGenBuffers(1, &shape.EBO);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, shape.EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);
            shape.indexCount = indexCount;
        }

        shape.vertexCount = vertexCount;
        shapes.push_back(shape);

        glBindVertexArray(0); // Unbind VAO
        return shapes.size() - 1; // Return the index of the created shape
    }

    /**
     * @brief Adds an attribute to a shape's VAO.
     *
     * @param shapeIndex: Index of the shape in the internal list.
     * @param index: Layout location of the attribute.
     * @param size: Number of components per vertex attribute (e.g., 3 for vec3).
     * @param type: Data type of each component (e.g., GL_FLOAT).
     * @param normalized: Whether fixed-point data values should be normalized (GL_TRUE or GL_FALSE).
     * @param stride: Byte offset between consecutive vertex attributes.
     * @param offset: Pointer offset to the first component of the first attribute.
     */
    void addAttribute(int shapeIndex,
        unsigned int index,
        int size,
        GLenum type,
        GLboolean normalized,
        unsigned int stride,
        void* offset) {
        if (shapeIndex < 0 || shapeIndex >= shapes.size()) {
            std::cerr << "Error: Invalid shape index.\n";
            return;
        }

        glBindVertexArray(shapes[shapeIndex].VAO);
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index, size, type, normalized, stride, offset);
        glBindVertexArray(0); // Unbind VAO
    }

    /**
     * @brief Renders a shape by its index.
     *
     * @param shapeIndex: Index of the shape in the internal list.
     * @param mode: OpenGL drawing mode
     */
    void renderShape(int shapeIndex, int constant, GLenum mode = GL_TRIANGLES) {
        if (shapeIndex < 0 || shapeIndex >= shapes.size()) {
            std::cerr << "Error: Invalid shape index.\n";
            return;
        }

        glBindVertexArray(shapes[shapeIndex].VAO);
        if (shapes[shapeIndex].indexCount > 0) {
            glDrawElements(mode, shapes[shapeIndex].indexCount, GL_UNSIGNED_INT, 0);
        }
        else {
            glDrawArrays(mode, 0, shapes[shapeIndex].vertexCount / constant);
        }
        glBindVertexArray(0); // Unbind VAO
    }

    /**
     * @brief Cleans up all shapes by deleting their VAOs, VBOs, and EBOs.
     */
    void cleanup() {
        for (const Shape& shape : shapes) {
            glDeleteVertexArrays(1, &shape.VAO);
            glDeleteBuffers(1, &shape.VBO);
            if (shape.indexCount > 0) {
                glDeleteBuffers(1, &shape.EBO);
            }
        }
        shapes.clear();
    }

    /**
     * @brief Gets the VAO of a shape.
     *
     * @param shapeIndex: Index of the shape in the internal list.
     * @return The VAO ID of the shape, or 0 if the index is invalid.
     */
    unsigned int getVAO(int shapeIndex) const {
        if (shapeIndex < 0 || shapeIndex >= shapes.size()) {
            std::cerr << "Error: Invalid shape index.\n";
            return 0;
        }
        return shapes[shapeIndex].VAO;
    }

    /**
     * @brief Gets the VBO of a shape.
     *
     * @param shapeIndex: Index of the shape in the internal list.
     * @return The VBO ID of the shape, or 0 if the index is invalid.
     */
    unsigned int getVBO(int shapeIndex) const {
        if (shapeIndex < 0 || shapeIndex >= shapes.size()) {
            std::cerr << "Error: Invalid shape index.\n";
            return 0;
        }
        return shapes[shapeIndex].VBO;
    }

    /**
     * @brief Gets the EBO of a shape.
     *
     * @param shapeIndex: Index of the shape in the internal list.
     * @return The EBO ID of the shape, or 0 if the index is invalid.
     */
    unsigned int getEBO(int shapeIndex) const {
        if (shapeIndex < 0 || shapeIndex >= shapes.size()) {
            std::cerr << "Error: Invalid shape index.\n";
            return 0;
        }
        return shapes[shapeIndex].EBO;
    }
    /**
    * @brief Destructor
    */
    ~ShapeManager() {
        cleanup();
    }

private:
    std::vector<Shape> shapes; /* Internal list of shapes managed by ShapeManager */
};

#endif
