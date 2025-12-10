// ============================================================================
//  AlienForceClient.cpp — Demo with Menu/Arena/Results + HUD/Lives + MySQL save
//  Build: C++20, VS2022 v143, SFML 3.0.2 (x64)
//  Link:  sfml-graphics.lib; sfml-window.lib; sfml-system.lib
// ============================================================================

#include <SFML/Graphics.hpp>
#include <iostream>
#include <filesystem>
#include <memory>
#include <vector>
#include <fstream>
#include <string>
#include <random>
#include <cmath>
#include <algorithm>

// ---- Project headers ----
#include "Player.hpp"
#include "Projectile.hpp"
#include "Enemy.hpp"
#include "Db.hpp"   // << DB module

// ============================================================================
// ======================= Game state & constants =============================
// ============================================================================
enum class GameState { Menu, Arena, Results };

static constexpr int   START_LIVES = 3;
static constexpr float INVULN_TIME_SEC = 1.0f;   // after player is hit
static constexpr float HURT_FLASH_TIME_SEC = 0.15f;  // white flash overlay
static constexpr float PROJ_RADIUS = 3.0f;   // projectile hit circle
// ============================================================================


// ============================================================================
// ======================= Enemy (B): state ===================================
// ============================================================================
std::vector<Enemy> g_enemies;

// Optional shared texture for all enemies (keeps VRAM copies low)
std::shared_ptr<sf::Texture> g_enemyTexture;

// Spawn timing
float g_enemySpawnAccumulator = 0.f;
float g_enemySpawnEvery = 1.75f; // seconds
// ============================================================================


// ============================================================================
// ==================== Enemy (D): helpers ====================================
// ============================================================================
static sf::Vector2f randomSpawnOnEdge(std::mt19937& rng, const sf::Vector2u winSize) {
    std::uniform_int_distribution<int>    edgeDist(0, 3);
    std::uniform_real_distribution<float> xDist(0.f, static_cast<float>(winSize.x));
    std::uniform_real_distribution<float> yDist(0.f, static_cast<float>(winSize.y));

    int edge = edgeDist(rng); // 0=top,1=right,2=bottom,3=left
    switch (edge) {
    case 0: return sf::Vector2f{ xDist(rng), -40.f };                         // top (offscreen)
    case 1: return sf::Vector2f{ static_cast<float>(winSize.x) + 40.f, yDist(rng) }; // right
    case 2: return sf::Vector2f{ xDist(rng), static_cast<float>(winSize.y) + 40.f }; // bottom
    default:return sf::Vector2f{ -40.f, yDist(rng) };                          // left
    }
}

static void spawnEnemy(std::vector<Enemy>& enemies,
    const std::shared_ptr<sf::Texture>& enemyTex,
    const sf::Vector2u winSize,
    std::mt19937& rng) {
    auto pos = randomSpawnOnEdge(rng, winSize);
    enemies.emplace_back(enemyTex, pos, /*speed*/ 80.f, /*radius*/ 16.f);
}
// ============================================================================


// ============================================================================
// ==================== Utility: robust file loaders ==========================
// ============================================================================
static bool loadTextureFromFileBytes(const std::filesystem::path& absPath,
    std::unique_ptr<sf::Texture>& outTex) {
    std::ifstream in(absPath, std::ios::binary);
    if (!in) return false;
    std::vector<unsigned char> bytes((std::istreambuf_iterator<char>(in)), {});
    if (bytes.empty()) return false;

    auto tex = std::make_unique<sf::Texture>();
    if (!tex->loadFromMemory(bytes.data(), bytes.size())) return false;

    outTex = std::move(tex);
    return true;
}

static bool tryLoadBackground(const std::filesystem::path& rel,
    sf::RenderWindow& window,
    std::unique_ptr<sf::Texture>& bgTexture,
    std::unique_ptr<sf::Sprite>& background) {
    const auto abs = std::filesystem::absolute(rel);
    std::cout << "Try: " << abs.string()
        << "  exists? " << (std::filesystem::exists(abs) ? "YES" : "NO") << "\n";
    if (!std::filesystem::exists(abs)) return false;
    if (!loadTextureFromFileBytes(abs, bgTexture)) {
        std::cout << "SFML failed to decode: " << abs.filename().string() << "\n";
        return false;
    }
    // Build sprite + scale to window
    background = std::make_unique<sf::Sprite>(*bgTexture);
    background->setPosition(sf::Vector2f{ 0.f, 0.f });
    const auto win = window.getSize();
    const auto ts = bgTexture->getSize();
    if (ts.x > 0 && ts.y > 0) {
        background->setScale(sf::Vector2f{
            static_cast<float>(win.x) / static_cast<float>(ts.x),
            static_cast<float>(win.y) / static_cast<float>(ts.y)
            });
    }
    std::cout << "Loaded background: " << abs.filename().string()
        << "  size: " << ts.x << "x" << ts.y << "\n";
    return true;
}

static bool robustLoadFont(sf::Font& font) {
    // SFML 3: use openFromFile (not loadFromFile)
    const char* candidates[] = {
        "Assets/Orbitron.ttf",
        "Assets/RobotoMono.ttf",
        "Assets/Roboto-Regular.ttf",
        "Assets/DejaVuSans.ttf",
        "Assets/font.ttf"
    };
    for (auto* p : candidates) {
        const auto abs = std::filesystem::absolute(p);
        if (font.openFromFile(abs.string())) {
            std::cout << "[INFO] HUD font loaded: " << abs.string() << "\n";
            return true;
        }
    }
    std::cout << "[WARN] No HUD font found in Assets/. HUD will be minimal.\n";
    return false;
}
// ============================================================================


// ============================================================================
// ================================ main() ====================================
// ============================================================================
int main() {
    // -------------------- Window & basics -----------------------------------
    sf::RenderWindow window(sf::VideoMode{ sf::Vector2u{960u, 540u} }, "AlienForceClient");
    window.setFramerateLimit(120);
    // window.setVerticalSyncEnabled(true); // optional
    std::cout << "CWD: " << std::filesystem::current_path().string() << "\n";

    // -------------------- Background loader ---------------------------------
    std::unique_ptr<sf::Texture> bgTexture;
    std::unique_ptr<sf::Sprite>  background;

    bool bgOK = false;
    for (const char* name : { "space_bg.png","space_bg.jpg","space_bg.jpeg","space_bg.bmp" }) {
        if (bgOK) break;
        bgOK = tryLoadBackground(name, window, bgTexture, background);
    }
    for (const char* name : { "Assets/space_bg.png","Assets/space_bg.jpg","Assets/space_bg.jpeg","Assets/space_bg.bmp" }) {
        if (bgOK) break;
        bgOK = tryLoadBackground(name, window, bgTexture, background);
    }
    if (!bgOK) {
        std::cout << "[ERROR] Could not load space_bg.(png|jpg|jpeg|bmp) from CWD or Assets.\n";
    }

    // -------------------- Enemy (C): load texture ---------------------------
    g_enemyTexture = std::make_shared<sf::Texture>();
    bool enemyTexLoaded = false;
    for (auto path : {
        std::string{"enemy.png"},
        std::string{"Assets/enemy.png"},
        std::string{"Assets/textures/enemy.png"}
        }) {
        if (g_enemyTexture->loadFromFile(path)) { enemyTexLoaded = true; break; }
    }
    if (!enemyTexLoaded) {
        g_enemyTexture.reset(); // shape fallback
    }

    // -------------------- Game objects --------------------------------------
    Player player;
    std::vector<Projectile> shots;
    shots.reserve(128);

    // -------------------- HUD (Score/Lives/State text) ----------------------
    int score = 0;
    int lives = START_LIVES;
    float invulnTimer = 0.f;       // player invulnerability after being hit
    float hurtFlashTimer = 0.f;    // brief white flash overlay

    sf::Font hudFont;
    const bool hudOk = robustLoadFont(hudFont);

    std::unique_ptr<sf::Text> scoreText;
    std::unique_ptr<sf::Text> livesText;
    std::unique_ptr<sf::Text> infoText;

    if (hudOk) {
        scoreText = std::make_unique<sf::Text>(hudFont);
        scoreText->setString("Score: 0");
        scoreText->setCharacterSize(22);
        scoreText->setFillColor(sf::Color::White);
        scoreText->setOutlineColor(sf::Color(0, 0, 0));
        scoreText->setOutlineThickness(2.f);
        scoreText->setPosition(sf::Vector2f{ 12.f, 8.f });

        livesText = std::make_unique<sf::Text>(hudFont);
        livesText->setString("Lives: 3");
        livesText->setCharacterSize(22);
        livesText->setFillColor(sf::Color::White);
        livesText->setOutlineColor(sf::Color(0, 0, 0));
        livesText->setOutlineThickness(2.f);
        livesText->setPosition(sf::Vector2f{ 12.f, 36.f });

        infoText = std::make_unique<sf::Text>(hudFont);
        infoText->setString("");
        infoText->setCharacterSize(26);
        infoText->setFillColor(sf::Color::White);
        infoText->setOutlineColor(sf::Color(0, 0, 0));
        infoText->setOutlineThickness(3.f);
    }

    // -------------------- DB: connect once ----------------------------------
    if (auto err = db::connect_from_cfg("Assets/db.cfg"); !err.empty()) {
        std::cout << "[DB] connect failed: " << err << "\n";
    }
    else {
        std::cout << "[DB] connected.\n";
    }

    // -------------------- State & timing ------------------------------------
    GameState state = GameState::Menu;   // With font present we show a Menu
    sf::Clock  clock;
    float shootCooldown = 0.f;
    static std::mt19937 rng{ std::random_device{}() };
    bool paused = false;
    static bool savedThisRun = false;

    // -------------------- Helpers (inline lambdas) --------------------------
    auto circleHit = [](sf::Vector2f a, float ra, sf::Vector2f b, float rb) {
        float dx = a.x - b.x, dy = a.y - b.y;
        float r = ra + rb;
        return (dx * dx + dy * dy) <= (r * r);
        };

    auto resetRun = [&]() {
        score = 0;
        lives = START_LIVES;
        invulnTimer = 0.f;
        hurtFlashTimer = 0.f;
        g_enemies.clear();
        shots.clear();
        g_enemySpawnAccumulator = 0.f;
        savedThisRun = false; // allow saving at next game over
        if (hudOk) {
            scoreText->setString("Score: 0");
            livesText->setString("Lives: " + std::to_string(lives));
        }
        };

    // -------------------- Main loop -----------------------------------------
    while (window.isOpen()) {
        // ======================= Event pump =================================
        while (auto ev = window.pollEvent()) {
            if (ev->is<sf::Event::Closed>()) window.close();
            if (ev->is<sf::Event::FocusLost>() && state == GameState::Arena) {
                paused = true;
            }


            if (auto key = ev->getIf<sf::Event::KeyPressed>()) {
                const auto k = key->code;
                if (k == sf::Keyboard::Key::Escape) {
                    if (state == GameState::Arena) paused = !paused;
                    else if (state == GameState::Menu) window.close();
                }
                if (k == sf::Keyboard::Key::Enter) {
                    if (state == GameState::Menu) { resetRun(); state = GameState::Arena; }
                    else if (state == GameState::Results) { resetRun(); state = GameState::Arena; }
                }
                if (k == sf::Keyboard::Key::R && state == GameState::Arena) {
                    resetRun();
                }
            }
        }

        // ======================= Frame timing ===============================
        const float dt = clock.restart().asSeconds();
        const float dtc = std::clamp(dt, 0.f, 0.033f); // clamp to ~30fps step max

        // Debounce used by Menu fallback input
        static float menuInputCooldown = 0.f;
        if (menuInputCooldown > 0.f) menuInputCooldown -= dtc;

        const auto sz = window.getSize();

        // ======================= MENU STATE =================================
        if (state == GameState::Menu) {
            window.clear();

            if (background) { window.draw(*background); }
            else {
                sf::VertexArray grad(sf::PrimitiveType::TriangleStrip, 4);
                grad[0].position = { 0.f, 0.f };
                grad[1].position = { static_cast<float>(sz.x), 0.f };
                grad[2].position = { 0.f, static_cast<float>(sz.y) };
                grad[3].position = { static_cast<float>(sz.x), static_cast<float>(sz.y) };
                grad[0].color = sf::Color(10, 10, 40);
                grad[1].color = sf::Color(10, 10, 40);
                grad[2].color = sf::Color(30, 30, 100);
                grad[3].color = sf::Color(30, 30, 100);
                window.draw(grad);
            }

            if (hudOk) {
                infoText->setString("ALIEN FORCE — DEMO\n\nEnter: Play   Esc: Quit\n\nWASD to move  •  Space/Click to shoot");
                auto gb = infoText->getGlobalBounds();
                infoText->setPosition(sf::Vector2f{
                    (static_cast<float>(sz.x) - gb.size.x) / 2.f,
                    (static_cast<float>(sz.y) - gb.size.y) / 2.f
                    });
                window.draw(*infoText);
            }

            // --- Fallback start (works even if KeyPressed is missed) ---
            if (menuInputCooldown <= 0.f &&
                (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter) ||
                    sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) ||
                    sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))) {
                resetRun();
                state = GameState::Arena;
                paused = false;
                menuInputCooldown = 0.25f;
                std::cout << "[INFO] Menu start via fallback input.\n";
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) window.close();

            window.display();
            continue;
        }

        // ======================= RESULTS STATE ================================
        if (state == GameState::Results) {
            window.clear();
            if (background) window.draw(*background); else {
                sf::VertexArray grad(sf::PrimitiveType::TriangleStrip, 4);
                grad[0].position = { 0.f, 0.f };
                grad[1].position = { static_cast<float>(sz.x), 0.f };
                grad[2].position = { 0.f, static_cast<float>(sz.y) };
                grad[3].position = { static_cast<float>(sz.x), static_cast<float>(sz.y) };
                grad[0].color = sf::Color(10, 10, 40);
                grad[1].color = sf::Color(10, 10, 40);
                grad[2].color = sf::Color(30, 30, 100);
                grad[3].color = sf::Color(30, 30, 100);
                window.draw(grad);
            }
            if (hudOk) {
                // Optional: show Top 5
                std::string board = "GAME OVER\n\nScore: " + std::to_string(score) + "\n\n";
                auto rows = db::top_scores(5, "demo");
                if (!rows.empty()) {
                    board += "TOP 5\n";
                    int r = 1;
                    for (auto& row : rows) {
                        board += std::to_string(r++) + ". " + row.name + "  " + std::to_string(row.score) + "  (" + row.when + ")\n";
                    }
                    board += "\n";
                }
                board += "Enter: Play Again   Esc: Close";
                infoText->setString(board);
                auto gb = infoText->getGlobalBounds();
                infoText->setPosition(sf::Vector2f{
                    (static_cast<float>(sz.x) - gb.size.x) / 2.f,
                    (static_cast<float>(sz.y) - gb.size.y) / 2.f
                    });
                window.draw(*infoText);
            }
            window.display();
            continue;
        }

        // ======================= ARENA GAMEPLAY ================================
        if (!paused) {
            // --- Timers ---
            shootCooldown -= dtc;
            if (invulnTimer > 0.f)    invulnTimer -= dtc;
            if (hurtFlashTimer > 0.f) hurtFlashTimer -= dtc;

            // --- Player input + update ---
            player.handleInput(dtc);
            player.update(dtc, window);

            // --- Fire control ---
            const bool fireKey = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);
            const bool fireMouse = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
            if (fireKey || fireMouse) {
                if (shootCooldown <= 0.f) {
                    bool reused = false;
                    for (auto& p : shots) {
                        if (!p.alive) { p.fire(player.getMuzzle(), player.getForward()); reused = true; break; }
                    }
                    if (!reused) {
                        Projectile p; p.fire(player.getMuzzle(), player.getForward()); shots.push_back(p);
                    }
                    shootCooldown = 0.12f;
                }
            }

            // --- Projectiles update + cull out-of-bounds ---
            for (auto& p : shots) p.update(dtc);
            const sf::FloatRect bounds{
                sf::Vector2f{0.f, 0.f},
                sf::Vector2f{ static_cast<float>(sz.x), static_cast<float>(sz.y) }
            };
            for (auto& p : shots) if (p.outOf(bounds)) p.alive = false;

            // --- Enemies: spawn + update toward player ---
            const sf::Vector2f playerPosForAI = player.getMuzzle();
            g_enemySpawnAccumulator += dtc;
            if (g_enemySpawnAccumulator >= g_enemySpawnEvery) {
                g_enemySpawnAccumulator = 0.f;
                spawnEnemy(g_enemies, g_enemyTexture, window.getSize(), rng);
            }
            for (auto& e : g_enemies) {
                e.update(dtc, playerPosForAI);
            }

            // --- Collision: projectiles vs enemies ---
            for (auto& e : g_enemies) {
                if (!e.isAlive()) continue;
                for (auto& p : shots) {
                    if (!p.alive) continue;
                    const sf::Vector2f projPos = p.getPosition();
                    if (circleHit(projPos, PROJ_RADIUS, e.position(), e.radius())) {
                        p.alive = false;
                        e.kill();
                        score += 10;
                        if (hudOk) scoreText->setString("Score: " + std::to_string(score));
                        break;
                    }
                }
            }

            // --- Collision: enemies vs player (approx via muzzle position) ---
            constexpr float PLAYER_RADIUS = 18.f;
            if (invulnTimer <= 0.f) {
                for (auto& e : g_enemies) {
                    if (!e.isAlive()) continue;
                    const float dx = playerPosForAI.x - e.position().x;
                    const float dy = playerPosForAI.y - e.position().y;
                    const float R = PLAYER_RADIUS + e.radius();
                    if (dx * dx + dy * dy <= R * R) {
                        lives -= 1;
                        if (hudOk) livesText->setString("Lives: " + std::to_string(lives));
                        invulnTimer = INVULN_TIME_SEC;
                        hurtFlashTimer = HURT_FLASH_TIME_SEC;
                        e.kill();
                        break;
                    }
                }
            }

            // --- GC: remove dead/expired entities ---
            g_enemies.erase(
                std::remove_if(g_enemies.begin(), g_enemies.end(),
                    [](const Enemy& e) { return !e.isAlive(); }),
                g_enemies.end()
            );
            shots.erase(
                std::remove_if(shots.begin(), shots.end(),
                    [](const Projectile& p) { return !p.alive; }),
                shots.end()
            );

            // --- Check game over ---
            if (lives <= 0) {
                state = GameState::Results;

                // DB: save score exactly once per run
                if (!savedThisRun) {
                    if (auto err = db::upsert_player_and_add_score("TestPilot", score); !err.empty()) {
                        std::cout << "[DB] save failed: " << err << "\n";
                    }
                    else {
                        std::cout << "[DB] score saved.\n";
                    }
                    savedThisRun = true;
                }
            }
        }

        // ========================= Render ===================================
        window.clear();

        // ---- Background (image or gradient fallback)
        if (background) {
            window.draw(*background);
        }
        else {
            sf::VertexArray grad(sf::PrimitiveType::TriangleStrip, 4);
            grad[0].position = { 0.f, 0.f };
            grad[1].position = { static_cast<float>(sz.x), 0.f };
            grad[2].position = { 0.f, static_cast<float>(sz.y) };
            grad[3].position = { static_cast<float>(sz.x), static_cast<float>(sz.y) };
            grad[0].color = sf::Color(10, 10, 40);
            grad[1].color = sf::Color(10, 10, 40);
            grad[2].color = sf::Color(30, 30, 100);
            grad[3].color = sf::Color(30, 30, 100);
            window.draw(grad);
        }

        // ---- Player & projectiles
        player.draw(window);
        for (auto& p : shots) p.draw(window);

        // ===================== Enemy (F): draw ===============================
        for (const auto& e : g_enemies) {
            e.draw(window);
        }

        // ===================== HUD: score/lives/pause ========================
        if (hudOk) {
            window.draw(*scoreText);
            window.draw(*livesText);

            if (paused) {
                infoText->setString("PAUSED\n\nEsc: Resume   R: Reset");
                auto gb = infoText->getGlobalBounds();
                infoText->setPosition(sf::Vector2f{
                    (static_cast<float>(sz.x) - gb.size.x) / 2.f,
                    (static_cast<float>(sz.y) - gb.size.y) / 2.f
                    });
                window.draw(*infoText);
            }
        }

        // ===================== Hurt flash overlay ============================
        if (hurtFlashTimer > 0.f) {
            sf::RectangleShape flash(sf::Vector2f{ static_cast<float>(sz.x), static_cast<float>(sz.y) });
            flash.setFillColor(sf::Color(255, 255, 255, 120));
            window.draw(flash);
        }

        window.display();
    }

    return 0;
}
