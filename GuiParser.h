//
// Created by dimka on 3/1/2020.
//

#ifndef TGUILUA_GUIPARSER_H
#define TGUILUA_GUIPARSER_H

#include <map>
#include <fstream>
#include <iostream>
#include <string>
#include <TGUI/TGUI.hpp>
#include <any>

using namespace std;
namespace Parser
{
    enum class type
    {
        button, textbox, image, group
    };

    enum class token
    {
        position, size, name, text, path, array
    };

    struct parseObject
    {
        type t;
        bool usedInGroup = false;
        std::map<token, std::any> values;
    };

    class GuiParser
    {
    private:
        static std::map<std::string, type> Types;
        static std::map<std::string, token> Tokens;

        std::vector<parseObject*> tokenizeFile;
        tgui::Gui* g;
        std::string path;

        template<class T>
        void basicTokenize(std::map<token, std::any>& values, T& obj, type typ)
        {
            for(auto& v : values)
            {
                switch(v.first)
                {
                    case token::position:
                        obj->setPosition(std::any_cast<sf::Vector2f>(v.second));
                        break;
                    case token::size:
                        obj->setSize(std::any_cast<sf::Vector2f>(v.second));
                        break;
                    case token::name:
                        obj->setWidgetName(std::any_cast<std::string>(v.second));
                        break;
                }
            }
        }

        tgui::Button::Ptr ButtonCreate(parseObject&);
        tgui::TextBox::Ptr TextBoxCreate(parseObject&);
        tgui::Picture::Ptr PictureCreate(parseObject&);
        tgui::Group::Ptr GroupCreate(parseObject& elem);
    public:
        GuiParser(std::string_view, tgui::Gui&);

        void tokenize();

        void createGui(tgui::Gui& g);

        void typesParse(const std::string&);
    };
}
#endif //TGUILUA_GUIPARSER_H
