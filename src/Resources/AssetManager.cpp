#include <Resources/AssetManager.hpp>
#include <Common/Logger.hpp>
#include <Common/FilesystemUtils.hpp>

AssetManager::AssetManager()
{
    m_fonts.reserve(8);
    m_textures.reserve(32);
}

void AssetManager::init()
{
    m_errorFlag = false;

    sf::Image fallbackImg;
    fallbackImg.create(64, 64, sf::Color::Magenta);
    m_fallbackTexture.loadFromImage(fallbackImg);
}

void AssetManager::loadFont(const std::string &key, const std::string &path)
{
    sf::Font *f = new sf::Font();

    if (!f->loadFromFile(ASSETS_PATH + path))
    {
        m_errorFlag = true;

        GetLogger().tag("AssetManager") << "ERROR: Failed to load font " << path;
        delete f;
        return;
    }
    // f->setSmooth(fal);

    m_fonts.insert(std::pair<std::string, sf::Font *>(key, f));

    GetLogger().tag("AssetManager") << "Loaded font " << key;
}

void AssetManager::loadTexture(const std::string &key, const std::string &path)
{
    sf::Texture *t = new sf::Texture();

    if (!t->loadFromFile(ASSETS_PATH + path))
    {
        m_errorFlag = true;

        GetLogger().tag("AssetManager") << "ERROR: Failed to load texture " << path;
        delete t;
        return;
    }

    m_textures.insert(std::pair<std::string, sf::Texture *>(key, t));

    GetLogger().tag("AssetManager") << "Loaded texture " << key;
}

void AssetManager::loadSound(const std::string &key, const std::string &path)
{
    sf::SoundBuffer *s = new sf::SoundBuffer();

    if (!s->loadFromFile(ASSETS_PATH + path))
    {
        m_errorFlag = true;

        GetLogger().tag("AssetManager") << "ERROR: Failed to load sound " << path;
        delete s;
        return;
    }

    m_sounds.insert(std::pair<std::string, sf::SoundBuffer *>(key, s));

    GetLogger().tag("AssetManager") << "Loaded sound " << key;
}

bool AssetManager::hasError()
{
    return m_errorFlag;
}

sf::Texture &AssetManager::getTexture(const std::string &key)
{
    std::unordered_map<std::string, sf::Texture *>::iterator it = m_textures.find(key);

    if (it == m_textures.end())
    {
        return m_fallbackTexture;
    }
    else
    {
        return *it->second;
    }
}

sf::Font &AssetManager::getFont(const std::string &key)
{
    std::unordered_map<std::string, sf::Font *>::iterator it = m_fonts.find(key);

    if (it == m_fonts.end())
    {
        return m_fallbackFont;
    }
    else
    {
        return *it->second;
    }
}

sf::SoundBuffer &AssetManager::getSound(const std::string &key)
{
    std::unordered_map<std::string, sf::SoundBuffer *>::iterator it = m_sounds.find(key);

    if (it == m_sounds.end())
    {
        return m_fallbackSound;
    }
    else
    {
        return *it->second;
    }
}

void AssetManager::release()
{
    GetLogger().tag("AssetManager") << "Releasing all assets...";
    GetLogger().tag("AssetManager") << "Unloading fonts...";

    for (
        std::unordered_map<std::string, sf::Font *>::iterator it = m_fonts.begin();
        it != m_fonts.end();
        it++)
    {
        delete it->second;
        GetLogger().tag("AssetManager") << "Unloaded font " << it->first;
    }

    m_fonts.clear();

    GetLogger().tag("AssetManager") << "Unloading textures...";

    for (
        std::unordered_map<std::string, sf::Texture *>::iterator it = m_textures.begin();
        it != m_textures.end();
        it++)
    {
        delete it->second;
        GetLogger().tag("AssetManager") << "Unloaded texture " << it->first;
    }

    m_textures.clear();

    GetLogger().tag("AssetManager") << "Unloading sounds...";

    for (
        std::unordered_map<std::string, sf::SoundBuffer *>::iterator it = m_sounds.begin();
        it != m_sounds.end();
        it++)
    {
        delete it->second;
        GetLogger().tag("AssetManager") << "Unloaded sound " << it->first;
    }

    m_sounds.clear();

    GetLogger().tag("AssetManager") << "Finished unloading all resources.";
}

void AssetManager::loadAllSoundsInFolder(const std::string &folder)
{
    std::string fullPath = ASSETS_PATH + folder;

    auto files = FilesystemUtils::listFilesInDirectory(fullPath);

    GetLogger().tag("AssetManager") << "Loading all sounds in folder " << folder << ", found " << (int)files.size() << " files";

    for (auto filename : files)
    {
        if (filename.find(".ogg") == std::string::npos && filename.find(".wav") == std::string::npos && filename.find(".mp3") == std::string::npos)
            continue;

        loadSound(FilesystemUtils::getFileBasename(filename), folder + "/" + filename);
    }
}

void AssetManager::loadAllTexturesInFolder(const std::string &folder)
{
    std::string fullPath = ASSETS_PATH + folder;

    auto files = FilesystemUtils::listFilesInDirectory(fullPath);

    GetLogger().tag("AssetManager") << "Loading all textures in folder " << folder << ", found " << (int)files.size() << " files";

    for (auto filename : files)
    {
        if (filename.find(".png") == std::string::npos && filename.find(".jpg") == std::string::npos)
            continue;

        loadTexture(FilesystemUtils::getFileBasename(filename), folder + "/" + filename);
    }
}
