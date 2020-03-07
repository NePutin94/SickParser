#include "GuiParser.h"
using namespace Parser;

Parser::GuiParser::GuiParser(std::string_view p, tgui::Gui& gui) : path(p)
{
    g = &gui;
}

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

void GuiParser::typesParse(std::string buffer)
{
    size_t type_start = buffer.find_first_of('[') + 1;
    size_t type_end = buffer.find_first_of(']');
    auto type_str = buffer.substr(type_start, type_end - type_start);
    if(auto it = Types.find(type_str); it != Types.end())
    {
        type expr_type = it->second;
        switch(expr_type)
        {
            case type::button:
            {
                tgui::Button::Ptr w = tgui::Button::create();
                valueParse(w, buffer, type_end);
                g->add(w);
            }
                break;
            case type::textbox:
            {
                tgui::TextBox::Ptr l = tgui::TextBox::create();
                valueParse(l, buffer, type_end);
                g->add(l);
            }
                break;
        }
    } else
        std::cout << "Oha!";
}
