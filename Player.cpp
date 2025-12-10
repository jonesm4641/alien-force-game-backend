#include "Player.hpp"
void Player::draw(sf::RenderWindow& window) const {
    window.draw(body);
}
#include <cmath> // for sqrt, atan2, cos, sin
#include <SFML/System/Angle.hpp> // for sf::degrees (usually pulled by Graphics.hpp already)

Player::Player() {
    body.setSize(sf::Vector2f{ 40.f, 40.f });          // setSize(Vector2f)
    body.setOrigin(sf::Vector2f{ 20.f, 20.f });   // was: setOrigin(20.f, 20.f)
    body.setFillColor(sf::Color(90, 200, 255));
    body.setPosition(sf::Vector2f{ 400.f, 300.f });    // setPosition(Vector2f)
}

void Player::handleInput(float dt) {
    sf::Vector2f move{ 0.f, 0.f };

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) move.x -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) move.x += 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) move.y -= 1.f;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) move.y += 1.f;

    if (move.x != 0.f || move.y != 0.f) {
        const float len = std::sqrt(move.x * move.x + move.y * move.y);
        move /= len;
        body.move(move * speed * dt);
    }
}

void Player::update(float /*dt*/, const sf::RenderWindow& window) {
    // face mouse
    const sf::Vector2f mouse = sf::Vector2f(sf::Mouse::getPosition(window));
    const sf::Vector2f pos = body.getPosition();
    const sf::Vector2f dir = mouse - pos;

    // Compute degrees, then pass as sf::Angle
    const float angleDeg = std::atan2(dir.y, dir.x) * 180.f / 3.1415926535f;
    body.setRotation(sf::degrees(angleDeg + 90.f));  // setRotation(sf::Angle)
}

sf::Vector2f Player::getMuzzle() const {
    // body.getRotation() returns sf::Angle in SFML 3
    const sf::Angle rot = body.getRotation();
    const sf::Angle fwd = rot - sf::degrees(90.f);   // subtract Angle, not float
    const float rad = fwd.asRadians();

    const sf::Vector2f forward{ std::cos(rad), std::sin(rad) };
    return body.getPosition() + forward * 28.f;
}

sf::Vector2f Player::getForward() const {
    const sf::Angle rot = body.getRotation();
    const sf::Angle fwd = rot - sf::degrees(90.f);
    const float rad = fwd.asRadians();

    return { std::cos(rad), std::sin(rad) };
}
