//
// Created by dimka on 3/1/2020.
//


#include "GuiParser.h"

using namespace Parser;
std::map<std::string, type> Parser::GuiParser::Types = {{"button",  type::button},
                                                        {"textbox", type::textbox},
                                                        {"image",   type::image},
                                                        {"group",   type::group}};
std::map<std::string, token> Parser::GuiParser::Tokens = {{"position", token::position},
                                                          {"name",     token::name},
                                                          {"text",     token::text},
                                                          {"size",     token::size},
                                                          {"path",     token::path},
                                                          {"visible",  token::visible},
                                                          {"array",    token::array},
                                                          {"theme",    token::theme}};


Parser::GuiParser::GuiParser(std::string_view p) : path(p)
{

}

void GuiParser::tokenize()
{
    std::ifstream i;
    i.open(path);
    std::string buffer;
    std::string prevBuffer;
    size_t line = 1;
    try
    {
        while(std::getline(i, buffer))
        {
            if(size_t n = std::count(buffer.begin(), buffer.end(), ';'); n != 0)
            {
                if(!prevBuffer.empty())
                    buffer.insert(0, prevBuffer);
                prevBuffer.clear();
                size_t current_expr_e = buffer.find_first_of(';');
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
            ++line;
        }
    }
    catch (TokenParseException& p)
    {
        tokenizeFile.clear();
        std::cout << "[line " + std::to_string(line) + "] " + p.what() + p.getToken() +
                     " token cannot be obtained for the " + p.getType() +
                     " type, expression \"" + p.getExpression() + "\"";
    }
    catch (std::invalid_argument& p)
    {
        tokenizeFile.clear();
        std::cout << p.what();
    }
    catch (std::out_of_range& p)
    {
        tokenizeFile.clear();
        std::cout << p.what();
    }
    catch (...)
    {
        tokenizeFile.clear();
    }
    i.close();
}

void GuiParser::param_parse(size_t curr_param_start, size_t curr_param_end, std::string params_str, type expr_type, parseObject* con)
{
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
                case token::position: // number parse
                case token::size:
                {
                    size_t middle = curr_param_value.find_first_of(',');
                    float x = stof(curr_param_value.substr(1, middle));
                    float y = stof(curr_param_value.substr(middle + 1, curr_param_value.length() - middle - 1));
                    con->values.emplace(param_type, std::any{sf::Vector2f{x, y}});
                }
                    break;
                case token::name: // srting parse
                case token::text:
                case token::theme:
                case token::path:
                {
                    auto str = curr_param_value.substr(curr_param_value.find_first_of('\"') + 1,
                                                       (curr_param_value.find_last_of('\"') -
                                                        curr_param_value.find_first_of('\"'))
                                                       - 1);
                    con->values.emplace(param_type, std::any{str});
                }
                    break;
                case token::visible: //boolean parse
                {
                    auto str = curr_param_value.substr(1, (curr_param_value.length() - 1) - 1);
                    bool value;
                    if(str.find("true") != std::string::npos)
                        value = true;
                    else if(str.find("false") != std::string::npos)
                        value = false;
                    else
                        throw TokenParseException(expr_type, param_type, str.c_str());
                    con->values.emplace(param_type, std::any{value});
                }
                    break;
                case token::array: // string array parse
                {
                    auto elem_count = std::count(curr_param_value.begin(), curr_param_value.end(), ',') + 1;
                    if(elem_count == 0)
                        break;
                    auto array_elem_start = curr_param_value.find_first_of('\"') + 1;
                    auto array_elem_end = curr_param_value.find_first_of('\"', array_elem_start) - 1;
                    std::vector<std::string> names;
                    while(elem_count > 0)
                    {
                        names.push_back(curr_param_value.substr(array_elem_start, array_elem_end - array_elem_start + 1));
                        array_elem_start = curr_param_value.find_first_of('\"', array_elem_end + 2) + 1;
                        array_elem_end = curr_param_value.find_first_of('\"', array_elem_start + 1) - 1;
                        --elem_count;
                    }
                    std::vector<parseObject*> pointers;
                    for(auto& n : names)
                    {
                        auto f = std::find_if(tokenizeFile.begin(), tokenizeFile.end(), [n](parseObject* t)
                        {
                            return (t->values.count(token::name)) ? std::any_cast<std::string>(t->values[token::name]) == n : false;
                        });
                        if(f == tokenizeFile.end())
                            throw TokenParseException(expr_type, param_type, n.c_str());
                            //std::cout << "[Error]: group_array_value cannot be found or is not defined, value:  " + n + "\n";
                        else
                        {
                            (*f)->usedInGroup = true;
                            pointers.push_back(*f);
                        }
                    }
                    con->values.emplace(param_type, std::any{pointers});
                }
                    break;
            }
        } else
            throw TokenParseException(expr_type, token::undefined, curr_param_token_n);
        curr_param_start = params_str.find_first_of(',', curr_param_end);
        if(curr_param_start == std::string::npos)
            break;
        else
            ++curr_param_start;
        curr_param_end = params_str.find_first_of('}', curr_param_start);
    }
}

void GuiParser::typesParse(const std::string& buffer)
{
    if(size_t global_param_s = buffer.find_first_of('#');global_param_s != std::string::npos)
    {
        auto* con = new parseObject();
        con->t = type::global_param;
        size_t global_param_e = buffer.find_first_of('}');
        if(global_param_e != std::string::npos)
            try
            {
                param_parse(global_param_s + 1, global_param_e, buffer.substr(global_param_s, global_param_e - global_param_s + 1),
                            type::global_param, con);
                tokenizeFile.emplace_back(con);
            }
            catch (...)
            {
                delete con;
                throw;
            }
    } else
    {
        size_t type_start = buffer.find_first_of('[') + 1;
        size_t type_end = buffer.find_first_of(']');
        auto type_str = buffer.substr(type_start, type_end - type_start);
        if(auto it1 = Types.find(type_str); it1 != Types.end())
        {
            type expr_type = it1->second;
            auto* con = new parseObject();
            con->t = expr_type;
            size_t tokens_start = buffer.find_first_not_of(':', type_end + 1);
            size_t tokens_end = buffer.length();
            auto params_str = buffer.substr(buffer.find_first_not_of(' ', tokens_start), tokens_end - tokens_start);
            size_t curr_param_start = 0;
            size_t curr_param_end = params_str.find_first_of('}', curr_param_start);
            try
            {
                param_parse(curr_param_start, curr_param_end, params_str, expr_type, con);
                tokenizeFile.emplace_back(con);
            }
            catch (...)
            {
                delete con;
                throw;
            }
        } else
            std::cout << "Oha!";
    }
}

void GuiParser::createGui(tgui::Gui& gui)
{
    for(auto& elem : tokenizeFile)
    {
        switch(elem->t)
        {
            case type::global_param:
                glob_params.erase(std::remove_if(glob_params.begin(), glob_params.end(), [&elem](glob_param& param)
                {
                    return param.to == elem->values.begin()->first;
                }), glob_params.end());
                glob_params.emplace_back(glob_param(*elem));
                break;
            case type::button:
                if(!elem->usedInGroup)
                {
                    gui.add(ButtonCreate(*elem));
                }
                break;
            case type::textbox:
                if(!elem->usedInGroup)
                {
                    gui.add(TextBoxCreate(*elem));
                }
                break;
            case type::image:
                if(!elem->usedInGroup)
                {
                    gui.add(PictureCreate(*elem));
                }
                break;
            case type::group:
            {
                gui.add(GroupCreate(*elem));
            }
                break;
        }
    }
}

tgui::Button::Ptr GuiParser::ButtonCreate(parseObject& elem)
{
    auto button = tgui::Button::create();
    button->setText(std::any_cast<std::string>(elem.values[token::text]));
    basicTokenize(elem.values, button, elem.t);
    return button;
}

tgui::TextBox::Ptr GuiParser::TextBoxCreate(parseObject& elem)
{
    auto textBox = tgui::TextBox::create();
    textBox->setText(std::any_cast<std::string>(elem.values[token::text]));
    basicTokenize(elem.values, textBox, elem.t);
    return textBox;
}

tgui::Picture::Ptr GuiParser::PictureCreate(parseObject& elem)
{
    if(elem.values.count(token::path))
    {
        auto pic = tgui::Picture::create(std::any_cast<std::string>(elem.values[token::path]));
        basicTokenize(elem.values, pic, elem.t);
        return pic;
    } else
    {
        auto pic = tgui::Picture::create();
        basicTokenize(elem.values, pic, elem.t);
        return pic;
    }
}

tgui::Group::Ptr GuiParser::GroupCreate(parseObject& elem)
{
    auto group = tgui::Group::create();
    basicTokenize(elem.values, group, elem.t);
    for(auto p : std::any_cast<std::vector<parseObject*>>(elem.values[token::array]))
    {
        switch(p->t)
        {
            case type::button:
                if(!elem.usedInGroup)
                    group->add(ButtonCreate(*p));
                break;
            case type::textbox:
                if(!elem.usedInGroup)
                    group->add(TextBoxCreate(*p));
                break;
            case type::image:
                if(!elem.usedInGroup)
                    group->add(PictureCreate(*p));
                break;
        }
    }
    return group;
}

