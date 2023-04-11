#include "../../includes/webserv.hpp"

/**
 * @brief Check if the line is neeeded and not just a comment or just spaces
 *
 * @param file The path to the file
 * @return true if not just a comment or just spaces
 * @return false otherwise
 */
bool lineNeeded(const std::string line)
{
    size_t i = line.find_first_not_of(" \t");
    bool val = (i != string::npos && line[i] != '#');
    return val;
}

/**
 * @brief Get the line without newline
 *
 * @param input the string to parse
 * @return std::string without (/n)
 */
std::string getline_with_newline(std::istream &input)
{
    std::string line;
    char c;
    while (input.get(c))
    {
        line += c;
        if (c == '\n')
            break;
    }
    return line;
}

/**
 * @brief check if the char is a (/n)
 *
 * @param c the caracter to check
 * @return true if it not a (/n)
 * @return false if it's a (/n)
 */
bool isrealspace(char c)
{
    return (c == ' ' || c == '\t' || c == '\v' || c == '\f' || c == '\r');
}

/**
 * @brief Print an error and exit
 *
 * @param str the error string
 * @param err the error object
 */
void exit_error(string str, string err)
{
    std::cout << BOLD_RED << str << err << RESET << std::endl;
    exit(1); // TODO fix the error!
}

/**
 * @brief Validate that the braces are even
 *
 * @param str The string to check
 * @return true if the braces are even
 * @return false if they're odd
 */
bool validBraces(const string &str)
{
    int count = 0;
    for (size_t i = 0; i < str.size(); i++)
    {
        if (str[i] == '{')
            count++;
        else if (str[i] == '}')
            count--;
    }
    return (count == 0);
}

/**
 * @brief Check if the port is valid port number
 *
 * @param portNumb the port to check
 * @return true if it's a valid port number
 * @return false if the port number is not valid
 */
bool validPort(int portNumb)
{
    return (portNumb == 80 || (portNumb > 1024 && portNumb < 65535));
}

/**
 * @brief Check if the host is valid
 *
 * @param host the host to check
 * @return true if the host is valid
 * @return false if the host is not valid
 */
bool validHost(std::string host)
{
    return (host == "127.0.0.1" || host == "localhost");
}

/**
 * @brief Check if the server name is valid
 *
 * @param serverName the server name to check
 * @return true if the server name is valid
 * @return false if the server name is not valid
 */
bool validServerName(std::string serverName) // TODO check if the server name is duplicate in 2 diffrent servers
{
    return (serverName != "");
}

/**
 * @brief check if the error pages are valid
 *
 * @param input the error page string
 * @return true if the input is valid
 * @return false if the input is not valid
 */
bool validErrorPage(int code, std::string input)
{ 
     if (static_cast<int>(input.size()) < 5 ||
        input.substr(static_cast<int>(input.size()) - 5) != ".html")
        return false;
    if (code < 400 || code > 599)
        return false;
    return true; // TODO add the path checking 
}







/**
 * @brief check if the file path is valid
 *
 * @param input The file path
 * @return true if valid
 * @return false if not valid
 */
bool validFilePath(std::string input)
{
    std::ifstream file(input.c_str());
    return file.good();
}

bool validRoot(std::string input)
{
    size_t i = 0;
    if (input.size() < 1)
        return false;
    if (input[0] == '/' && input.size() > 1)
        return false;
    while (i < input.size())
    {
        if (!std::isalpha(input[i]) && input[i] != '/')
            return false;
        if (input[i] == '/' && input[i + 1] == '/')
            return false;
        i++;
    }
    return true;
}

std::string format_string(const std::string &str)
{
    // std::cout << std::endl <<  "str: " << str << std::endl << std::endl;
    if (!validRoot(str))
        exit_error("Error: Bad Root |", str + "|");

    std::string result = str;

    if (result.size() == 1 && result[0] == '/')
        return result;
    else if (result.size() > 1)
    {
        while (result[result.size() - 1] == '/')
            result.erase(result.size() - 1);
    }
    if (!validFilePath(result))
        exit_error("Error: Bad Root Path |", result + "|");
    return result;
}

std::vector<enum eRequestType> validMethods(std::vector<std::string> words)
{
    std::vector<enum eRequestType> methods;
    for (int i = 0; i < static_cast<int>(words.size()); i++) // loop until all words have been returned
    {
        if (words[i].empty())
            exit_error("Error: methods can't be empty!", "");
        else if (words[i] == "GET")
            methods.push_back(GET);
        else if (words[i] == "POST")
            methods.push_back(POST);
        else if (words[i] == "DELETE")
            methods.push_back(DELETE);
        else
            exit_error("Error: Invalid HTTP method: ", words[i]);
    }
    return methods;
}

/**
 * @brief Check if the port is duplicated
 *
 * @param seq the vector of port to check
 * @return true if the port is duplicated
 * @return false if the port is not duplicated
 */
bool portDup(std::vector<int> seq)
{
    for (int i = 0; i < static_cast<int>(seq.size()); i++)
    {
        for (int j = i + 1; j < static_cast<int>(seq.size()); j++)
        {
            if (seq[i] == seq[j])
                return true;
        }
    }
    return false;
}

/**
 * @brief check if path is duplicated
 *
 * @param seq the vector of the location paths
 * @return true if it's duplicat
 * @return false if not duplicate
 */
bool pathDup(std::vector<std::string> seq)
{
    for (int i = 0; i < static_cast<int>(seq.size()); i++)
    {
        for (int j = i + 1; j < static_cast<int>(seq.size()); j++)
        {
            if (seq[i] == seq[j])
                return true;
        }
    }
    return false;
}






/**
 * @brief Check if the client size is valid
 *
 * @param clientSize the client size to check
 * @return true if the client size is valid
 * @return false if the client size is not valid
 */
bool validClientSize(int clientSize)
{
    return (clientSize > 0);
}

/**
 * @brief Check if the body size is valid
 *
 * @param bodySize the body size to check
 * @return true if the body size is valid
 * @return false if the body size is not valid
 */
bool validBodySize(int bodySize)
{
    return (bodySize > 0);
}



/**
 * @brief trim a tring from the spaces
 *
 * @param str the string to trim
 * @return std::string the string without the spaces
 */
std::string trim(const std::string &str)
{
    std::string::size_type start = str.find_first_not_of(" ");
    if (start == std::string::npos)
        return "";
    std::string::size_type end = str.find_last_not_of(" ");
    return str.substr(start, end - start + 1);
}

/**
 * @brief split the string into parts and return it in a vector of strings
 *
 * @param input the string to split
 * @return std::vector<std::string> the splitted string
 */
std::vector<std::string> splitString(std::string input)
{
    std::vector<std::string> result;
    std::istringstream iss(input);
    std::string word;
    while (iss >> word)
    {
        result.push_back(word);
    }
    return result;
}


/**
 * @brief check if the path is valid
 *
 * @param path the path to check
 * @return true if the path is valid
 * @return false if the path is not valid
 */
bool lactionPathValid(string &path)
{
    int i = 1;
    if (path.size() < 1)
        return false;
    if (path[0] != '/')
        return false;
    if (path[1] == '/')
        return false;
    while (path[i])
        if (!std::isalpha(path[i++]))
            return (false);
    return true;
}

/**
 * @brief Get the Location Path object from the location block
 *
 * @param locationBlock the location block to get the path from
 * @return std::string the path of the location
 */
std::string getLocationPath(const std::string &locationBlock)
{
    std::vector<std::string> paths;
    std::string::size_type pos = locationBlock.find("location");

    while (pos != std::string::npos)
    {
        pos = locationBlock.find("/", pos);
        std::string::size_type end_pos = locationBlock.find(" ", pos);

        if (pos != std::string::npos && end_pos != std::string::npos)
        {
            std::string path = locationBlock.substr(pos, end_pos - pos);
            if (!lactionPathValid(path))
                exit_error("Error: Invalid path: |", path);
            paths.push_back(path);
        }

        pos = locationBlock.find("location", end_pos);
    }

    if (paths.empty())
        return "";
    else
    {
        std::string result = paths[0];
        for (int i = 1; i < (int)paths.size(); i++)
        {
            result += paths[i];
        }
        return result;
    }
}

/**
 * @brief check if the indexes are valid
 *
 * @param input The list of indexes
 * @return true if valid
 * @return false if not valid
 */
void validIndex(Locations &input)
{
    // Check if the index + root is valid
    std::string path = "";
    for (int i = 0; i < (int)input.index.size(); i++)
    {
        if (input.index[i].size() < 4 && (input.index[i].substr(input.index[i].size() - 5) != ".html" || input.index[i].substr(input.index[i].size() - 4) != ".htm"))
            exit_error("Error:: ", input.index[i] + " is not a valid index!");
        if (input.root == "/")
            path = "";
        else
            path = input.root;
        if (!validFilePath(path + "/" + input.index[i]))
            exit_error("Error: index path:-: |", (path + input.index[i]) + "| not valid");
    }
}

/**
 * @brief check if the autoindex is valid
 *
 * @param input The autoindex
 * @return true if valid
 * @return false if not valid
 */
bool validAutoindex(std::string input)
{
    if (input != "on" && input != "off")
        return false;
    return true;
}








