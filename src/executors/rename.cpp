#include "global.h"
/**
 * @brief 
 * SYNTAX: RENAME column_name TO column_name FROM relation_name
 */
bool syntacticParseRENAME()
{
    logger.log("syntacticParseRENAME");
    if(tokenizedQuery.size()==6){
        if (tokenizedQuery[2] != "TO" || tokenizedQuery[4] != "FROM")
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
        parsedQuery.queryType = RENAME;
        parsedQuery.renameFromColumnName = tokenizedQuery[1];
        parsedQuery.renameToColumnName = tokenizedQuery[3];
        parsedQuery.renameRelationName = tokenizedQuery[5];
        return true;
    }
    else{
        if (tokenizedQuery.size() != 4 || tokenizedQuery[1] != "MATRIX")
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
        parsedQuery.queryType = RENAME;
        parsedQuery.renameFromMatrixName = tokenizedQuery[2];
        parsedQuery.renameToMatrixName = tokenizedQuery[3];
        return true;
    }
   
}

bool semanticParseRENAME()
{
    logger.log("semanticParseRENAME");
    if (tokenizedQuery.size() == 4)
    {
        if (!matrixCatalogue.isMatrix(parsedQuery.renameFromMatrixName))
        {
            cout << "SEMANTIC ERROR: Matrix doesn't exist" << endl;
            return false;
        }

        if (matrixCatalogue.isMatrix(parsedQuery.renameToMatrixName))
        {
            cout << "SEMANTIC ERROR: Matrix with name already exists" << endl;
            return false;
        }
        return true;
    }
    if (!tableCatalogue.isTable(parsedQuery.renameRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.renameFromColumnName, parsedQuery.renameRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }

    if (tableCatalogue.isColumnFromTable(parsedQuery.renameToColumnName, parsedQuery.renameRelationName))
    {
        cout << "SEMANTIC ERROR: Column with name already exists" << endl;
        return false;
    }
    return true;
}

void executeRENAME()
{
    logger.log("executeRENAME");
    if (tokenizedQuery.size() == 4)
    {
        Matrix *originalMatrix = matrixCatalogue.getMatrix(parsedQuery.renameFromMatrixName);
        Matrix *resultMatrix = new Matrix(originalMatrix);
        resultMatrix->matrixName = parsedQuery.renameToMatrixName;
        resultMatrix->sourceFileName = "../data/" + parsedQuery.renameToMatrixName + ".csv";
        matrixCatalogue.insertMatrix(resultMatrix);

        for (int rowCounter = 1; rowCounter <= originalMatrix->rowCount; rowCounter++)
        {
            vector<int> r1 = Page(originalMatrix->matrixName, to_string(rowCounter) + "_1").getRow(0);
            
            Page p1(resultMatrix->matrixName, to_string(rowCounter) + "_1", vector<vector<int>>(1, r1), r1.size());
            p1.writePage();
        }

        for (int rowCounter = 1; rowCounter <= originalMatrix->rowCount; rowCounter++)
        {
            for (int blockCounter = 1; blockCounter <= originalMatrix->blocksPerRow; blockCounter++)
            {
                string fileName = "../data/temp/Matrix/" + originalMatrix->matrixName + "_BLOCK" +to_string(rowCounter) + "_" + to_string(blockCounter) ;
                bufferManager.deleteFile(fileName);
            }
        }
        matrixCatalogue.deleteMatrix(parsedQuery.renameFromMatrixName);
        return;
    }
    Table* table = tableCatalogue.getTable(parsedQuery.renameRelationName);
    table->renameColumn(parsedQuery.renameFromColumnName, parsedQuery.renameToColumnName);
    return;
}