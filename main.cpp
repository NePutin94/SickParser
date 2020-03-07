#define SOL_ALL_SAFETIES_ON 1

#include <TGUI/TGUI.hpp>

#include <iostream>
#include "Game.h"
#include "GuiParser.h"

using namespace Engine;

int main()
{
    sf::RenderWindow window{{800, 600}, "Window"};
    tgui::Gui gui{window}; // Create the gui and attach it to the window
    Parser::GuiParser p("test.txt", gui);
    p.parse();
    p.createGui(gui);
    //sf::Texture t;
    //t.loadFromFile("D:\\!VisualStudioProject\\SfmlProject\\SFMLEngine\\Release\\Data\\images\\logoz.png");
    //auto Pic = tgui::Picture::create(t);
    //Pic->setSize(12,12);
    //gui.add(Pic);
    while(window.isOpen())
    {
        sf::Event event{};
        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
                window.close();

            gui.handleEvent(event); // Pass the event to the widgets
        }

        window.clear();
        gui.draw(); // Draw all widgets
        window.display();
    }
}