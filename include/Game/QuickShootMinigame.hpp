#include <Game/Minigame.hpp>
#include <random>

enum class QuickShootState
{
    INTRO,
    SHOOTING,
    END
};

class QuickShootMinigame : public Minigame
{
public:
    QuickShootMinigame(GameContext &context);

    void render();
    void update(float delta);
    void start();
    void input(sf::Event &ev);

    bool hasUserWon();

private:
    void nextTarget();

    sf::Sprite m_bg;

    sf::Sound m_startSound;
    int m_secondsLeft;
    sf::Text m_timerText;
    sf::Text m_scoreText;

    sf::Clock m_gameClock;
    int m_score;
    QuickShootState m_state;
    sf::Sound m_shotSound;
    sf::Sound m_introSound;
    sf::Music m_music;

    sf::Sprite m_target;

    std::random_device m_rndDevice;
    std::mt19937 m_rndGen;
    std::uniform_int_distribution<> m_rndDistr;

    sf::Sprite m_crosshair;
};