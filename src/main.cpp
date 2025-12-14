#include <raylib.h>
#include <cstdlib>
#include <entt.hpp>
#include <random>

struct PlayerTag {};

struct BulletTag {};

struct AsteroidTag {};

struct Position {
    Vector2 vec2;
};

struct Velocity {
    Vector2 vec2;
};

struct Lifetime {
    float seconds;
};

struct RenderableCircle {
    int radius;
    Color color;
};

struct Shooter {
    float cooldown;
    float interval;
    float bulletSpeed;
};

static void SpawnBullet(entt::registry& world, Vector2 pos, float speed) {
    auto b = world.create();
    world.emplace<BulletTag>(b);
    world.emplace<Position>(b, pos);
    world.emplace<Velocity>(b, Vector2{0.0f, -speed});
    world.emplace<RenderableCircle>(b, 8, ORANGE);
    world.emplace<Lifetime>(b, 1.5f);
}

static void SpawnAsteroid(entt::registry& world, Vector2 pos, float speed, int size, float lifetime) {
    auto a = world.create();
    world.emplace<AsteroidTag>(a);
    world.emplace<Position>(a, pos);
    world.emplace<Velocity>(a, Vector2{0.0f, speed});
    world.emplace<RenderableCircle>(a, size, DARKBROWN);
    world.emplace<Lifetime>(a, lifetime);
}

static void ShootingSystem(entt::registry& world, float dt) {
    const bool fireHeld = IsKeyDown(KEY_SPACE);

    auto view = world.view<PlayerTag, Position, Shooter>();
    for (auto e : view) {
        auto& pos = view.get<Position>(e).vec2;
        auto& sh = view.get<Shooter>(e);

        sh.cooldown -= dt;
        if (sh.cooldown < 0.0f) {
            sh.cooldown = 0.0f;
        }

        if (fireHeld && sh.cooldown <= 0.0f) {
            sh.cooldown = sh.interval;
            SpawnBullet(world, pos, sh.bulletSpeed);
        }
    }
}

static void MovementSystem(entt::registry& world, float dt) {
    for (auto e : world.view<Position, Velocity>()) {
        auto& p = world.get<Position>(e).vec2;
        auto& v = world.get<Velocity>(e).vec2;
        p.x += v.x * dt;
        p.y += v.y * dt;
    }

    for (auto e : world.view<PlayerTag, Velocity>()) {
        auto& v = world.get<Velocity>(e).vec2;
        v.x *= 0.90f;
        v.y *= 0.90f;
    }
}

static void ClampPlayerToScreen(entt::registry& world, int w, int h) {
    for (auto e : world.view<PlayerTag, Position, RenderableCircle, Velocity>()) {
        auto& p = world.get<Position>(e).vec2;
        auto& r = world.get<RenderableCircle>(e).radius;
        auto& v = world.get<Velocity>(e).vec2;

        float minX = (float)r;
        float maxX = (float)w - r;
        float minY = (float)r;
        float maxY = (float)h - r;

        float newX = std::clamp(p.x, minX, maxX);
        float newY = std::clamp(p.y, minY, maxY);

        if (newX != p.x) {
            v.x = 0.0f;
        }
        if (newY != p.y) {
            v.y = 0.0f;
        }

        p.x = newX;
        p.y = newY;
    }
}

static void LifetimeSystem(entt::registry& world, float dt) {
    std::vector<entt::entity> dead;
    auto view = world.view<Lifetime>();
    for (auto e : view) {
        auto& life = view.get<Lifetime>(e).seconds;
        life -= dt;
        if (life <= 0.0f) {
            dead.push_back(e);
        }
    }
    for (auto e : dead) {
        world.destroy(e);
    }
}

static void AsteroidSpawnSystem(entt::registry& world, float dt, int width, int height) {
    static float timer = 0.0f;
    const float interval = 0.25f;

    timer -= dt;

    if (timer < 0.0f) {
        timer += interval;
        static std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<std::mt19937::result_type> distX(0, width);
        std::uniform_int_distribution<std::mt19937::result_type> distSize(16, 64);
        int x = distX(rng);
        float size = distSize(rng);
        SpawnAsteroid(world, Vector2{(float)x, -(size + 1)}, 10 * size, size, height / size);
    }
}

int main() {
    const int TARGET_FPS = 120;
    int screenWidth = 1600, screenHeight = 900;
    const float playerX = screenWidth / 2.0;
    const float playerY = 86 * screenHeight / 90.0;
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "Asteroids");
    SetTargetFPS(TARGET_FPS);

    entt::registry world;

    auto player = world.create();
    world.emplace<PlayerTag>(player);
    world.emplace<Position>(player, Vector2{playerX, playerY});
    world.emplace<Velocity>(player, Vector2{0, 0});
    world.emplace<RenderableCircle>(player, 32, BLACK);
    world.emplace<Shooter>(player, 0.0f, TARGET_FPS / 500.0f, 1000.0f);

    while (!WindowShouldClose()) {
        if (IsWindowResized() && !IsWindowFullscreen()) {
            screenWidth = GetScreenWidth();
            screenHeight = GetScreenHeight();
        }

        float dt = GetFrameTime();

        BeginDrawing();
        ClearBackground(DARKBLUE);

        auto& vel = world.get<Velocity>(player);
        if (IsKeyDown(KEY_W)) {
            vel.vec2.y = -700.0f;
        }
        if (IsKeyDown(KEY_A)) {
            vel.vec2.x = -700.0f;
        }
        if (IsKeyDown(KEY_S)) {
            vel.vec2.y = 700.0f;
        }
        if (IsKeyDown(KEY_D)) {
            vel.vec2.x = 700.0f;
        }
        AsteroidSpawnSystem(world, dt, screenWidth, screenHeight);
        ShootingSystem(world, dt);
        MovementSystem(world, dt);
        ClampPlayerToScreen(world, screenWidth, screenHeight);
        LifetimeSystem(world, dt);

        auto rederableCircles = world.view<Position, RenderableCircle>();
        for (auto [entity, pos, cc] : rederableCircles.each()) {
            DrawCircle(pos.vec2.x, pos.vec2.y, cc.radius, cc.color);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
