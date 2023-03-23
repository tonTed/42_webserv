# include "../../includes/webserv.hpp"

std::string getline_with_newline(std::istream& input) 
{
    std::string line;
    char c;
    while (input.get(c)) {
        line += c;
        if (c == '\n') {
            break;
        }
    }
    return line;
}

bool isrealspace(char c)
{
    return (c == ' ' || c == '\t' || c == '\v' || c == '\f' || c == '\r');
}

bool validPort(int portNumb)
{
    return (portNumb > 0 && portNumb < 65535);
}