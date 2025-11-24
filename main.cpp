#include <raylib.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>

struct Ball {
    float x, y;
    float radius;
    float vx, vy;
    Color color;
};

float randFloat(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX/(max - min)));
}

int main() {
    srand(time(NULL));

    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Raylib Random Balls with Collision");

    const int BALL_COUNT = 20;
    std::vector<Ball> balls;

    for (int i = 0; i < BALL_COUNT; i++) {
        Ball b;
        b.radius = randFloat(10, 25);
        b.x = randFloat(b.radius, screenWidth - b.radius);
        b.y = randFloat(b.radius, screenHeight - b.radius);

        b.vx = randFloat(-200, 200);
        b.vy = randFloat(-200, 200);

        b.color = Color{ 
            (unsigned char)(rand() % 255), 
            (unsigned char)(rand() % 255), 
            (unsigned char)(rand() % 255),
            255
        };

        balls.push_back(b);
    }

    SetTargetFPS(60);


    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        for (auto &b : balls) {
            b.x += b.vx * dt;
            b.y += b.vy * dt;

        
            if (b.x - b.radius <= 0) {
                b.x = b.radius;
                b.vx *= -1;
            }
            if (b.x + b.radius >= screenWidth) {
                b.x = screenWidth - b.radius;
                b.vx *= -1;
            }
            if (b.y - b.radius <= 0) {
                b.y = b.radius;
                b.vy *= -1;
            }
            if (b.y + b.radius >= screenHeight) {
                b.y = screenHeight - b.radius;
                b.vy *= -1;
            }
        };

        
        for (size_t i = 0; i < balls.size(); ++i) {
            for (size_t j = i + 1; j < balls.size(); ++j) {
                Ball &a = balls[i];
                Ball &b = balls[j];
                float dx = b.x - a.x;
                float dy = b.y - a.y;
                float dist = sqrtf(dx * dx + dy * dy);
                float minDist = a.radius + b.radius;
                if (dist < minDist && dist > 0.0f) {

                    float overlap = 0.5f * (minDist - dist);
                    float nx = dx / dist;
                    float ny = dy / dist;
                    a.x -= overlap * nx;
                    a.y -= overlap * ny;
                    b.x += overlap * nx;
                    b.y += overlap * ny;

                    float rvx = b.vx - a.vx;
                    float rvy = b.vy - a.vy;
                    float velAlongNormal = rvx * nx + rvy * ny;
                    if (velAlongNormal > 0) continue; 

                    float restitution = 1.0f; 
                    float impulse = -(1 + restitution) * velAlongNormal / 2.0f;
                    float impulseX = impulse * nx;
                    float impulseY = impulse * ny;
                    a.vx -= impulseX;
                    a.vy -= impulseY;
                    b.vx += impulseX;
                    b.vy += impulseY;
                }
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        for (auto &b : balls) {
            DrawCircleV(Vector2{b.x, b.y}, b.radius, b.color);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}

