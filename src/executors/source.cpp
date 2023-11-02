#include "global.h"
/**
 * @brief
 * SYNTAX: SOURCE filename
 */
bool syntacticParseSOURCE()
{
    logger.log("syntacticParseSOURCE");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = SOURCE;
    parsedQuery.sourceFileName = tokenizedQuery[1];
    return true;
}

bool semanticParseSOURCE()
{
    logger.log("semanticParseSOURCE");
    if (!isQueryFile(parsedQuery.sourceFileName))
    {
        cout << "SEMANTIC ERROR: File doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeSOURCE()
{
    logger.log("executeSOURCE");
    const string sourceFileName = "../data/" + parsedQuery.sourceFileName + ".ra";
    const regex delim("[^\\s,]+");

    fstream fin(sourceFileName, ios::in);
    string command;

    while (getline(fin, command))
    {
        parsedQuery.clear();
        tokenizedQuery.clear();

        for (std::sregex_iterator i = std::sregex_iterator(command.begin(), command.end(), delim); i != std::sregex_iterator(); ++i)
            tokenizedQuery.emplace_back((*i).str());

        if (!tokenizedQuery.empty())
        {
            if (tokenizedQuery.size() == 1)
            {
                if(tokenizedQuery.front() == "QUIT"){
                    exit(0);
                }
                cout << "SYNTAX ERROR" << endl;
                continue;
            }
            if (syntacticParse() && semanticParse())
                executeCommand();
        }
    }
    fin.close();
    return;
}
