#include "Enemy.hpp"
#include <cmath>

static inline sf::Vector2f normalize(sf::Vector2f v) {
    float len = std::sqrt(v.x * v.x + v.y * v.y);
    return (len > 0.0001f) ? sf::Vector2f{ v.x / len, v.y / len } : sf::Vector2f{ 0.f, 0.f };
}

Enemy::Enemy(const std::shared_ptr<sf::Texture>& tex,
    sf::Vector2f spawnPos,
    float speed,
    float radius)
    : m_texture(tex),
    m_position(spawnPos),
    m_speed(speed),
    m_radius(radius) {

    if (m_texture) {
        m_sprite = std::make_unique<sf::Sprite>(*m_texture); // SFML 3: texture required at construction
        // center origin based on texture size
        auto sz = m_texture->getSize();
        m_sprite->setOrigin(sf::Vector2f{ static_cast<float>(sz.x) * 0.5f,
                                          static_cast<float>(sz.y) * 0.5f });
        m_sprite->setPosition(m_position);
        m_sprite->setScale(sf::Vector2f{ (m_radius * 2.f) / static_cast<float>(sz.x),
                                         (m_radius * 2.f) / static_cast<float>(sz.y) });
    }
    else {
        m_shape = sf::CircleShape(m_radius);
        m_shape.setOrigin(sf::Vector2f{ m_radius, m_radius });
        m_shape.setFillColor(sf::Color(200, 60, 60));
        m_shape.setOutlineThickness(2.f);
        m_shape.setOutlineColor(sf::Color(255, 180, 180));
        m_shape.setPosition(m_position);
    }
}

void Enemy::update(float dt, sf::Vector2f playerPos) {
    if (!m_alive) return;

    // Drift toward the player
    sf::Vector2f dir = normalize(playerPos - m_position);
    m_velocity = sf::Vector2f{ dir.x * m_speed, dir.y * m_speed };
    m_position += sf::Vector2f{ m_velocity.x * dt, m_velocity.y * dt };

    if (m_sprite) {
        m_sprite->setPosition(m_position);
        // face the player (optional)
        float angleRad = std::atan2(dir.y, dir.x);
        m_sprite->setRotation(sf::degrees(angleRad * 180.f / 3.14159265f));
    }
    else {
        m_shape.setPosition(m_position);
        // simple eye-candy: rotate a bit
        m_shape.setRotation(sf::degrees(m_shape.getRotation().asDegrees() + 90.f * dt));
    }
}

void Enemy::draw(sf::RenderTarget& target) const {
    if (!m_alive) return;
    if (m_sprite) {
        target.draw(*m_sprite);
    }
    else {
        target.draw(m_shape);
    }
}
