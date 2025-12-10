#pragma once
#include <SFML/Graphics.hpp>
#include <memory>

class Enemy {
public:
    // If you have a texture to share across enemies, pass a shared_ptr to it (can be null)
    explicit Enemy(const std::shared_ptr<sf::Texture>& tex,
        sf::Vector2f spawnPos,
        float speed = 80.f,
        float radius = 16.f);

    void update(float dt, sf::Vector2f playerPos);
    void draw(sf::RenderTarget& target) const;

    bool isAlive() const { return m_alive; }
    void kill() { m_alive = false; }

    sf::Vector2f position() const { return m_position; }
    float radius() const { return m_radius; }

private:
    // Render as sprite if we have a texture, otherwise as a circle shape
    std::shared_ptr<sf::Texture> m_texture;
    std::unique_ptr<sf::Sprite>  m_sprite;   // SFML 3: construct with texture at creation
    sf::CircleShape              m_shape;    // fallback

    sf::Vector2f m_position{};
    sf::Vector2f m_velocity{};
    float        m_speed{ 80.f };
    float        m_radius{ 16.f };
    bool         m_alive{ true };
};
