#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <random>

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float GRAVITY = 0.5f;
const float JUMP_FORCE = 10.0f;
const float PIPE_SPEED = 3.0f;
int goalScore = 3;

sf::RenderWindow window;
sf::Texture birdTexture;
sf::Sprite birdSprite;
float birdVelocity;

sf::Texture backgroundTexture;
sf::Sprite backgroundSprite;

std::vector<sf::RectangleShape> pipes;
float gapSize = 200.0f;
float pipeWidth = 80.0f;
float pipeSpacing = 400.0f;

int score = 0;
int bestScore = 0; // Added best score variable
bool isGameOver = false;
bool isGameStarted = false;

void initialize();
void handleInput();
void update();
void render();
void addPipe();
void startGame();
void gameOver();

int main() {
    initialize();

    while (window.isOpen()) {
        handleInput();
        update();
        render();
    }

    return 0;
}

void initialize() {
    window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Flappy Bird");
    window.setFramerateLimit(60);

    birdTexture.loadFromFile("bird4bg.png");
    birdSprite.setTexture(birdTexture);
    birdSprite.setPosition(WINDOW_WIDTH / 4, WINDOW_HEIGHT / 2);
    birdVelocity = 0.0f;

    if (!backgroundTexture.loadFromFile("background.png")) {
        // Handle error if the background image failed to load
    }
    backgroundSprite.setTexture(backgroundTexture);

    pipes.clear();
    score = 0;
    isGameOver = false;
    isGameStarted = false;
}

void handleInput() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type == sf::Event::KeyPressed) {
            if (!isGameStarted) {
                startGame();
            } else if (isGameOver) {
                if (event.key.code == sf::Keyboard::Y) {
                    initialize();
                }
            } else {
                if (event.key.code == sf::Keyboard::Space) {
                    birdVelocity = -JUMP_FORCE;
                }
            }
        }
    }
}

void update() {
    if (!isGameStarted || isGameOver) {
        return;
    }

    birdVelocity += GRAVITY;
    birdSprite.move(0, birdVelocity);

    if (birdSprite.getPosition().y < 0 || birdSprite.getPosition().y + birdSprite.getLocalBounds().height > WINDOW_HEIGHT) {
        isGameOver = true;
    }

    bool passedPipe = false; // To track if the bird has successfully passed a pipe

    for (size_t i = 0; i < pipes.size(); i += 2) {
        pipes[i].move(-PIPE_SPEED, 0);
        pipes[i + 1].move(-PIPE_SPEED, 0);

        if (pipes[i].getPosition().x + pipeWidth < 0) {
            pipes.erase(pipes.begin() + i, pipes.begin() + i + 2);
        }

        if (!passedPipe && birdSprite.getPosition().x > pipes[i].getPosition().x + pipeWidth &&
            birdSprite.getPosition().x < pipes[i].getPosition().x + pipeWidth + PIPE_SPEED) {
            score++; // Increase score by 1 when the bird successfully passes a pipe gap
            passedPipe = true;
        }

        if (birdSprite.getGlobalBounds().intersects(pipes[i].getGlobalBounds()) ||
            birdSprite.getGlobalBounds().intersects(pipes[i + 1].getGlobalBounds())) {
            isGameOver = true;
        }
    }

    if (passedPipe) {
        passedPipe = false; // Reset the flag for the next pipe
    }

    if (pipes.empty() || WINDOW_WIDTH - pipes.back().getPosition().x >= pipeSpacing) {
        addPipe();
    }

    if (score > bestScore) { // Update best score if the current score is higher
        bestScore = score;
    }

}


void render() {
    window.clear();

    if (!isGameStarted) {
        window.draw(backgroundSprite);

        sf::Font font;
        if (!font.loadFromFile("arial.ttf")) {
            // Handle error if the font failed to load
        }

        sf::Text bannerText("Welcome to Flappy Bird", font, 50);
        bannerText.setFillColor(sf::Color::Black);
        bannerText.setPosition(WINDOW_WIDTH / 2 - bannerText.getLocalBounds().width / 2, WINDOW_HEIGHT / 2 - bannerText.getLocalBounds().height);
        window.draw(bannerText);

        sf::Text startText("Press any key to start", font, 40);
        startText.setFillColor(sf::Color::Black);
        // startText.setPosition(WINDOW_WIDTH / 2 - startText.getLocalBounds().width / 2, WINDOW_HEIGHT / 2);
        startText.setPosition(WINDOW_WIDTH / 2 - startText.getLocalBounds().width / 2, 330);

        window.draw(startText);

        // sf::Text bestScoreText("Best Score: " + std::to_string(bestScore), font, 30);
        // bestScoreText.setFillColor(sf::Color::White);
        // bestScoreText.setPosition(10, 50);
        // window.draw(bestScoreText);

        if (score < bestScore) {
            
            int remainingScore = bestScore - score;
            sf::Text remainingScoreText("Remaining Score to Beat Best: " + std::to_string(remainingScore), font, 30);
            remainingScoreText.setFillColor(sf::Color::Black);
            remainingScoreText.setPosition(WINDOW_WIDTH / 2 - remainingScoreText.getLocalBounds().width / 2, WINDOW_HEIGHT / 2 - remainingScoreText.getLocalBounds().height / 2 + 150);
            window.draw(remainingScoreText);
        }
    } else {
        window.draw(backgroundSprite);

        for (const auto& pipe : pipes) {
            window.draw(pipe);
        }
        window.draw(birdSprite);

        sf::Font font;
        if (!font.loadFromFile("arial.ttf")) {
            // Handle error if the font failed to load
        }

        if (isGameOver) {
            sf::Text gameOverText("Game Over", font, 50);
            gameOverText.setFillColor(sf::Color::Red);
            gameOverText.setPosition(WINDOW_WIDTH / 2 - gameOverText.getLocalBounds().width / 2, WINDOW_HEIGHT / 2 - gameOverText.getLocalBounds().height / 2 - 50);
            window.draw(gameOverText);

            sf::Text scoreText("Final Score: " + std::to_string(score), font, 30);
            scoreText.setFillColor(sf::Color::Black);
            scoreText.setPosition(WINDOW_WIDTH / 2 - scoreText.getLocalBounds().width / 2, WINDOW_HEIGHT / 2 - scoreText.getLocalBounds().height / 2 + 50);
            window.draw(scoreText);

            sf::Text playAgainText("Press 'Y' to play again", font, 30);
            playAgainText.setFillColor(sf::Color::Black);
            playAgainText.setPosition(WINDOW_WIDTH / 2 - playAgainText.getLocalBounds().width / 2, WINDOW_HEIGHT / 2 - playAgainText.getLocalBounds().height / 2 + 100);
            window.draw(playAgainText);
        } else {
            sf::Text scoreText("Score: " + std::to_string(score), font, 30);
            scoreText.setFillColor(sf::Color::Black);
            scoreText.setPosition(10, 10);
            window.draw(scoreText);

            sf::Text bestScoreText("Best Score: " + std::to_string(bestScore), font, 30);
            bestScoreText.setFillColor(sf::Color::Black);
            bestScoreText.setPosition(10, 50);
            window.draw(bestScoreText);

            if (score < goalScore) {
                int remainingScore = goalScore - score;
                sf::Text remainingScoreText("Remaining Score: " + std::to_string(remainingScore), font, 30);
                remainingScoreText.setFillColor(sf::Color::Black);
                remainingScoreText.setPosition(10, 90);
                window.draw(remainingScoreText);
            } else {
                sf::Text goalAchievedText("Congratulations! You achieved the goal score!", font, 30);
                goalAchievedText.setFillColor(sf::Color::Red);
                goalAchievedText.setPosition(10, 90);
                window.draw(goalAchievedText);

                goalScore = bestScore;
            }
        }
    }

    window.display();
}
const int pipeGapSize = 200; // The size of the gap between the pipes

void addPipe() {
    int gapPosition = rand() % (WINDOW_HEIGHT - pipeGapSize); // Random position within the height of the window

    sf::RectangleShape upperPipe(sf::Vector2f(pipeWidth, gapPosition));
    upperPipe.setPosition(WINDOW_WIDTH, 0);
    upperPipe.setFillColor(sf::Color::Green);
    pipes.push_back(upperPipe);

    sf::RectangleShape lowerPipe(sf::Vector2f(pipeWidth, WINDOW_HEIGHT - gapPosition - pipeGapSize));
    lowerPipe.setPosition(WINDOW_WIDTH, gapPosition + pipeGapSize);
    lowerPipe.setFillColor(sf::Color::Green);
    pipes.push_back(lowerPipe);
}

void startGame() {
    isGameStarted = true;
}

void gameOver() {
    isGameOver = true;
}