#include "global.h"

/**
 * @brief 
 * SYNTAX: EXPORT <relation_name> 
 */

// function to perform operation based on the query type
bool syntacticParseEXPORT()
{
    logger.log("syntacticParseEXPORT");
    if (tokenizedQuery.size() == 2)
    {
        parsedQuery.queryType = EXPORT;
        parsedQuery.exportRelationName = tokenizedQuery[1];
        return true;
    }
    else if (tokenizedQuery.size() == 3)
    {
        if (tokenizedQuery[1] != "MATRIX")
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
        parsedQuery.queryType = EXPORT;
        parsedQuery.exportMatrixName = tokenizedQuery[2];
        return true;
    }
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
}

// function to check whether the semantic content of the query is valid or not
bool semanticParseEXPORT()
{
    logger.log("semanticParseEXPORT");
    //Table should exist
    if (tokenizedQuery.size() == 2 && tableCatalogue.isTable(parsedQuery.exportRelationName))
    {
        return true;
    }
    else if (tokenizedQuery.size() != 2 && matrixCatalogue.isMatrix(parsedQuery.exportMatrixName))
    {
        return true;
    }
    cout << "SEMANTIC ERROR: No such relation exists" << endl;
    return false;
}

// function to execute the parsed query
void executeEXPORT()
{
    logger.log("executeEXPORT");
    if (tokenizedQuery.size() == 2)
    {
        Table* table = tableCatalogue.getTable(parsedQuery.exportRelationName);
        table->makePermanent();
    }
    else
    {
        Matrix *matrix = matrixCatalogue.getMatrix(parsedQuery.exportMatrixName);
        matrix->makePermanent();
    }
    return;
}