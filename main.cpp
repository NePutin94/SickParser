#include <TGUI/TGUI.hpp>
#include <iostream>
#include "GuiParser.h"

using namespace Engine;

int main()
{
    sf::RenderWindow window{{800, 600}, "Window"};
    tgui::Gui gui{window};
    Parser::GuiParser p("test.txt", gui);
    p.parse();
    while(window.isOpen())
    {
        sf::Event event{};
        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                window.close();

            gui.handleEvent(event);
        }

        window.clear();
        gui.draw();
        window.display();
    }
}