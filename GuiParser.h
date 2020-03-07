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
        button, textbox, image
    };

    enum class token
    {
        position, size, name, text, path
    };
    struct cont
    {
        type t;
        std::vector<std::pair<token, std::any>> values;
    };

    class GuiParser
    {
    private:
        std::map<std::string, type> Types = {{"button",  type::button},
                                             {"textbox", type::textbox}};
        std::map<std::string, token> Tokens = {{"position", token::position},
                                               {"name",     token::name},
                                               {"text",     token::text},
                                               {"size",     token::size},
                                               {"path",     token::path}};
        std::vector<cont> tokenize;
        tgui::Gui* g;

    public:
        GuiParser(std::string_view, tgui::Gui&);

        void parse();

        void typesParse(std::string);

        template<class T>
        void valueParse(T obj, const string& buffer, size_t type_end)
        {
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
                auto curr_param_token_s = curr_param.find_first_not_of(' ');
                auto curr_param_token_n = curr_param.substr(curr_param_token_s, curr_param_value_s - curr_param_token_s);
                if(auto it = Tokens.find(curr_param_token_n); it != Tokens.end())
                {
                    token param_type = it->second;
                    switch(param_type)
                    {
                        case token::position:
                        {
                            size_t middle = curr_param_value.find_first_of(',');
                            auto x = stod(curr_param_value.substr(1, middle));
                            auto y = stod(curr_param_value.substr(middle + 1, curr_param_value.length() - middle - 1));
                            std::cout << curr_param.substr(0, curr_param_value_s) + " received with arguments: " + "X = " +
                                         std::to_string(x) + "Y = " + std::to_string(y) + "\n";
                            obj->setPosition(x, y);
                        }
                            break;
                        case token::size:
                        {
                            size_t middle = curr_param_value.find_first_of(',');
                            auto x = stod(curr_param_value.substr(1, middle));
                            auto y = stod(curr_param_value.substr(middle + 1, curr_param_value.length() - middle - 1));
                            std::cout << curr_param.substr(0, curr_param_value_s) + " received with arguments: " + "X = " +
                                         std::to_string(x) + "Y = " + std::to_string(y) + "\n";
                            obj->setSize(x, y);
                        }
                            break;
                        case token::name:
                            obj->setWidgetName(curr_param_value.substr(curr_param_value.find_first_of('\"') + 1,
                                                                       curr_param_value.find_last_of('\"') - 2));
                            break;
                        case token::text:
                            obj->setText(curr_param_value.substr(curr_param_value.find_first_of('\"') + 1,
                                                                 curr_param_value.find_last_of('\"') - 2));
                            break;
                        case token::path:
                        {

                        }
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
        }
    };
}
#endif //TGUILUA_GUIPARSER_H
