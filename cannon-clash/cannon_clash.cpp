#include <algorithm>
#include <array>
#include <cmath>
#include "ExtraHeader.h"
#include <raylib.h>
#include <raymath.h>
#include <vector>
#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

constexpr int WINDOW_WIDTH = 1000;
constexpr int WINDOW_HEIGHT = 800;

Texture2D textureTerrain;
Texture2D textureCannonTurret;
Texture2D textureCannonWheel;
Texture2D textureBall;
Texture2D textureSniperCar;
Texture2D textureBullet;
Texture2D textureTitleScreen;
Texture2D textureCoin;
Texture2D textureDrone;
Texture2D texturePause;
Texture2D texturePlay;

Font font;

Sound soundFail;
Sound soundKaching;
Sound soundSelection;
Sound soundCarHit;
Sound soundGunshot;
Sound soundCollect;

Music musicOverheating;
Music musicBackground;

void loadMedia() {
    textureTerrain = LoadTexture("images/terrain.png");
    textureCannonTurret = LoadTexture("images/cannon_turret.png");
    textureCannonWheel = LoadTexture("images/cannon_wheel.png");
    textureBall = LoadTexture("images/ball.png");
    textureSniperCar = LoadTexture("images/sniper_car.png");
    textureBullet = LoadTexture("images/bullet.png");
    textureTitleScreen = LoadTexture("images/title_screen.png");
    textureCoin = LoadTexture("images/coin.png");
    textureDrone = LoadTexture("images/drone.png");
    texturePause = LoadTexture("images/pause.png");
    texturePlay = LoadTexture("images/play.png");

    soundFail = LoadSound("sounds/fail.wav");
    soundKaching = LoadSound("sounds/kaching.wav");
    soundSelection = LoadSound("sounds/selection.wav");
    soundCarHit = LoadSound("sounds/car_hit.wav");
    soundGunshot = LoadSound("sounds/gunshot.wav");
    soundCollect = LoadSound("sounds/collect.wav");

    musicOverheating = LoadMusicStream("sounds/overheating.wav");
    musicOverheating.looping = true;
    musicBackground = LoadMusicStream("sounds/background.wav");
    musicBackground.looping = true;

    font = LoadFontEx("fonts/font.ttf", 64, nullptr, 0);
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
}

class Player {
public:
    Vector2 position;
    Vector2 turretSize;
    Vector2 wheelSize;
    Vector2 turretOrigin;
    Vector2 wheelOrigin;
    float velocityX;
    float turretAngleDeg;
    float timeElapsed;
    float acceleration = 0;
    float wheelAngle = 0;
    float shootingPower = 0.0f;
    float overheatingDuration = 6.0f;
    float hp;
    float maxHealth = 100.0f;
    int timesCannonShotPrematurely = 0;
    float cannonShotPrematurelyUpdateTime = 0;
    float lastOverheatingUpdateTime = 0;
    static constexpr float groundY = WINDOW_HEIGHT - 150.0f;
    std::array<Vector2, 4> rectCorners;
    int coins = 0;

    bool shouldShoot = false;
    bool shouldFlipCannonTurretImage = false;
    bool isOverHeating = false;

    Player() {
        this->position = {WINDOW_WIDTH / 2 - 100.0f, groundY - 100.0f};
        this->turretSize = {(float)textureCannonTurret.width, (float)textureCannonTurret.height};
        this->wheelSize = {(float)textureCannonWheel.width, (float)textureCannonWheel.height};
        this->turretOrigin = {turretSize.x / 2, turretSize.y / 2};
        this->wheelOrigin = {wheelSize.x / 2, wheelSize.y / 2};
        this->turretAngleDeg = 0;
        this->velocityX = 6.0f;
        this->hp = maxHealth;
        PlayMusicStream(musicOverheating);
    }

    void draw() {
        Rectangle src = {0, 0, turretSize.x, shouldFlipCannonTurretImage ? -turretSize.y : turretSize.y};
        Rectangle dest = {position.x + turretSize.x / 2.0f, position.y - 50.0f + turretSize.y / 2.0f, turretSize.x, turretSize.y};
        DrawTexturePro(textureCannonTurret, src, dest, turretOrigin, turretAngleDeg, WHITE);

        src = {0, 0, wheelSize.x, wheelSize.y};
        dest = {position.x + 50.0f + wheelSize.x / 2, position.y + wheelSize.y / 2, wheelSize.x, wheelSize.y};
        DrawTexturePro(textureCannonWheel, src, dest, wheelOrigin, wheelAngle, WHITE);
    }
    void update(float dt, float timeElapsed) {
        this->timeElapsed = timeElapsed;
        const float angleRotationSpeed = 150.0f;

        if ((IsKeyDown(KEY_E) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) && turretAngleDeg < 45) {
            turretAngleDeg += angleRotationSpeed * dt;
        }
        if ((IsKeyDown(KEY_Q) || IsMouseButtonDown(MOUSE_BUTTON_LEFT)) && turretAngleDeg > -225) {
            turretAngleDeg -= angleRotationSpeed * dt;
        }
        if (IsKeyDown(KEY_A) && position.x + 50.0f > 0) {
            position.x -= velocityX + acceleration;
            wheelAngle -= 2;
        }
        if (IsKeyDown(KEY_D) && position.x + 50.0f + textureCannonWheel.width < WINDOW_WIDTH) {
            position.x += velocityX + acceleration;
            wheelAngle += 2;
        }
        if (IsKeyDown(KEY_SPACE) && !isOverHeating) {
            shootingPower += 120.0f * dt;
        }
        if (IsKeyUp(KEY_SPACE) && shootingPower > 0 && !isOverHeating) {
            if (shootingPower < 50.0f) {
                timesCannonShotPrematurely++;
                cannonShotPrematurelyUpdateTime = timeElapsed;
            }
            shouldShoot = true;
        }
        if (shootingPower > 100.0f) {
            shootingPower = 100.0f;
        }
        if (timesCannonShotPrematurely >= 6) {
            isOverHeating = true;
            timesCannonShotPrematurely = 0;
            lastOverheatingUpdateTime = timeElapsed;
            shootingPower = 0;
        }
        if (isOverHeating && timeElapsed - lastOverheatingUpdateTime > overheatingDuration) {
            isOverHeating = false;
            lastOverheatingUpdateTime = timeElapsed;
        }
        if (isOverHeating) {
            UpdateMusicStream(musicOverheating);
        }
        if (turretAngleDeg < -90.0f) {
            shouldFlipCannonTurretImage = true;
        } else {
            shouldFlipCannonTurretImage = false;
        }
        if (timesCannonShotPrematurely && timeElapsed - cannonShotPrematurelyUpdateTime > 1.0f) {
            timesCannonShotPrematurely--;
            cannonShotPrematurelyUpdateTime = timeElapsed;
        }
        std::array<Vector2, 4> localCorners = {
            Vector2{ -turretOrigin.x, -turretOrigin.y },
            Vector2{  turretOrigin.x, -turretOrigin.y },
            Vector2{  turretOrigin.x,  turretOrigin.y },
            Vector2{ -turretOrigin.x,  turretOrigin.y }
        };
        for (int i = 0; i < 4; i++) {
            Vector2 rotated = Vector2Rotate(localCorners[i], turretAngleDeg * DEG2RAD);
            rectCorners[i] = Vector2Add({position.x + turretSize.x / 2.0f, position.y - 50.0f + turretSize.y / 2.0f}, rotated);
        }
    }
    void drawDebugLines() {
        for (int i = 0; i < rectCorners.size(); i++) {
            DrawLineEx(rectCorners.at(i), rectCorners.at(i != 3 ? i+1 : 0), 2, RED);
        }
        DrawCircleV(position, 5, BLUE);
    }
    void updateMaximumHealth(int gameDifficultyIndex) {
        switch (gameDifficultyIndex) {
            case 0: maxHealth = 100.0f; break;
            case 1: maxHealth = 200.0f; break;
            case 2: maxHealth = 150.0f; break;
            case 3: maxHealth = 100.0f; break;
        };
    }
};

class Ball {
public: 
    Vector2 position;
    Vector2 size;
    Vector2 velocity;
    Vector2 originalVelocity;
    static constexpr float gravity = 1000.0f;
    static constexpr float drag = 0.99f;
    float firePower = 1000.0f;
    static constexpr float groundY = Player::groundY;
    float shotPower;
    float angleDirection;
    double destructionUpdateTime;

    bool isOnGround = false;
    bool shouldBeDestroyed = false;
    bool createdDestructionTimer = false;

    Ball(Vector2 position, float angleDirection, float shotPower) {
        this->size = {(float)textureBall.width, (float)textureBall.height};
        position = {position.x - size.x / 2, position.y - size.y / 2};
        this->position = position;
        this->angleDirection = angleDirection;
        float angleRad = angleDirection * DEG2RAD;
        firePower *= shotPower / 100.0f;
        this->velocity = {cosf(angleRad) * firePower, sinf(angleRad) * firePower};
    }

    void draw() {
        DrawTexture(textureBall, position.x, position.y, WHITE);
    }
    void update(float dt, float timeElapsed) {
        velocity.y += gravity * dt;
        velocity.x *= drag;
        velocity.y *= drag;
        position.x += velocity.x * dt;
        position.y += velocity.y * dt;

        if (position.y + size.y > groundY) {
            isOnGround = true;
            position.y = groundY - size.y;
            velocity.y *= -0.4f;
            if (fabs(velocity.y) < 50.0f)
                velocity.y = 0;
        }
        if (isOnGround && !createdDestructionTimer) {
            destructionUpdateTime = timeElapsed;
            createdDestructionTimer = true;
        }
        if (createdDestructionTimer) {
            if (timeElapsed - destructionUpdateTime > 4.0f) {
                shouldBeDestroyed = true;
            }
        }
    }
    void drawDebugLines() {
        DrawRectangleLinesEx({position.x, position.y, size.x, size.y}, 2, RED);
        DrawCircleV(position, 5, BLUE);
    }
};

class SniperCar {
public: 
    Vector2 position;
    Vector2 size;
    Vector2 *playerPosition;
    float velocityX;
    float movingDistance;
    float distanceMoved = 0;
    float reloadCooldown;
    float lastReloadTime;
    int hp;
    
    bool shouldShoot = false;
    bool isFacingLeft;
    bool hasReachedPosition = false;
    bool shouldBeDestroyed = false;
    bool isDead = false;
    bool isSuperCar;

    std::array<Rectangle, 2> hitboxes;
    
    SniperCar(Vector2 position, bool isFacingLeft, Vector2 *playerPosition, bool isSuperCar) {
        this->isSuperCar = isSuperCar;
        this->position = position;
        this->movingDistance = GetRandomValue(300, 500);
        this->isFacingLeft = isFacingLeft;
        this->velocityX = 6.0f;
        this->size = {(float)textureSniperCar.width, (float)textureSniperCar.height};
        this->playerPosition = playerPosition;
        this->hp = isSuperCar ? 50 : 30;
        this->reloadCooldown = (isSuperCar ? (GetRandomValue(8, 12)) : (GetRandomValue(5, 10))) / 10.0f;
        this->lastReloadTime = 0;
    }

    void draw() {
        Rectangle src = {0, 0, isFacingLeft ? (float)-textureSniperCar.width : (float)textureSniperCar.width, (float)textureSniperCar.height};
        Rectangle dest = {position.x, position.y, size.x, size.y};
        DrawTexturePro(textureSniperCar, src, dest, {0, 0}, 0, isSuperCar ? GOLD : WHITE);
        float textWidth = MeasureTextEx(font, TextFormat("%i", hp), 30, 1).x;
        DrawTextEx(font, TextFormat("%i", hp), {position.x + (size.x - textWidth) / 2.0f, position.y - 30.0f}, 30.0f, 1, BLACK);
    }
    void update(float dt, float timeElapsed) {
        if (hasReachedPosition) {
            float distance = playerPosition->x - position.x;

            if (isFacingLeft) {
                if (distance > -100.0f && distance < 200) {
                    position.x += velocityX;
                }
                if (distance < -600.0f) {
                    position.x -= velocityX;
                }
            } else {
                if (distance < 200.0f && distance > -200) {
                    position.x -= velocityX;
                }
                if (distance > 600.0f) {
                    position.x += velocityX;
                }
            }
            if (timeElapsed - lastReloadTime > reloadCooldown) {
                shouldShoot = true;
                lastReloadTime = timeElapsed;
            }

        } else {
            position.x += isFacingLeft ? -velocityX : velocityX;
            distanceMoved += velocityX;

            if (distanceMoved >= movingDistance) {
                hasReachedPosition = true;
            }
        }
        hitboxes = {
            Rectangle{position.x + 70.0f, position.y, 170.0f, 44.0f}, 
            Rectangle{position.x, position.y + 45.0f, size.x, size.y - 45.0f}
        };
    }
    void drawDebugLines() {
        for (int i = 0; i < hitboxes.size(); i++) {
            DrawRectangleLinesEx(hitboxes.at(i), 2, RED);
        }
        DrawCircleV(position, 5, BLUE);
    }
};

class Bullet {
public: 
    Vector2 position;
    Vector2 size;
    Vector2 velocity;
    float angleDeg;
    
    bool isFacingLeft;
    bool shouldBeDestroyed = false;
    bool isDroneBullet;
    bool isSuperBullet;

    Bullet(Vector2 position, bool isFacingLeft, bool isDroneBullet, float angleDeg, bool isSuperBullet) {
        this->position = position;
        this->size = {(float)textureBullet.width, (float)textureBullet.height};
        this->isSuperBullet = isSuperBullet;
        this->isFacingLeft = isFacingLeft;
        this->velocity = {720, 720};
        this->isDroneBullet = isDroneBullet;
        if (isDroneBullet) {
            this->angleDeg = angleDeg;
        }
    }

    void draw() {
        Rectangle src = {0, 0, isFacingLeft ? -size.x : size.x, size.y};
        Rectangle dest = {position.x, position.y, size.x, size.y};
        DrawTexturePro(textureBullet, src, dest, {0, 0}, isDroneBullet ? angleDeg : 0, WHITE);
    }
    void update(float dt) {
        if (isDroneBullet) {
            Vector2 distanceToMove = {
                cos(angleDeg * DEG2RAD) * velocity.x * dt, 
                sin(angleDeg * DEG2RAD) * velocity.y * dt
            };
            position = Vector2Add(position, distanceToMove);
        } else {
            position.x += (isFacingLeft ? -velocity.x : velocity.y) * dt;
        }
    }
    void drawDebugLines() {
        DrawRectangleLinesEx({position.x, position.y, size.x, size.y}, 2, RED);
        DrawCircleV(position, 5, DARKBLUE);
    }
};

class Coin {
public: 
    Vector2 position;
    Vector2 size;
    float floatingAmount = 0;

    bool shouldBeDestroyed = false;

    Coin(Vector2 position) {
        this->position = position;
        this->size = {(float)textureCoin.width, (float)textureCoin.height};
    }

    void draw() {
        DrawTexture(textureCoin, position.x, position.y, WHITE);
    }
    void update(float dt) {
        floatingAmount += 0.05;
        float hover = sin(floatingAmount) * (60.0f * dt);
        position.y += hover;
    }
    void drawDebugLines() {
        DrawRectangleLinesEx({position.x, position.y, size.x, size.y}, 2, RED);
        DrawCircleV(position, 5, BLUE);
    }
};

class Drone {
public:
    Vector2 position;
    Vector2 size;
    Vector2 velocity;
    double lastDeletionUpdateTime;
    double lastShootTime = 0;
    float movedDistance = 0;
    float horizontalMovementAngleRad = 0;
    float shootingCooldown;
    float existanceTime = 15.0f;
    float aimingAngleDeg = 0;
    double currentTime = 0;

    bool shouldBeDestroyed = false;
    bool hasReachedPositon = false;
    bool shouldShoot = false;
    
    Drone(Vector2 position) {
        this->velocity = {4.0f, 8.0f};
        this->position = position;
        this->size = {(float)textureDrone.width, (float)textureDrone.height};
        this->shootingCooldown = GetRandomValue(5, 10) / 10.0f;
    }

    void draw() {
        DrawTexture(textureDrone, position.x, position.y, WHITE);
        if (hasReachedPositon) {
            float textWidth = MeasureTextEx(font, TextFormat("%i", (int)(existanceTime + 1 - (currentTime - lastDeletionUpdateTime))), 30, 1).x;
            DrawTextEx(font, TextFormat("%i", (int)(existanceTime + 1 - (currentTime - lastDeletionUpdateTime))), {position.x + (size.x - textWidth) / 2.0f, position.y - 30.0f}, 30.0f, 1, BLACK);
        }
    }
    void update(float dt, float currentTime, const std::vector<SniperCar> *sniperCars){
        this->currentTime = currentTime;
        if (!hasReachedPositon) {
            position.y += velocity.y;
            movedDistance += velocity.y;
            if (movedDistance >= 300.0f) {
                hasReachedPositon = true;
                lastDeletionUpdateTime = currentTime;
            }
        }
        
        if (hasReachedPositon) {
            horizontalMovementAngleRad += 0.05;
            float horizontalDistance = cos(horizontalMovementAngleRad) * (60.0f * dt);
            position.x += horizontalDistance;

            if (!sniperCars->empty()) {
                Vector2 distance = Vector2Subtract({sniperCars->at(0).position.x + sniperCars->at(0).size.x / 2, sniperCars->at(0).position.y + sniperCars->at(0).size.y / 2}, {position.x + size.x / 2.0f, position.y + size.y / 2.0f});
                aimingAngleDeg = atan2(distance.y, distance.x) * RAD2DEG;
            }

            if (currentTime - lastShootTime > shootingCooldown && !sniperCars->empty()) {
                shouldShoot = true;
                lastShootTime = currentTime;
            }

            if (currentTime - lastDeletionUpdateTime >= existanceTime) {
                shouldBeDestroyed = true;
            }
        }
    }
    void drawDebugLines() {
        DrawRectangleLinesEx({position.x, position.y, size.x, size.y}, 2, RED);
        DrawCircleV(position, 5, BLUE);
    }
};

class Game {
public: 
    Player player;
    std::vector<Ball> balls;
    std::vector<SniperCar> sniperCars;
    std::vector<Bullet> bullets;
    std::vector<Coin> coins;
    std::vector<Drone> drones;
    float dt;
    float timeElapsed;
    float overheatingFadeAmount;
    int numCarsToSpawn = 2;
    int gameDifficultyIndex = 0;
    int superCarSpawnChance = 0;
    
    bool shouldSpawnSniperCar = false;
    bool isDebugging = false;
    bool isPaused = false;
    bool isInTitleScreen = true;
    bool shouldPurchase = false;
    
    Rectangle startButtonBounds = {429.0f, 562.0f, 190.0f, 84.0f};
    Rectangle purchaseButtonBounds = {WINDOW_WIDTH - 270.0f, 80.0f, 150.0f, 50.0f};
    Rectangle pausePlayButtonBounds = {50.0f, WINDOW_HEIGHT - 100.0f, 50.0f, 50.0f};
    
    struct SniperCarIdentity {
        Vector2 position;
        bool isFacingLeft;
    };
    
    enum class GameDifficulty {
        PEACEFUL, 
        EASY,
        NORMAL,
        HARD,
    };
    GameDifficulty gameDifficulty = GameDifficulty::PEACEFUL;

    std::array<SniperCarIdentity, 2> sniperCarIdentities;

    Game() {
        InitAudioDevice();
        loadMedia();
        player = Player();
        timeElapsed = 0.0f;
        sniperCarIdentities = {
            SniperCarIdentity{ {-400, Player::groundY - textureSniperCar.height}, false },
            SniperCarIdentity{ {WINDOW_WIDTH + 50, Player::groundY - textureSniperCar.height}, true },
        };
        RayGuiTools::SetAllButtonBaseStyles(ColorToInt(BLACK));
        GuiSetStyle(BUTTON, BASE_COLOR_DISABLED, ColorToInt({100, 100, 100, 255}));
        RayGuiTools::SetAllButtonBorderStyles(ColorToInt(BLACK));
        RayGuiTools::SetAllButtonTextStyles(ColorToInt(WHITE));
        RayGuiTools::SetAllComboBoxBaseStyles(ColorToInt(BLACK));
        GuiSetStyle(COMBOBOX, BASE_COLOR_DISABLED, ColorToInt({100, 100, 100, 255}));
        RayGuiTools::SetAllComboBoxBorderStyles(ColorToInt(BLACK));
        RayGuiTools::SetAllComboBoxTextStyles(ColorToInt(WHITE));
        GuiSetStyle(COMBOBOX, COMBO_BUTTON_WIDTH, 60);
        GuiSetFont(font);
        GuiSetStyle(DEFAULT, TEXT_SIZE, 30);
        PlayMusicStream(musicBackground);
    }

    void draw() {
        if (isInTitleScreen) {
            DrawTexture(textureTitleScreen, 0, 0, WHITE);
            static int prevGameDifficultyIndex = 0;
            GuiComboBox({WINDOW_WIDTH - 250.0f, WINDOW_HEIGHT - 100.0f, 200.0f, 50.0f}, "Peaceful;Easy;Normal;Hard", &gameDifficultyIndex);
            gameDifficulty = static_cast<GameDifficulty>(gameDifficultyIndex);
            if (prevGameDifficultyIndex != gameDifficultyIndex) {
                PlaySound(soundSelection);
                prevGameDifficultyIndex = gameDifficultyIndex;
            }
        } else {
            DrawTexture(textureTerrain, 0, 0, WHITE);
            player.draw();
            if (isDebugging) player.drawDebugLines();
            for (auto &ball: balls) {
                ball.draw();
                if (isDebugging) ball.drawDebugLines();
            }
            for (auto &car: sniperCars) {
                car.draw();
                if (isDebugging) car.drawDebugLines();
            }
            for (auto &bullet: bullets) {
                bullet.draw();
                if (isDebugging) bullet.drawDebugLines();
            }
            for (auto &coin: coins) {
                coin.draw();
                if (isDebugging) coin.drawDebugLines();
            }
            for (auto &drone: drones) {
                drone.draw();
                if (isDebugging) drone.drawDebugLines();
            }
            if (player.isOverHeating) {
                overheatingFadeAmount += dt * 4.0f;
                float alpha = 0.5f + 0.5f * sin(overheatingFadeAmount);
                if (alpha < 0.5f) overheatingFadeAmount = 0;
                Color fadedColor = Fade(RED, Clamp(alpha, 0.5f, 1.0f));
                
                DrawTextEx(font, "Overheating!", {WINDOW_WIDTH - 330, WINDOW_HEIGHT - 100}, 50, 1, fadedColor);
            }
            DrawRectangleLinesEx({25, 25, 325, 30}, 3, BLACK);
            float progress = Clamp((player.shootingPower) / 100.0f, 0.0f, 1.0f);
            DrawRectangleGradientV(28, 28, progress * 319.0f, 24.0f, BLACK, BLUE);
            float textWidth = MeasureTextEx(font, "Shooting Power", 30, 1).x;
            DrawTextEx(font, "Shooting Power", {25 + (325.0f - textWidth) / 2.0f, 28}, 30, 1, WHITE);
    
            DrawRectangleLinesEx({400, 25, 325, 30}, 3, BLACK);
            progress = Clamp((player.hp) / player.maxHealth, 0.0f, 1.0f);
            if (gameDifficulty == GameDifficulty::PEACEFUL) {
                DrawRectangleGradientV(403, 28, progress * 319.0f, 24.0f, {50, 50, 50, 255}, {255, 100, 100, 255});
            } else {
                DrawRectangleGradientV(403, 28, progress * 319.0f, 24.0f, BLACK, RED);
            }
            textWidth = MeasureTextEx(font, "Health", 30, 1).x;
            DrawTextEx(font, "Health", {400 + (325.0f - textWidth) / 2.0f, 28}, 30, 1, WHITE);

            DrawTextEx(font, TextFormat("Coins: %i", player.coins), {WINDOW_WIDTH - 250.0f, 25.0f}, 35, 1, WHITE);

            DrawTexture(isPaused ? texturePlay : texturePause, pausePlayButtonBounds.x, pausePlayButtonBounds.y, WHITE);

            GuiDisable();
            if (player.coins >= 5) {
                GuiEnable();
            }

            shouldPurchase = GuiButton(purchaseButtonBounds, "Purchase");
            GuiEnable();
        }
    }
    void update(float dt) {
        Vector2 mousePos = GetMousePosition();
        if (isInTitleScreen) {
            if (CheckCollisionPointRec(mousePos, startButtonBounds) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                player.updateMaximumHealth(gameDifficultyIndex);
                player.hp = player.maxHealth;
                isInTitleScreen = false;
                isPaused = false;
                PlaySound(soundSelection);
            }
        } else {
            if (IsKeyPressed(KEY_F3)) {
                isDebugging = !isDebugging;
            }
            if (CheckCollisionPointRec(mousePos, pausePlayButtonBounds) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                isPaused = !isPaused;
                PlaySound(soundSelection);
            }
            if (!isPaused) {
                this->dt = dt;
                timeElapsed += dt;
                UpdateMusicStream(musicBackground);
                if (sniperCars.empty()) {
                    shouldSpawnSniperCar = true;
                }
                if (shouldSpawnSniperCar) {
                    for (int i = 0; i < numCarsToSpawn; i++) {
                        if (superCarSpawnChance >= 5) {
                            sniperCars.push_back(SniperCar(sniperCarIdentities.at(i).position, sniperCarIdentities.at(i).isFacingLeft, &player.position, true));
                            superCarSpawnChance = 0;
                        } else {
                            sniperCars.push_back(SniperCar(sniperCarIdentities.at(i).position, sniperCarIdentities.at(i).isFacingLeft, &player.position, false));
                        }
                    }
                    bullets.clear();
                    balls.clear();
                    player.hp = player.maxHealth;
                    shouldSpawnSniperCar = false;
                    superCarSpawnChance++;
                }
                if (player.hp <= 0) {
                    PlaySound(soundFail);
                    reset();
                }
                if (player.shouldShoot) {
                    Vector2 spawnPos = {
                        player.position.x + player.turretSize.x / 2 + cos(player.turretAngleDeg * DEG2RAD) * player.turretSize.x / 2,
                        player.position.y - 50.0f + player.turretSize.y / 2 + sin(player.turretAngleDeg * DEG2RAD) * player.turretSize.x / 2
                    };
                    balls.push_back(Ball(spawnPos, player.turretAngleDeg, player.shootingPower));
                    player.shootingPower = 0;
                    player.shouldShoot = false;
                }
                if (shouldPurchase) {
                    player.coins -= 5;
                    Vector2 spawnPos = {(float)GetRandomValue(100, WINDOW_WIDTH - 100), -100.0f};
                    drones.push_back(Drone(spawnPos));
                    PlaySound(soundKaching);
                }
                if (IsKeyPressed(KEY_PERIOD)) {
                    reset();
                }
        
                for (auto &car: sniperCars) {
                    car.update(dt, timeElapsed);
                    if (car.shouldShoot) {
                        Vector2 spawnPos = {car.position.x + (car.isFacingLeft ? 80 - textureBullet.width : 240), car.position.y + 11.0f};
                        bullets.push_back(Bullet(spawnPos, car.isFacingLeft, false, 0, car.isSuperCar));
                        car.shouldShoot = false;
                        PlaySound(soundGunshot);
                    }
                }
        
                for (auto &bullet: bullets) {
                    bullet.update(dt);
                }
        
                player.update(dt, timeElapsed);
                for (auto &ball: balls) {
                    ball.update(dt, timeElapsed);
                }
                for (auto &coin: coins) {
                    coin.update(dt);
                }
                for (auto &drone: drones) {
                    drone.update(dt, timeElapsed, &sniperCars);
                    if (drone.shouldShoot) {
                        Vector2 spawnPos = {drone.position.x + drone.size.x / 2, drone.position.y + drone.size.y / 2};
                        bullets.push_back(Bullet(spawnPos, false, true, drone.aimingAngleDeg, false));
                        drone.shouldShoot = false;
                        PlaySound(soundGunshot);
                    }
                }
                garbageCollect();
                checkForRemoval();
                checkForCollisions();
            }
        }
    }
    void garbageCollect() {
        for (int i = 0; i < balls.size(); i++) {
            if (balls.at(i).shouldBeDestroyed) {
                balls.erase(balls.begin() + i);
                i--;
            }
        }
        for (int i = 0; i < bullets.size(); i++) {
            if (bullets.at(i).shouldBeDestroyed) {
                bullets.erase(bullets.begin() + i);
                i--;
            }
        }
        for (int i = 0; i < sniperCars.size(); i++) {
            if (sniperCars.at(i).isDead) {
                sniperCars.erase(sniperCars.begin() + i);
                i--;
            }
        }
        for (int i = 0; i < coins.size(); i++) {
            if (coins.at(i).shouldBeDestroyed) {
                coins.erase(coins.begin() + i);
                i--;
            }
        }
        for (int i = 0; i < drones.size(); i++) {
            if (drones.at(i).shouldBeDestroyed) {
                drones.erase(drones.begin() + i);
                i--;
            }
        }
    }
    void checkForRemoval() {
        for (auto &bullet: bullets) {
            if (bullet.position.x < -50 || bullet.position.x + bullet.size.x > WINDOW_WIDTH + 50.0f || bullet.position.y < -50 || bullet.position.y + bullet.size.y > WINDOW_HEIGHT + 50.0f ) {
                bullet.shouldBeDestroyed = true;
            }
        }
    }
    void checkForCollisions() {
        for (auto &car: sniperCars) {
            for (auto hitbox: car.hitboxes) {
                for (auto &ball: balls) {
                    if (CheckCollisionRecs({hitbox.x, hitbox.y, hitbox.width, hitbox.height}, {ball.position.x, ball.position.y, ball.size.x, ball.size.y})) {
                        float damage = ball.firePower / 1000.0f * 10.0f;
                        car.hp -= damage;
                        if (car.hp <= 0) {
                            Vector2 spawnPos = {car.position.x + 150.0f, Player::groundY - 100.0f};
                            coins.push_back(Coin(spawnPos));
                            if (car.isSuperCar) {
                                coins.push_back(Coin({spawnPos.x - 70, spawnPos.y}));
                                coins.push_back(Coin({spawnPos.x + 70, spawnPos.y}));
                            }
                            car.hp = 0;
                            car.isDead = true;
                        }
                        PlaySound(soundCarHit);
                        ball.shouldBeDestroyed = true;
                    }
                }
                for (auto &bullet: bullets) {
                    if (CheckCollisionRecs({bullet.position.x, bullet.position.y, bullet.size.x, bullet.size.y}, {hitbox.x, hitbox.y, hitbox.width, hitbox.height}) && bullet.isDroneBullet) {
                        float damage = GetRandomValue(3, 5);
                        car.hp -= damage;
                        if (car.hp <= 0) {
                            Vector2 spawnPos = {car.position.x + 150.0f, Player::groundY - 100.0f};
                            coins.push_back(Coin(spawnPos));
                            if (car.isSuperCar) {
                                coins.push_back(Coin({spawnPos.x - 70, spawnPos.y}));
                                coins.push_back(Coin({spawnPos.x + 70, spawnPos.y}));
                            }
                            car.hp = 0;
                            car.isDead = true;
                        }
                        bullet.shouldBeDestroyed = true;
                    }
                }
            }
        }
        for (auto &bullet: bullets) {
            if (Collision::CheckCollisionRectCornersRec({bullet.position.x, bullet.position.y, bullet.size.x, bullet.size.y}, player.rectCorners) && !bullet.isDroneBullet) {
                float damage = bullet.isSuperBullet ? GetRandomValue(10, 15) : GetRandomValue(3, 5);
                if (gameDifficulty != GameDifficulty::PEACEFUL) player.hp -= damage;
                bullet.shouldBeDestroyed = true;
            }
        }
        for (auto &coin: coins) {
            if (Collision::CheckCollisionRectCornersRec({coin.position.x, coin.position.y, coin.size.x, coin.size.y}, player.rectCorners)) {
                coin.shouldBeDestroyed = true;
                player.coins++;
                PlaySound(soundCollect);
            }
        }
    }
    void reset() {
        isInTitleScreen = true;
        player.hp = player.maxHealth;
        player.shootingPower = 0;
        bullets.clear();
        sniperCars.clear();
        balls.clear();
        coins.clear();
        drones.clear();
        player.shootingPower = 0;
        player.coins = 0;
        player.position = {WINDOW_WIDTH / 2 - 100.0f, Player::groundY - 100.0f};
    }
};

int main() {
    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Cannon Clash");
    SetTargetFPS(60);

    Game game;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        float dt = GetFrameTime();
        game.update(dt);
        game.draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}