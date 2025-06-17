#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sstream>

int main() {
    std::vector<std::vector<int>> labyrinth = {
        { 0, 1, 0, 1, 1, 0, 1, 1, 0, 1, 1, 0, 1, 0, 0, 1 },
        { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 0, 1 },
        { 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0 },
        { 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 0 },
        { 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0 },
        { 1, 0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 0 },
        { 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 1, 0 },
        { 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0 },
        { 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0 },
        { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0 },
        { 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0 },
        { 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
    };
    std::vector < sf::Sprite > sprites;
    sf::RenderWindow window(sf::VideoMode(800, 600), "My window");
    //set player speed
    float playerSpeed = 0.1;
    //creating textures
    sf::Texture mineT, grassT, wallT, guyT, flagT, boomT;
    mineT.loadFromFile("mine.png");
    grassT.loadFromFile("grass.png");
    wallT.loadFromFile("wall.png");
    guyT.loadFromFile("guy.png");
    flagT.loadFromFile("flag.png");
    boomT.loadFromFile("boom.png");
    //creating sprites
    sf::Sprite mineS, grassS, wallS, guyS, flagS, boomS;
    mineS.setTexture(mineT);
    grassS.setTexture(grassT);
    wallS.setTexture(wallT);
    guyS.setTexture(guyT);
    flagS.setTexture(flagT);
    boomS.setTexture(boomT);
    //setting size
    guyS.setScale(0.7, 0.7);
    wallS.setScale(1, 1);
    wallS.setTextureRect(sf::IntRect(0, 0, 50, 50));
    grassS.setScale(1, 1);
    grassS.setTextureRect(sf::IntRect(0, 0, 50, 50));
    mineS.setScale(1, 1);
    mineS.setTextureRect(sf::IntRect(0, 0, 50, 50));
    flagS.setScale(1, 1);
    flagS.setTextureRect(sf::IntRect(0, 0, 50, 50));
    boomS.setScale(1, 1);
    boomS.setTextureRect(sf::IntRect(0, 0, 50, 50));

    std::vector < sf::Sprite > mines;
    std::vector < sf::Sprite > explosions;
    bool minesSpawned = false;

    sf::Clock clock;
    sf::Clock gameTimer;
    sf::Time elapsedTime;

    // Add game state and text
    bool gameOver = false;
    bool youWon = false;
    //creating font
    sf::Font font;
    font.loadFromFile("arial.ttf");

    sf::Text gameOverText("Game Over", font, 50);
    gameOverText.setFillColor(sf::Color::Red);
    gameOverText.setPosition(250, 250);

    sf::Text youWonText("You Won!", font, 50);
    youWonText.setFillColor(sf::Color::Green);
    youWonText.setPosition(270, 250);

    sf::Text timerText("", font, 24);
    timerText.setFillColor(sf::Color::White);
    timerText.setPosition(320, 10);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        if (!gameOver && !youWon) {
            elapsedTime = gameTimer.getElapsedTime();
        }
        int seconds = static_cast<int>(elapsedTime.asSeconds());
        int minutes = seconds / 60;
        seconds %= 60;
        std::ostringstream ss;
        ss << std::setw(2) << std::setfill('0') << minutes << ":" << std::setw(2) << std::setfill('0') << seconds;
        timerText.setString("Time: " + ss.str());

        //player movement
        if (!gameOver && !youWon) {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                guyS.move(0.f, -playerSpeed);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
                guyS.move(0.f, playerSpeed);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
                guyS.move(-playerSpeed, 0.f);
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
                guyS.move(playerSpeed, 0.f);
            }
        }
        //tileset logic
        int x = 0, y = 0;
        for (const auto& row : labyrinth) {
            for (int cell : row) {
                if (cell == 1) {
                    wallS.setPosition(x * 50, y * 50);
                    if (!gameOver && !youWon && guyS.getGlobalBounds().intersects(wallS.getGlobalBounds())) {
                        sf::FloatRect p = guyS.getGlobalBounds();
                        sf::FloatRect w = wallS.getGlobalBounds();
                        float dx = (p.left + p.width / 2) - (w.left + w.width / 2);
                        float dy = (p.top + p.height / 2) - (w.top + w.height / 2);
                        float overlapX = (p.width + w.width) / 2 - std::abs(dx);
                        float overlapY = (p.height + w.height) / 2 - std::abs(dy);
                        if (overlapX < overlapY)
                            guyS.move(dx > 0 ? overlapX : -overlapX, 0);
                        else
                            guyS.move(0, dy > 0 ? overlapY : -overlapY);
                    }
                    window.draw(wallS);
                }
                else if (cell == 0) {
                    grassS.setPosition(x * 50, y * 50);
                    window.draw(grassS);
                }
                if (x < 15) x++; else { y++; x = 0; }
            }
        }
        //mine logic
        if (!minesSpawned && clock.getElapsedTime().asSeconds() > 2) {
            std::srand(static_cast<unsigned>(std::time(nullptr)));
            for (int i = 0; i < 100; ++i) {
                int randomNumberX = std::rand() % 16;
                int randomNumberY = std::rand() % 12;
                if (labyrinth[randomNumberY][randomNumberX] == 1) continue;
                sf::Sprite m = mineS;
                m.setPosition(randomNumberX * 50, randomNumberY * 50);
                mines.push_back(m);
            }
            minesSpawned = true;
        }
        else if (clock.getElapsedTime().asSeconds() > 3) {
            for (auto& m : mines) {
                boomS.setPosition(m.getPosition());
                explosions.push_back(boomS);
            }
            mines.clear();
        }
        if (clock.getElapsedTime().asSeconds() > 3.5) {
            explosions.clear();
            clock.restart();
            minesSpawned = false;
        }

        for (const auto& m : mines) {
            window.draw(m);
        }

        for (auto& e : explosions) {
            window.draw(e);
        }

        for (const auto& e : explosions) {
            if (!gameOver && guyS.getGlobalBounds().intersects(e.getGlobalBounds())) {
                gameOver = true;
                elapsedTime = gameTimer.getElapsedTime();
            }
        }

        flagS.setPosition(750, 550);
        window.draw(flagS);
        if (!youWon && guyS.getGlobalBounds().intersects(flagS.getGlobalBounds())) {
            youWon = true;
            elapsedTime = gameTimer.getElapsedTime();
        }
        //border bounds
        sf::FloatRect bounds = guyS.getGlobalBounds();
        sf::Vector2u winSize = window.getSize();
        sf::Vector2f pos = guyS.getPosition();
        if (pos.x < 0) pos.x = 0;
        if (pos.y < 0) pos.y = 0;
        if (pos.x + bounds.width > winSize.x) pos.x = winSize.x - bounds.width;
        if (pos.y + bounds.height > winSize.y) pos.y = winSize.y - bounds.height;
        guyS.setPosition(pos);

        if (!gameOver && !youWon)
            window.draw(guyS);

        window.draw(timerText);

        if (gameOver)
            window.draw(gameOverText);

        if (youWon)
            window.draw(youWonText);

        window.display();
    }

    return 0;
}
