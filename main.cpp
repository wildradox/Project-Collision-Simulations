#include <raylib.h>
#include <vector>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>

//struct bola
struct Ball {
    float posX, posY;
    float radius;
    float velX, velY;
    Color color;
};

float RandomFloat(float min, float max) {
    return min + (float)rand() / (float)(RAND_MAX/(max - min));
}

//bagian kotaknya quadtree
struct Rect {
    float cx, cy;
    float halfW, halfH;

    bool Contains(float x, float y) const {
        return (x >= cx - halfW && x <= cx + halfW &&
                y >= cy - halfH && y <= cy + halfH);
    }

    bool Intersects(const Rect &o) const {
        return !(o.cx - o.halfW > cx + halfW ||
                 o.cx + o.halfW < cx - halfW ||
                 o.cy - o.halfH > cy + halfH ||
                 o.cy + o.halfH < cy - halfH);
    }
};

/*quadtree yang memuat 
1. insert untuk masuk bola sesuai posisinya
2. query mengambil bola sesuai dengan areanya
3. draw untuk gambar kotak quadtree
*/
class QuadTree {
public:
    Rect box;
    int capacity;
    int depth;
    bool split = false;
    static const int MAX_DEPTH = 6;

    std::vector<int> ids;

    QuadTree *nw=nullptr, *ne=nullptr;
    QuadTree *sw=nullptr, *se=nullptr;

    QuadTree(Rect r, int cap, int d = 0)
    : box(r), capacity(cap), depth(d) {}


    void Clear() {
        ids.clear();
        if (split) {
            delete nw; delete ne;
            delete sw; delete se;
            nw=ne=sw=se=nullptr;
            split = false;
        }
    }

    void Subdivide() {
        float x = box.cx;
        float y = box.cy;
        float hw = box.halfW / 2;
        float hh = box.halfH / 2;
        if (depth >= MAX_DEPTH) return;

        nw = new QuadTree({x - hw, y - hh, hw, hh}, capacity, depth + 1);
        ne = new QuadTree({x + hw, y - hh, hw, hh}, capacity, depth + 1);
        sw = new QuadTree({x - hw, y + hh, hw, hh}, capacity, depth + 1);
        se = new QuadTree({x + hw, y + hh, hw, hh}, capacity, depth + 1);

        split = true;
    }

    bool Insert(int id, float x, float y) {
        if (!box.Contains(x, y)) return false;

        if ((int)ids.size() < capacity) {
            ids.push_back(id);
            return true;
        }

        if (!split) Subdivide();

        return (nw->Insert(id,x,y) ||
                ne->Insert(id,x,y) ||
                sw->Insert(id,x,y) ||
                se->Insert(id,x,y));
    }

    void Query(const Rect &range, std::vector<int> &out) const {
        if (!box.Intersects(range)) return;

        for (int id : ids) out.push_back(id);

        if (split) {
            nw->Query(range, out);
            ne->Query(range, out);
            sw->Query(range, out);
            se->Query(range, out);
        }
    }

    void Draw() const {
        DrawRectangleLines(
            box.cx - box.halfW,
            box.cy - box.halfH,
            box.halfW * 2,
            box.halfH * 2,
            DARKGRAY
        );

        if (split) {
            nw->Draw(); ne->Draw();
            sw->Draw(); se->Draw();
        }
    }
    void GetAllPairs(const std::vector<Ball>& balls,
                 std::vector<std::pair<int,int>>& pairs) const
{
    // Brute force kecil dalam satu node
    for (size_t i = 0; i < ids.size(); ++i) {
        for (size_t j = i + 1; j < ids.size(); ++j) {
            pairs.emplace_back(ids[i], ids[j]);
        }
    }

    if (split) {
        nw->GetAllPairs(balls, pairs);
        ne->GetAllPairs(balls, pairs);
        sw->GetAllPairs(balls, pairs);
        se->GetAllPairs(balls, pairs);
    }
}

};

/* Tabrakan bola
1. Hitung jarak dua bola
2. Jika jarak < radiusA + radiusB → mereka bertabrakan
3. Geser kedua bola supaya tidak menempel
4. Hitung arah tumbukan (normal vector)
5. Pantulkan kecepatannya
6. Jika kecepatannya terlalu kecil → dikasih nudging kecil supaya tidak diam */
void ResolveCollision(Ball &A, Ball &B) {
    float dx = B.posX - A.posX;
    float dy = B.posY - A.posY;
    float dist = sqrt(dx*dx + dy*dy);
    float minDist = A.radius + B.radius;

    if (dist < 0.0001f) {
        dx = 0.001f;
        dy = 0.001f;
        dist = 0.001f;
    }

    if (dist < minDist) {

        float nx = dx / dist;
        float ny = dy / dist;

        float overlap = (minDist - dist) * 0.52f;

        A.posX -= nx * overlap;
        A.posY -= ny * overlap;
        B.posX += nx * overlap;
        B.posY += ny * overlap;

        float rvx = B.velX - A.velX;
        float rvy = B.velY - A.velY;

        float velN = rvx*nx + rvy*ny;
        if (velN > 0) return;

        float impulse = -(1 + 1) * velN / 2;

        A.velX -= nx * impulse;
        A.velY -= ny * impulse;
        B.velX += nx * impulse;
        B.velY += ny * impulse;

        if (fabs(A.velX) < 5) A.velX += RandomFloat(-1,1);
        if (fabs(A.velY) < 5) A.velY += RandomFloat(-1,1);

        if (fabs(B.velX) < 5) B.velX += RandomFloat(-1,1);
        if (fabs(B.velY) < 5) B.velY += RandomFloat(-1,1);
    }
}

Ball SpawnBallAtCursor(const std::vector<Ball>& balls, Vector2 mouse, int screenW, int screenH) {
    Ball b;
    b.radius = RandomFloat(3.0f, 7.0f);

    b.posX = std::max(b.radius, std::min(mouse.x, screenW - b.radius));
    b.posY = std::max(b.radius, std::min(mouse.y, screenH - b.radius));

    // Cegah overlap: geser sedikit kalau nabrak bola lain
    for (const auto& o : balls) {
        float dx = b.posX - o.posX;
        float dy = b.posY - o.posY;
        float dist = sqrtf(dx*dx + dy*dy);
        float minDist = b.radius + o.radius;

        if (dist < minDist && dist > 0.0001f) {
            float nx = dx / dist;
            float ny = dy / dist;
            float push = minDist - dist + 1.0f;
            b.posX += nx * push;
            b.posY += ny * push;
        }
    }

    // Kecepatan random
    b.velX = RandomFloat(-180.0f, 180.0f);
    b.velY = RandomFloat(-180.0f, 180.0f);

    if (fabs(b.velX) < 30) b.velX += (b.velX < 0 ? -30 : 30);
    if (fabs(b.velY) < 30) b.velY += (b.velY < 0 ? -30 : 30);

    // Warna random
    b.color = {
        (unsigned char)(rand()%256),
        (unsigned char)(rand()%256),
        (unsigned char)(rand()%256),
        255
    };

    return b;
}


int main() {
    srand((unsigned int)time(NULL));

    const int screenW = 1400;
    const int screenH = 900;
    InitWindow(screenW, screenH, "PROJECT COLLISION — FINAL");
    SetTargetFPS(60);

    int BALL_COUNT = 1000;
    std::vector<Ball> balls;
    balls.reserve(BALL_COUNT);

    // Spawn without overlap (simple rejection sampling)
    for (int i = 0; i < BALL_COUNT; ++i) {
        Ball b;
        b.radius = RandomFloat(3.0f, 7.0f);

        bool placed = false;
        int attempts = 0;
        while (!placed && attempts < 2000) {
            placed = true;
            b.posX = RandomFloat(b.radius, screenW - b.radius);
            b.posY = RandomFloat(b.radius, screenH - b.radius);

            for (const auto &o : balls) {
                float dx = b.posX - o.posX;
                float dy = b.posY - o.posY;
                float d = sqrtf(dx*dx + dy*dy);
                if (d < b.radius + o.radius + 2.0f) { // small padding
                    placed = false;
                    break;
                }
            }
            ++attempts;
        }
        if (!placed) {
            // fallback: place anyway (rare)
            b.posX = RandomFloat(b.radius, screenW - b.radius);
            b.posY = RandomFloat(b.radius, screenH - b.radius);
        }

        b.velX = RandomFloat(-160.0f, 160.0f);
        b.velY = RandomFloat(-160.0f, 160.0f);

        // avoid extremely small velocities
        if (fabs(b.velX) < 20.0f) b.velX += (b.velX < 0 ? -20.0f : 20.0f);
        if (fabs(b.velY) < 20.0f) b.velY += (b.velY < 0 ? -20.0f : 20.0f);

        b.color = {(unsigned char)(rand()%256), (unsigned char)(rand()%256), (unsigned char)(rand()%256), 255};
        balls.push_back(b);
    }

    // UI / state
    bool paused = false;
    bool showHUD = true;
    bool showGrid = true;
    float speedScale = 1.0f;

    enum Mode { MODE_BRUTE, MODE_QUAD, MODE_HYBRID };
    Mode mode = MODE_QUAD;

    long long checkCount = 0;

    // We'll reuse a quadtree pointer each frame only when needed (fix 6A)
    QuadTree *frameQT = nullptr;

    // Main loop
    while (!WindowShouldClose()) {

        // ---------- INPUT ----------
        if (IsKeyPressed(KEY_SPACE)) paused = !paused;
        if (IsKeyPressed(KEY_H)) showHUD = !showHUD;
        if (IsKeyPressed(KEY_G)) showGrid = !showGrid;

        if (IsKeyPressed(KEY_Z)) speedScale = 0.3f;
        if (IsKeyPressed(KEY_X)) speedScale = 1.0f;

        if (IsKeyPressed(KEY_B)) mode = MODE_BRUTE;
        if (IsKeyPressed(KEY_Q)) mode = MODE_QUAD;
        if (IsKeyPressed(KEY_T)) mode = MODE_HYBRID;

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Vector2 mouse = GetMousePosition();
        balls.push_back(SpawnBallAtCursor(balls, mouse, screenW, screenH));
        BALL_COUNT++;
        }

        float dt = GetFrameTime() * speedScale;
        checkCount = 0;

        // ---------- UPDATE / MOVEMENT ----------
        if (!paused) {
            // Move balls and correct wall collisions (anti-stuck)
            for (auto &b : balls) {
                b.posX += b.velX * dt;
                b.posY += b.velY * dt;

                // X-axis
                if (b.posX - b.radius < 0.0f) {
                    b.posX = b.radius;
                    b.velX *= -1.0f;
                } else if (b.posX + b.radius > screenW) {
                    b.posX = screenW - b.radius;
                    b.velX *= -1.0f;
                }
                // Y-axis
                if (b.posY - b.radius < 0.0f) {
                    b.posY = b.radius;
                    b.velY *= -1.0f;
                } else if (b.posY + b.radius > screenH) {
                    b.posY = screenH - b.radius;
                    b.velY *= -1.0f;
                }
            }

            if (mode == MODE_QUAD || mode == MODE_HYBRID) {
                // free previous if any
                if (frameQT) { delete frameQT; frameQT = nullptr; }

                Rect world = { screenW/2.0f, screenH/2.0f, screenW/2.0f, screenH/2.0f };
                frameQT = new QuadTree(world, 12);

                for (int i = 0; i < BALL_COUNT; ++i) {
                    frameQT->Insert(i, balls[i].posX, balls[i].posY);
                }
            } else {
                if (frameQT) { delete frameQT; frameQT = nullptr; }
            }

            if (mode == MODE_BRUTE) {
                for (int i = 0; i < BALL_COUNT; ++i) {
                    for (int j = i + 1; j < BALL_COUNT; ++j) {
                        ++checkCount;
                        ResolveCollision(balls[i], balls[j]);
                    }
                }
            }
            else if (mode == MODE_QUAD) {
                for (int i = 0; i < BALL_COUNT; ++i) {
                    Rect q = { balls[i].posX, balls[i].posY, balls[i].radius * 4.0f, balls[i].radius * 4.0f };
                    std::vector<int> found;
                    frameQT->Query(q, found);
                    for (int id : found) {
                        if (id > i) {
                            ++checkCount;
                            ResolveCollision(balls[i], balls[id]);
                        }
                    }
                }
            }
            else { 
                std::vector<std::pair<int,int>> collisionPairs;
                collisionPairs.reserve(BALL_COUNT * 4);
                frameQT->GetAllPairs(balls, collisionPairs);
                for (auto& p : collisionPairs) {
                    ++checkCount;
                    ResolveCollision(balls[p.first], balls[p.second]);
                }
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        // Gambar quadtree
        if ((mode == MODE_QUAD || mode == MODE_HYBRID) && showGrid && frameQT) {
            frameQT->Draw();
        }

        // Gambar Bola
        for (auto &b : balls) {
            DrawCircleV({b.posX, b.posY}, b.radius, b.color);
        }

        // HUD
        if (showHUD) {

            DrawRectangle(0, 0, 260, screenH, Fade(DARKGRAY, 0.65f));

            int y = 20;

            DrawText("PROJECT COLLISION", 20, y, 22, YELLOW);
            y += 40;

            DrawText("MODE:", 20, y, 20, WHITE); 
            y += 25;

            DrawText(
                mode == MODE_BRUTE ? "Brute Force" :
                mode == MODE_QUAD  ? "Quadtree" :
                                     "Hybrid",
                20, y, 22,
                mode == MODE_BRUTE ? RED :
                mode == MODE_QUAD ? GREEN :
                                    SKYBLUE
            );
            y += 40;

            DrawText("TOGGLES:", 20, y, 20, WHITE); 
            y += 25;
            DrawText("B = BruteForce", 20, y, 18, WHITE); y += 20;
            DrawText("Q = Quadtree", 20, y, 18, WHITE); y += 20;
            DrawText("T = Hybrid", 20, y, 18, WHITE); y += 20;
            DrawText("G = Toggle Grid", 20, y, 18, WHITE); y += 20;
            DrawText("H = Toggle HUD", 20, y, 18, WHITE); y += 20;
            DrawText("SPACE = Pause", 20, y, 18, WHITE); y += 20;
            DrawText("Z = Slow Motion", 20, y, 18, WHITE); y += 20;
            DrawText("X = Normal Speed", 20, y, 18, WHITE); y += 40;
            DrawText("LMB = Spawn Ball", 20, y, 18, WHITE); y += 40;


            DrawText("STATS:", 20, y, 20, WHITE); 
            y += 25;

            DrawText(TextFormat("Balls   : %d", BALL_COUNT), 20, y, 18, YELLOW); 
            y += 20;
            DrawText(TextFormat("Checks  : %lld", checkCount), 20, y, 18, YELLOW); 
            y += 20;
            DrawText(TextFormat("FPS     : %d", GetFPS()), 20, y, 18, YELLOW); 
            y += 20;

            float avgSpeed = 0;
            for (auto &b : balls)
                avgSpeed += fabs(b.velX) + fabs(b.velY);
            avgSpeed /= BALL_COUNT;

            DrawText(TextFormat("Avg Speed : %.1f", avgSpeed), 20, y, 18, YELLOW);
            y += 25;

            DrawText(paused ? "PAUSED" : "RUNNING", 
                     20, y, 24, paused ? RED : GREEN);
        }

        EndDrawing();
    }

    // cleanup
    if (frameQT) delete frameQT;
    CloseWindow();
    return 0;
}
