//
// Created by dimka on 3/1/2020.
//


#include "GuiParser.h"

using namespace Parser;
std::map<std::string, type> Parser::GuiParser::Types = {{"button",  type::button},
                                                        {"textbox", type::textbox},
                                                        {"image",   type::image}};
std::map<std::string, token> Parser::GuiParser::Tokens = {{"position", token::position},
                                                          {"name",     token::name},
                                                          {"text",     token::text},
                                                          {"size",     token::size},
                                                          {"path",     token::path}};

Parser::GuiParser::GuiParser(std::string_view p, tgui::Gui& gui) : path(p)
{}

void GuiParser::parse()
{
    std::ifstream i;
    i.open(path);
    std::string buffer;
    std::string prevBuffer;
    while(std::getline(i, buffer))
    {
        if(size_t n = std::count(buffer.begin(), buffer.end(), ';'); n != 0)
        {
            size_t current_expr_e = buffer.find_first_of(';');
            if(!prevBuffer.empty())
                buffer.insert(0, prevBuffer);
            prevBuffer.clear();
            typesParse(buffer.substr(0, current_expr_e));
            --n;
            while(n > 0)
            {
                size_t next_expr_s = current_expr_e + 1;
                size_t next_expr_e = buffer.find_first_of(';', next_expr_s);
                typesParse(buffer.substr(next_expr_s, next_expr_e - next_expr_s));
                current_expr_e = next_expr_e;
                --n;
            }
            if(buffer.find_last_of(';') + 1 < buffer.length())
                prevBuffer = buffer.substr(current_expr_e + 1, buffer.length() - current_expr_e - 1);
        } else
        {
            prevBuffer = buffer;
            continue;
        }
    }
    i.close();
}

void GuiParser::typesParse(const std::string& buffer)
{
    size_t type_start = buffer.find_first_of('[') + 1;
    size_t type_end = buffer.find_first_of(']');
    auto type_str = buffer.substr(type_start, type_end - type_start);
    if(auto it = Types.find(type_str); it != Types.end())
    {
        type expr_type = it->second;
        parseObject con;
        con.t = expr_type;
        size_t tokens_start = buffer.find_first_not_of(':', type_end + 1);
        size_t tokens_end = buffer.length();
        auto params_str = buffer.substr(buffer.find_first_not_of(' ', tokens_start), tokens_end - tokens_start);
        size_t curr_param_start = 0;
        size_t curr_param_end = params_str.find_first_of('}', curr_param_start);
        auto lg = params_str.length();
        while(curr_param_end <= params_str.length())
        {
            auto curr_param = params_str.substr(curr_param_start, curr_param_end - curr_param_start + 1);
            size_t curr_param_value_s = curr_param.find_first_of('{');
            size_t curr_param_value_e = curr_param.find_first_of('}');
            auto curr_param_value = curr_param.substr(curr_param_value_s, curr_param_value_e - curr_param_value_s + 1);
            size_t curr_param_token_s = curr_param.find_first_not_of(' ');
            auto curr_param_token_n = curr_param.substr(curr_param_token_s, curr_param_value_s - curr_param_token_s);
            if(auto it = Tokens.find(curr_param_token_n); it != Tokens.end())
            {
                token param_type = it->second;
                switch(param_type)
                {
                    case token::position:
                    {
                        size_t middle = curr_param_value.find_first_of(',');
                        float x = stof(curr_param_value.substr(1, middle));
                        float y = stof(curr_param_value.substr(middle + 1, curr_param_value.length() - middle - 1));
                        std::cout << curr_param.substr(0, curr_param_value_s) + " received with arguments: " + "X = " +
                                     std::to_string(x) + "Y = " + std::to_string(y) + "\n";
                        con.values.emplace(param_type, std::any{sf::Vector2f{x, y}});
                    }
                        break;
                    case token::size:
                    {
                        size_t middle = curr_param_value.find_first_of(',');
                        float x = stof(curr_param_value.substr(1, middle));
                        float y = stof(curr_param_value.substr(middle + 1, curr_param_value.length() - middle - 1));
                        std::cout << curr_param.substr(0, curr_param_value_s) + " received with arguments: " + "X = " +
                                     std::to_string(x) + "Y = " + std::to_string(y) + "\n";\
                             con.values.emplace(param_type, std::any{sf::Vector2f{x, y}});
                    }
                        break;
                    case token::name:
                        con.values.emplace(param_type, std::any{curr_param_value.substr(curr_param_value.find_first_of('\"') + 1,
                                                                                             curr_param_value.find_last_of('\"') - 2)});
                        break;
                    case token::text:
                        con.values.emplace(param_type, std::any{curr_param_value.substr(curr_param_value.find_first_of('\"') + 1,
                                                                                             curr_param_value.find_last_of('\"') - 2)});
                        break;
                    case token::path:
                        con.values.emplace(param_type, std::any{curr_param_value.substr(curr_param_value.find_first_of('\"') + 1,
                                                                                             curr_param_value.find_last_of('\"') - 2)});
                        break;
                }
            } else
                std::cout << "[Error]: param_name set incorrectly for " + curr_param + "\n";
            curr_param_start = params_str.find_first_of(',', curr_param_end);
            if(curr_param_start == std::string::npos)
                break;
            else
                ++curr_param_start;
            curr_param_end = params_str.find_first_of('}', curr_param_start);
        }
        tokenize.emplace_back(con);
    } else
        std::cout << "Oha!";
}

void GuiParser::createGui(tgui::Gui& gui)
{
    for(auto& elem : tokenize)
    {
        switch(elem.t)
        {
            case type::button:
            {
                auto button = tgui::Button::create();
                button->setText(std::any_cast<std::string>(elem.values[token::text]));
                basicTokenize(elem.values, button, elem.t);
                gui.add(button);
            }
                break;
            case type::textbox:
                break;
            case type::image:
            {
                if(elem.values.count(token::path))
                {
                    auto pic = tgui::Picture::create(std::any_cast<std::string>(elem.values[token::path]));
                    basicTokenize(elem.values, pic, elem.t);
                    gui.add(pic);
                }
            }
                break;
        }
    }
}

