#include <Game/DialogueUI.hpp>
#include <Render/WrapText.hpp>

const int TEXT_FRAME_MARGIN = 16;
const float TEXT_TYPING_SPEED = 0.02;

DialogueUI::DialogueUI(GameContext &_context) : m_context(_context)
{
    m_text.setFont(m_context.assetManager->getFont("main"));
    m_speakerName.setFont(m_context.assetManager->getFont("main"));

    m_textFrame.setSize(
        sf::Vector2f(m_context.window.getSize().x - TEXT_FRAME_MARGIN * 2,
                     m_context.window.getSize().y * 0.2)

    );

    m_textFrame.setPosition(
        TEXT_FRAME_MARGIN,
        m_context.window.getSize().y - m_textFrame.getSize().y - TEXT_FRAME_MARGIN);

    m_textFrame.setOutlineThickness(2);
    m_textFrame.setOutlineColor(sf::Color(255, 196, 54));
    m_textFrame.setFillColor(sf::Color(12, 53, 106, 128));
    m_speakerFrame.setFillColor(m_textFrame.getFillColor());
    m_speakerFrame.setOutlineColor(m_textFrame.getOutlineColor());
    m_speakerFrame.setOutlineThickness(m_textFrame.getOutlineThickness());

    m_text.setCharacterSize(16);
    m_text.setPosition(
        m_textFrame.getPosition().x + TEXT_FRAME_MARGIN, m_textFrame.getPosition().y + TEXT_FRAME_MARGIN);
    m_text.setFillColor(sf::Color::White);
    m_text.setOutlineColor(sf::Color::Black);
    m_text.setOutlineThickness(0.5f);

    m_speakerName.setFillColor(sf::Color::Yellow);
    m_speakerName.setOutlineColor(sf::Color::Black);
    m_speakerName.setOutlineThickness(0.5f);
    m_speakerName.setCharacterSize(14);

    m_lastTypedPosition = 0;
}

void DialogueUI::setStatement(const StoryDialogueStatement &stmt)
{
    m_currentStatement = stmt;
    m_lastTypedPosition = 0;
    m_isTyping = true;
    m_text.setString("");

    if (m_currentStatement.speaker.length())
    {
        m_speakerName.setString(stmt.speaker);
        m_speakerFrame.setSize(
            sf::Vector2f(
                m_speakerName.getLocalBounds().width + TEXT_FRAME_MARGIN * 2,
                m_speakerName.getLocalBounds().height + TEXT_FRAME_MARGIN));

        m_speakerFrame.setPosition(
            m_textFrame.getPosition() - sf::Vector2f(
                                            0, m_speakerFrame.getSize().y + TEXT_FRAME_MARGIN / 2));
        m_speakerName.setPosition(
            m_speakerFrame.getPosition() + sf::Vector2f(
                                               TEXT_FRAME_MARGIN, TEXT_FRAME_MARGIN / 2));
    }
    else
    {
        m_speakerName.setString("");
        m_speakerFrame.setPosition(-500, -500);
    }
}

void DialogueUI::update(float frameDelta)
{
    if (isTyping())
    {
        if (m_typingClock.getElapsedTime().asSeconds() > TEXT_TYPING_SPEED)
        {
            m_typingClock.restart();

            if (m_lastTypedPosition < m_currentStatement.text.length())
            {
                fitText();
                m_lastTypedPosition++;
            }
            else
            {
                finishTyping();
            }
        }
    }
}

void DialogueUI::render()
{
    if (!m_isShown)
        return;

    m_context.window.draw(m_speakerFrame);
    m_context.window.draw(m_speakerName);
    m_context.window.draw(m_textFrame);
    m_context.window.draw(m_text);

    if (m_currentStatement.speaker.length())
    {
        m_context.window.draw(m_speakerFrame);
        m_context.window.draw(m_speakerName);
    }
}

void DialogueUI::finishTyping()
{
    m_lastTypedPosition = m_currentStatement.text.length();
    fitText();
    m_isTyping = false;
}

bool DialogueUI::isTyping()
{
    return m_isTyping;
}

void DialogueUI::hide()
{
    m_isShown = false;
}

void DialogueUI::show()
{
    m_isShown = true;
}

void DialogueUI::fitText()
{
    m_text.setString(
        RenderUtils::wrapText(
            m_currentStatement.text.substr(0, m_lastTypedPosition),
            m_textFrame.getSize().x - TEXT_FRAME_MARGIN * 2,
            *m_text.getFont(),
            m_text.getCharacterSize())

    );
}