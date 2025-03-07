#ifndef BALL_H
#define BALL_H

#include <glm/gtc/matrix_transform.hpp>

class Ball {
public:
    glm::vec3 position;
    glm::vec2 velocity;
    float radius;
    float gravity = -9.81f;
    float damping = 0.8f;
    float velocityThreshold = 0.01f;
    int segments;

    Ball(glm::vec3 pos, glm::vec2 vel, float r, int res)
        : position(pos), velocity(vel), radius(r), segments(res) {
    }

    void updatePhysics(float deltaTime) {
        // Apply gravity if above ground
        if (position.y - radius > -1.0f)
            velocity.y += gravity * deltaTime;

        // Update position
        position += glm::vec3(velocity * deltaTime, 0.0f);

        // Handle collisions
        handleCollisions();
    }

    void generateBallVertices(std::vector<float>& circleVertices) {
        // Center position of the circle
        circleVertices.push_back(0.0f);
        circleVertices.push_back(0.0f);
        circleVertices.push_back(0.0f);

        // Generate circle circleVertices
        for (int i = 0; i <= segments; i++) {
            float angle = (2.0f * glm::pi<float>() * i) / segments;
            float x = radius * cos(angle);
            float y = radius * sin(angle);
            circleVertices.push_back(x);
            circleVertices.push_back(y);
            circleVertices.push_back(0.0f);
        }
    }

private:
    void handleCollisions() {
        if (position.x + radius >= 1.0f || position.x - radius <= -1.0f) {
            velocity.x *= -damping;
            position.x = glm::clamp(position.x, -1.0f + radius, 1.0f - radius);
            if (fabs(velocity.x) < velocityThreshold) velocity.x = 0.0f;
        }

        if (position.y - radius <= -1.0f) { // Ground collision
            velocity.y *= -damping;
            position.y = -1.0f + radius;
            velocity.x *= damping;  // Apply ground friction
            if (fabs(velocity.y) < velocityThreshold) velocity.y = 0.0f;
            if (fabs(velocity.x) < velocityThreshold) velocity.x = 0.0f;
        }
        else if (position.y + radius >= 1.0f) { // Ceiling collision
            velocity.y *= -damping;
            position.y = 1.0f - radius;
            if (fabs(velocity.y) < velocityThreshold) velocity.y = 0.0f;
        }
    }
};


#endif // !BALL_H
