#pragma once
#include <SFML/Graphics.hpp>

class Projectile {
public:
    explicit Projectile(float speed = 600.f, float radius = 3.f);

    void fire(sf::Vector2f start, sf::Vector2f forward);
    void update(float dt);
    void draw(sf::RenderTarget& target) const;

    // SFML 3 Rect API: rect.position / rect.size
    bool outOf(const sf::FloatRect& rect) const;

    // used by collision code
    sf::Vector2f getPosition() const;
    sf::FloatRect getBounds() const;

    bool alive{ false };

private:
    sf::CircleShape m_shape;
    sf::Vector2f    m_velocity{ 0.f, 0.f };
    float           m_speed{ 600.f };
    float           m_radius{ 3.f };
};
