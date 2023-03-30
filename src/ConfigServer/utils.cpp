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

bool validClientSize(int clientSize)
{
    return (clientSize > 0);
}

bool validBodySize(int bodySize)
{
    return (bodySize > 0);
}

bool validServerName(std::string serverName)
{
    return (serverName != "");
}

bool validHost(std::string host)
{
    return (host == "127.0.0.1" || host == "localhost");
}


std::string trim(const std::string& str)
{
    std::string::size_type start = str.find_first_not_of(" ");
    if (start == std::string::npos)
        return "";

    std::string::size_type end = str.find_last_not_of(" ");
    return str.substr(start, end - start + 1);
}

string::size_type findNextPos(const std::string& str, string::size_type startPos, char c)
{
    string::size_type pos = str.find(c, startPos);
    if (pos == string::npos)
        return string::npos;
    return pos + 1;
}