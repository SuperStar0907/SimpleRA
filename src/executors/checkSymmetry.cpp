#include "global.h"

// function to perform operation based on the query type
bool syntacticParseCHECKSYMMETRY()
{
    logger.log("syntacticParseCHECKSYMMETRY");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = CHECKSYMMETRY;
    parsedQuery.loadMatrixName = tokenizedQuery[1];
    return true;
}

// function to check whether the semantic content of the query is valid or not
bool semanticParseCHECKSYMMETRY()
{
    logger.log("semanticParseCHECKSYMMETRY");
    if (!matrixCatalogue.isMatrix(parsedQuery.loadMatrixName))
    {
        cout << "SEMANTIC ERROR: Matrix doesnt exists" << endl;
        return false;
    }

    return true;
}

// function to execute the parsed query
void executeCHECKSYMMETRY()
{
    logger.log("executeCHECKSYMMETRY");
    Matrix *matrix = matrixCatalogue.getMatrix(parsedQuery.loadMatrixName);

    int rowCounter=1;
    unordered_map<string,int> blocksRead;
    // iterate over upper triangle the rows and columns of the matrix find page and retrieve elements and then swap them
    while(rowCounter<=matrix->rowCount){
        int columnCounter = rowCounter + 1;
        while(columnCounter<=matrix->columnCount){
            int blockNumber1, blockNumber2;
            if (columnCounter % matrix->maxColumnsPerBlock == 0){
                blockNumber1 = columnCounter / matrix->maxColumnsPerBlock;
            }
            else{
                blockNumber1 = (columnCounter / matrix->maxColumnsPerBlock) + 1;
            }

            if (rowCounter % matrix->maxColumnsPerBlock == 0){
                blockNumber2 = rowCounter / matrix->maxColumnsPerBlock;
            }
            else{
                blockNumber2 = (rowCounter / matrix->maxColumnsPerBlock) + 1;
            }

            int idx1, idx2;
            idx1 = (columnCounter - 1) % matrix->maxColumnsPerBlock;
            idx2 = (rowCounter - 1) % matrix->maxColumnsPerBlock;

            vector<int> row1 = Page(matrix->matrixName, to_string(rowCounter) + "_" + to_string(blockNumber1)).getRow(0);
            vector<int> row2 = Page(matrix->matrixName, to_string(columnCounter) + "_" + to_string(blockNumber2)).getRow(0);
            blocksRead[to_string(rowCounter) + "_" + to_string(blockNumber1)]++;
            blocksRead[to_string(columnCounter) + "_" + to_string(blockNumber2)]++;
            if (row1[idx1] != row2[idx2])
            {
                cout << "NO" << endl;
                cout<< "Number of blocks read: " << blocksRead.size() << endl;
                cout<< "Number of blocks written: " << 0 << endl;
                cout<< "Number of blocks accessed: " << blocksRead.size() << endl;
                return;
            }
            
            columnCounter++;
        }

        rowCounter++;
    }
    cout << "YES" << endl;
    cout<< "Number of blocks read: " << blocksRead.size() << endl;
    cout<< "Number of blocks written: " << 0 << endl;
    cout<< "Number of blocks accessed: " << blocksRead.size() << endl;
    return;
}