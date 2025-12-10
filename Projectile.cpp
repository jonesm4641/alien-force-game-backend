#include "Projectile.hpp"
#include <cmath>

static inline sf::Vector2f normalize(sf::Vector2f v) {
    const float len = std::sqrt(v.x * v.x + v.y * v.y);
    return (len > 0.0001f) ? sf::Vector2f{ v.x / len, v.y / len } : sf::Vector2f{ 0.f, 0.f };
}

Projectile::Projectile(float speed, float radius)
    : m_shape(radius)
    , m_speed(speed)
    , m_radius(radius) {
    m_shape.setOrigin(sf::Vector2f{ m_radius, m_radius });
    m_shape.setFillColor(sf::Color(255, 220, 80));
    m_shape.setOutlineThickness(1.f);
    m_shape.setOutlineColor(sf::Color(255, 255, 160));
    alive = false;
}

void Projectile::fire(sf::Vector2f start, sf::Vector2f forward) {
    const sf::Vector2f dir = normalize(forward);
    m_velocity = sf::Vector2f{ dir.x * m_speed, dir.y * m_speed };
    m_shape.setPosition(start);

    const float angleRad = std::atan2(dir.y, dir.x);
    m_shape.setRotation(sf::degrees(angleRad * 180.f / 3.14159265f));
    alive = true;
}

void Projectile::update(float dt) {
    if (!alive) return;
    const sf::Vector2f pos = m_shape.getPosition();
    m_shape.setPosition(sf::Vector2f{ pos.x + m_velocity.x * dt, pos.y + m_velocity.y * dt });
}

void Projectile::draw(sf::RenderTarget& target) const {
    if (!alive) return;
    target.draw(m_shape);
}

// SFML 3: sf::FloatRect uses .position (Vector2f) and .size (Vector2f)
bool Projectile::outOf(const sf::FloatRect& rect) const {
    const sf::Vector2f p = m_shape.getPosition();
    const sf::Vector2f rpos = rect.position;
    const sf::Vector2f rsz = rect.size;
    return !(p.x >= rpos.x &&
        p.y >= rpos.y &&
        p.x <= rpos.x + rsz.x &&
        p.y <= rpos.y + rsz.y);
}

sf::Vector2f Projectile::getPosition() const {
    return m_shape.getPosition();
}

sf::FloatRect Projectile::getBounds() const {
    return m_shape.getGlobalBounds();
}
