#include "global.h"
/**
 * @brief 
 * SYNTAX: PRINT relation_name
 */

// function to perform operation based on the query type
bool syntacticParsePRINT()
{
    logger.log("syntacticParsePRINT");
    if (tokenizedQuery.size() == 2)
    {
        parsedQuery.queryType = PRINT;
        parsedQuery.printRelationName = tokenizedQuery[1];
        return true;

    }
    if (tokenizedQuery.size() == 3)
    {
        if (tokenizedQuery[1] != "MATRIX")
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
        parsedQuery.queryType = PRINT;
        parsedQuery.printMatrixName = tokenizedQuery[2];
        return true;
    }
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
}

// function to check whether the semantic content of the query is valid or not
bool semanticParsePRINT()
{
    logger.log("semanticParsePRINT");
    if (tokenizedQuery.size() == 2 && tableCatalogue.isTable(parsedQuery.printRelationName))
    {
        return true;
    }
    else if (tokenizedQuery.size() != 2 && matrixCatalogue.isMatrix(parsedQuery.printMatrixName))
    {
        return true;
    }
    cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
    return false;
}

// function to execute the parsed query
void executePRINT()
{
    logger.log("executePRINT");
    if (tokenizedQuery.size() == 3)
    {
        Matrix* matrix = matrixCatalogue.getMatrix(parsedQuery.printMatrixName);
        matrix->print();
        return;
    }
    Table* table = tableCatalogue.getTable(parsedQuery.printRelationName);
    table->print();
    return;
}