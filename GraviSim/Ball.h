#ifndef BALL_H
#define BALL_H

#include <glm/gtc/matrix_transform.hpp>

/**
 * @brief Represents a ball with physics properties such as position, velocity, gravity, and damping.
 */
class Ball {
public:
    glm::vec3 position; // Position of the ball in 3D space.
    glm::vec2 velocity; // Velocity of the ball in 2D (x, y) plane.
    float radius; // Radius of the ball.
    float gravity = -9.81f; // Gravity affecting the ball's motion.
    float damping = 0.8f; // Damping factor applied during collisions.
    float velocityThreshold = 0.01f; // Minimum velocity below which movement stops.
    int segments; // Number of segments used to approximate the circle.

    /**
     * @brief Constructs a Ball object with initial position, velocity, radius, and resolution.
     *
     * @param pos Initial position of the ball.
     * @param vel Initial velocity of the ball.
     * @param r Radius of the ball.
     * @param res Number of segments for circle approximation.
     */
    Ball(glm::vec3 pos, glm::vec2 vel, float r, int res)
        : position(pos), velocity(vel), radius(r), segments(res) {
    }

    /**
     * @brief Updates the ball's physics including position and velocity.
     *
     * @param deltaTime Time step for the physics update.
     */
    void updatePhysics(float deltaTime) {
        // Apply gravity if above ground
        //if (position.y - radius > -1.0f)
        //    velocity.y += gravity * deltaTime;

        // Update position
        position += glm::vec3(velocity * deltaTime, 0.0f);

        // Handle collisions
        handleCollisions();
    }

    /**
     * @brief Generates the vertex data for rendering the ball as a 2D circle.
     *
     * @param circleVertices Vector to store generated vertex data.
     */
    void generateBallVertices(std::vector<float>& circleVertices) {
        // Center position of the circle
        circleVertices.push_back(0.0f);
        circleVertices.push_back(0.0f);
        circleVertices.push_back(0.0f);

        // Generate circle vertices
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
    /**
     * @brief Handles ball collisions with boundaries and applies response forces.
     */
    void handleCollisions() {
        // Collision with left or right wall
        if (position.x + radius >= 1.0f || position.x - radius <= -1.0f) {
            velocity.x *= -damping;
            position.x = glm::clamp(position.x, -1.0f + radius, 1.0f - radius);
            if (fabs(velocity.x) < velocityThreshold) velocity.x = 0.0f;
        }

        // Collision with the ground
        if (position.y - radius <= -1.0f) {
            velocity.y *= -damping;
            position.y = -1.0f + radius;
            velocity.x *= damping;  // Apply ground friction
            if (fabs(velocity.y) < velocityThreshold) velocity.y = 0.0f;
            if (fabs(velocity.x) < velocityThreshold) velocity.x = 0.0f;
        }
        // Collision with the ceiling
        else if (position.y + radius >= 1.0f) {
            velocity.y *= -damping;
            position.y = 1.0f - radius;
            if (fabs(velocity.y) < velocityThreshold) velocity.y = 0.0f;
        }
    }
};

#endif
