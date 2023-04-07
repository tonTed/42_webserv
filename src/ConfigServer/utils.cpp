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
 * @brief Get the Host Line object
 *
 * @param str the string to get the Hostline from
 * @param startPos the position to start from
 * @param newPos the posiion of the last (';')
 * @return std::string the string line fron start to end
 */
std::string getHostLine(const std::string &str,
                        string::size_type &startPos, string::size_type *newPos)
{
    startPos += 6;
    string::size_type endPos = str.find(";", startPos);
    if (endPos == string::npos)
        return "";
    string::size_type colonPos = str.find_last_of(":", startPos, endPos - startPos);
    if (colonPos != string::npos && startPos - 6 < colonPos && colonPos < endPos)
    {
        if (newPos)
            *newPos = endPos;
        return str.substr(startPos, endPos - startPos);
    }
    else
        return "";
}

/**
 * @brief Get the Host Part object
 *
 * @param input the string to get the host part from
 * @return std::string the host part
 */
std::string getHostPart(const std::string &input)
{
    std::stringstream ss(input);
    std::string token;

    while (std::getline(ss, token, ' '))
    {
        std::string::size_type pos = token.find(':');
        if (pos != std::string::npos)
        {
            std::string host = token.substr(0, pos);
            return host;
        }
    }
    return "";
}

/**
 * @brief Get the Port Line object
 *
 * @param str the string to get the Portline from
 * @param startPos the position to start from
 * @param newPos the posiion of the last (';')
 * @return std::string the string line fron start to end
 */
std::string getPortLine(const std::string &str,
                        string::size_type &startPos, string::size_type *newPos)
{
    startPos += 6;
    string::size_type endPos = str.find(";", startPos);
    if (endPos == string::npos)
        return "";
    string::size_type colonPos = str.find_last_of(":", startPos, endPos - startPos);
    if (colonPos != string::npos && startPos - 6 < colonPos && colonPos < endPos)
    {
        if (newPos)
            *newPos = endPos;
        // std::cout << "    "; // TODO How the hell is this even possible??
        return str.substr(startPos, endPos - startPos);
    }
    else if (colonPos != string::npos)
        {
            if (newPos)
            *newPos = endPos;

        return str.substr(startPos, endPos - startPos);
        }
    else
        return "";
}

/**
 * @brief Get the Port Part object
 *
 * @param input the string to get the port part from
 * @return std::string the port part
 */
std::string getPortPart(const std::string &input)
{
    std::stringstream ss(input);
    std::string token;

    while (std::getline(ss, token, ' '))
    {
        std::string::size_type pos = token.find(':');
        if (pos != std::string::npos)
        {
            std::string port = token.substr(pos + 1);
            return port;
        }
        else if (pos == std::string::npos)
        {
            std::string port = token.substr(pos + 1);
            return port;
        }
    }
    return "";
}

/**
 * @brief Get the Methods Line object
 *
 * @param str the string to get the Methodsline from
 * @param startPos the position to start from
 * @param newPos the posiion of the last (';')
 * @return std::string the string line fron start to end
 */
std::string getMethodsLine(const std::string &str,
                           string::size_type &startPos, string::size_type *newPos)
{
    startPos += 8;
    string::size_type endPos = str.find(";", startPos);
    if (endPos != string::npos)
    {
        if (newPos)
            *newPos = endPos;
        return str.substr(startPos, endPos - startPos);
    }
    else
        return "";
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
 * @brief check if the error pages are valid
 *
 * @param input the error page string
 * @return true if the input is valid
 * @return false if the input is not valid
 */
bool validErrorPage(std::string input)
{
    if (static_cast<int>(input.size()) < 5 ||
        input.substr(static_cast<int>(input.size()) - 5) != ".html")
        return false;
    return true;
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
    {
        return "";
    }
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
    // std::vector<std::string> indexes = input.index;
    std::string path = "";
    for (int i = 0; i < (int)input.index.size(); i++)
    {

        if (input.index[i].size() < 4 && (input.index[i].substr(input.index[i].size() - 5) != ".html" 
            || input.index[i].substr(input.index[i].size() - 4) != ".htm"))
            exit_error("Error:: ", input.index[i] + " is not a valid index!");
        if(input.root == "/" || input.root == "/root")
            path = "/";
        // else
        //     path = path.erase(0, 1) + "/";
        std::cout << "input.index[i]: " << input.index[i] << std::endl;
        if(!validFilePath(path + input.index[i]))
            exit_error("Error: index path:-: |", (path + input.index[i]) + "| not valid");

    }

}

/**
 * @brief check if the redirection is valid
 * 
 * @param input The redirection
 * @return true if valid
 * @return false if not valid
 */
// bool validRedirection(std::string &input)
// {
//     return(true);
// }

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

/**
 * @brief check if the client body size is valid
 * 
 * @param input The client body size
 * @return true if valid
 * @return false if not valid
 */
// bool validClientBodySize(std::string input)
// {
//     if (input.size() < 1)
//         return false;
//     for (int i = 0; i < (int)input.size(); i++)
//     {
//         if (!std::isdigit(input[i]))
//             return false;
//     }
//     return true;
// }


/**
 * @brief check if the file path is valid
 * 
 * @param input The file path
 * @return true if valid
 * @return false if not valid
 */
bool validFilePath(std::string input)
{
    // std::cout << "Input path: " << input << std::endl;
    std::ifstream file(input.c_str());
    return file.good();
}


// data/www; #data/www/  /

bool validRoot(std::string input)
{
    // TODO check the path is valid!
    // std::cout << "input:: " << input << std::endl;
    if (input.size() < 1)
        return false;
    if (input[0] == '/' && input.size() > 1)
        return false;
    if (input[1] == '/')
        return false;
    return true;
}