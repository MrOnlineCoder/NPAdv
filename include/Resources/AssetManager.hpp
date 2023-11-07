#ifndef ASSETMANAGER_HPP
#define ASSETMANAGER_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include <unordered_map>

const std::string ASSETS_PATH = "assets/";

class AssetManager
{
public:
    AssetManager();

    void init();

    void loadFont(const std::string &key, const std::string &path);
    void loadTexture(const std::string &key, const std::string &path);
    void loadSound(const std::string &key, const std::string &path);

    void loadAllSoundsInFolder(const std::string &folder);
    void loadAllTexturesInFolder(const std::string &folder);

    sf::Texture &getTexture(const std::string &key);
    sf::Font &getFont(const std::string &key);
    sf::SoundBuffer &getSound(const std::string &key);

    bool hasError();
    void release();

private:
    std::unordered_map<std::string, sf::Font *> m_fonts;
    std::unordered_map<std::string, sf::Texture *> m_textures;
    std::unordered_map<std::string, sf::SoundBuffer *> m_sounds;

    sf::Texture m_fallbackTexture;
    sf::Font m_fallbackFont;
    sf::SoundBuffer m_fallbackSound;

    bool m_errorFlag;
};

#endif