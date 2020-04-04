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
        button = 0, textbox, image, group, global_param
    };

    enum class token
    {
        position = 0, size, name, text, path, array, visible, theme, undefined
    };

    constexpr std::string_view tokenString[] =
            {
                    "position",
                    "size",
                    "name",
                    "text",
                    "path",
                    "array",
                    "visible",
                    "undefined",
                    "theme"
            };
    constexpr std::string_view typeString[] =
            {
                    "Button",
                    "TextBox",
                    "Image",
                    "Group",
                    "global_param"
            };

    inline const char* typeToStr(type t) noexcept
    {
        return typeString[(int) t].data();
    }

    inline const char* tokenToStr(token t) noexcept
    {
        return tokenString[(int) t].data();
    }

    struct parseObject
    {
        type t;
        bool usedInGroup = false;
        std::map<token, std::any> values;
    };

    struct glob_param
    {
        token to;
        type tp;
        std::any value;

        glob_param(parseObject& obj)
        {
            tp = obj.t;
            auto iter = obj.values.begin();
            to = iter->first;
            value = iter->second;
        }
    };

    class TokenParseException : public std::exception
    {
    private:
        const char* typ;
        const char* tok;
        std::string expr;
    public:
        TokenParseException() = delete;

        explicit TokenParseException(type typ, token tok, std::string_view ex) : typ(typeToStr(typ)), tok(tokenToStr(tok)), expr(ex)
        {}

        [[nodiscard]] const char* what() const noexcept override
        {
            return "Token Parse Error: ";
        }

        [[nodiscard]] const char* getType() const noexcept
        {
            return typ;
        }

        [[nodiscard]] const char* getToken() const noexcept
        {
            return tok;
        }

        [[nodiscard]] const char* getExpression() const
        {
            return expr.c_str();
        }
    };

    class GuiParser
    {
    private:
        static std::map<std::string, type> Types;
        static std::map<std::string, token> Tokens;

        std::vector<parseObject*> tokenizeFile;
        std::vector<glob_param> glob_params;
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
                    case token::visible:
                        obj->setVisible(std::any_cast<bool>(v.second));
                        break;
                    case token::theme:
                        if(!(typ == type::group || typ == type::image))
                        {
                            tgui::Theme t{std::any_cast<std::string>(v.second)};
                            obj->setRenderer(t.getRenderer(typeToStr(typ)));
                        }
                        break;
                }
            }
            for(auto& v : glob_params)
            {
                switch(v.to)
                {
                    case token::position:
                        obj->setPosition(std::any_cast<sf::Vector2f>(v.value));
                        break;
                    case token::size:
                        obj->setSize(std::any_cast<sf::Vector2f>(v.value));
                        break;
                    case token::name:
                        obj->setWidgetName(std::any_cast<std::string>(v.value));
                        break;
                    case token::visible:
                        obj->setVisible(std::any_cast<bool>(v.value));
                        break;
                    case token::theme:
                        if(!(typ == type::group || typ == type::image))
                        {
                            tgui::Theme t{std::any_cast<std::string>(v.value)};
                            obj->setRenderer(t.getRenderer(typeToStr(typ)));
                        }
                        break;
                }
            }
        }

        tgui::Button::Ptr ButtonCreate(parseObject&);

        tgui::TextBox::Ptr TextBoxCreate(parseObject&);

        tgui::Picture::Ptr PictureCreate(parseObject&);

        tgui::Group::Ptr GroupCreate(parseObject& elem);

        void param_parse(size_t curr_param_start, size_t curr_param_end, std::string params_str, type expr_type, parseObject*);

    public:
        GuiParser(std::string_view);

        void tokenize();

        void createGui(tgui::Gui& g);

        void typesParse(const std::string&);
    };

}
#endif //TGUILUA_GUIPARSER_H
