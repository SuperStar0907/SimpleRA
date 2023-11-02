#include "global.h"

// function to perform operation based on the query type
bool syntacticParseCOMPUTE()
{
    logger.log("syntacticParseCOMPUTE");
    if (tokenizedQuery.size() != 2)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = COMPUTE;
    parsedQuery.loadMatrixName = tokenizedQuery[1];
    return true;
}

// function to check whether the semantic content of the query is valid or not
bool semanticParseCOMPUTE()
{
    logger.log("semanticParseCOMPUTE");
    if (!matrixCatalogue.isMatrix(parsedQuery.loadMatrixName))
    {
        cout << "SEMANTIC ERROR: Matrix doesnt exists" << endl;
        return false;
    }

    return true;
}

// function to execute the parsed query
void executeCOMPUTE()
{
    logger.log("executeCOMPUTE");
    Matrix *originalMatrix = matrixCatalogue.getMatrix(parsedQuery.loadMatrixName);
    unordered_map<string,int> blocksRead;
    unordered_map<string,int> blocksWritten;
    int rowCounter=1;
    // iterate over upper triangle the rows and columns of the matrix find page and retrieve elements and then swap them
    while(rowCounter<=originalMatrix->rowCount){
        int columnCounter = rowCounter + 1;
        while(columnCounter<=originalMatrix->columnCount){
            int blockNumber1, blockNumber2;
            if (columnCounter % originalMatrix->maxColumnsPerBlock == 0){
                blockNumber1 = columnCounter / originalMatrix->maxColumnsPerBlock;
            }
            else{
                blockNumber1 = (columnCounter / originalMatrix->maxColumnsPerBlock) + 1;
            }

            if (rowCounter % originalMatrix->maxColumnsPerBlock == 0){
                blockNumber2 = rowCounter / originalMatrix->maxColumnsPerBlock;
            }
            else{
                blockNumber2 = (rowCounter / originalMatrix->maxColumnsPerBlock) + 1;
            }

            int idx1, idx2;
            idx1 = (columnCounter - 1) % originalMatrix->maxColumnsPerBlock;
            idx2 = (rowCounter - 1) % originalMatrix->maxColumnsPerBlock;

            vector<int> row1 = Page(originalMatrix->matrixName, to_string(rowCounter) + "_" + to_string(blockNumber1)).getRow(0);
            vector<int> row2 = Page(originalMatrix->matrixName, to_string(columnCounter) + "_" + to_string(blockNumber2)).getRow(0);
            blocksRead[to_string(rowCounter) + "_" + to_string(blockNumber1)]++;
            blocksRead[to_string(columnCounter) + "_" + to_string(blockNumber2)]++;
            
            swap(row1[idx1], row2[idx2]);
            
            Page page1(originalMatrix->matrixName, to_string(rowCounter) + "_" + to_string(blockNumber1), vector<vector<int>>(1, row1), row1.size());
            Page page2(originalMatrix->matrixName, to_string(columnCounter) + "_" + to_string(blockNumber2), vector<vector<int>>(1, row2), row2.size());
            page1.writePage();
            page2.writePage();
            blocksWritten[to_string(rowCounter) + "_" + to_string(blockNumber1)]++;
            blocksWritten[to_string(columnCounter) + "_" + to_string(blockNumber2)]++;

            columnCounter++;
        }

        rowCounter++;
    }

    // make a copy of originalMatrix in resultMatrix
    Matrix *resultMatrix = new Matrix(originalMatrix);
    resultMatrix->matrixName = resultMatrix->matrixName + "_RESULT";
    resultMatrix->sourceFileName = resultMatrix->sourceFileName.substr(0,originalMatrix->sourceFileName.size()-4) + "_RESULT" + ".csv";
    matrixCatalogue.insertMatrix(resultMatrix);

    rowCounter=1;
    while(rowCounter<=originalMatrix->rowCount){
        int blockCounter=1;
        while(blockCounter<=originalMatrix->maxColumnsPerBlock){
            vector<int> row = Page(originalMatrix->matrixName, to_string(rowCounter) + "_" + to_string(blockCounter)).getRow(0);
            Page page(resultMatrix->matrixName, to_string(rowCounter) + "_" + to_string(blockCounter), vector<vector<int>>(1, row), row.size());
            page.writePage();
            blocksWritten[to_string(rowCounter) + "_" + to_string(blockCounter)]++; 
            blockCounter++;
        }
        rowCounter++;
    }
    
    // functionality for transpose of matrix
    rowCounter=1;
    // iterate over upper triangle the rows and columns of the matrix find page and retrieve elements and then swap them
    while(rowCounter<=originalMatrix->rowCount){
        int columnCounter = rowCounter + 1;
        while(columnCounter<=originalMatrix->columnCount){
            int blockNumber1, blockNumber2;
            if (columnCounter % originalMatrix->maxColumnsPerBlock == 0){
                blockNumber1 = columnCounter / originalMatrix->maxColumnsPerBlock;
            }
            else{
                blockNumber1 = (columnCounter / originalMatrix->maxColumnsPerBlock) + 1;
            }

            if (rowCounter % originalMatrix->maxColumnsPerBlock == 0){
                blockNumber2 = rowCounter / originalMatrix->maxColumnsPerBlock;
            }
            else{
                blockNumber2 = (rowCounter / originalMatrix->maxColumnsPerBlock) + 1;
            }

            int idx1, idx2;
            idx1 = (columnCounter - 1) % originalMatrix->maxColumnsPerBlock;
            idx2 = (rowCounter - 1) % originalMatrix->maxColumnsPerBlock;

            vector<int> row1 = Page(originalMatrix->matrixName, to_string(rowCounter) + "_" + to_string(blockNumber1)).getRow(0);
            vector<int> row2 = Page(originalMatrix->matrixName, to_string(columnCounter) + "_" + to_string(blockNumber2)).getRow(0);
            blocksRead[to_string(rowCounter) + "_" + to_string(blockNumber1)]++;
            blocksRead[to_string(columnCounter) + "_" + to_string(blockNumber2)]++;

            swap(row1[idx1], row2[idx2]);
            
            Page page1(originalMatrix->matrixName, to_string(rowCounter) + "_" + to_string(blockNumber1), vector<vector<int>>(1, row1), row1.size());
            Page page2(originalMatrix->matrixName, to_string(columnCounter) + "_" + to_string(blockNumber2), vector<vector<int>>(1, row2), row2.size());
            page1.writePage();
            page2.writePage();
            blocksWritten[to_string(rowCounter) + "_" + to_string(blockNumber1)]++;
            blocksWritten[to_string(columnCounter) + "_" + to_string(blockNumber2)]++;

            columnCounter++;
        }

        rowCounter++;
    }

    // final subtraction of matrix
    rowCounter=1;
    while (rowCounter <= originalMatrix->rowCount)
    {
        int blockCounter = 1;
        while (blockCounter <= originalMatrix->maxColumnsPerBlock)
        {
            vector<int> r1 = Page(originalMatrix->matrixName, to_string(rowCounter) + "_" + to_string(blockCounter)).getRow(0);
            vector<int> r2 = Page(resultMatrix->matrixName, to_string(rowCounter) + "_" + to_string(blockCounter)).getRow(0);

            for (int i = 0; i < r1.size(); i++)
            {
                r2[i] = r1[i] - r2[i];
            }

            Page p1(resultMatrix->matrixName, to_string(rowCounter) + "_" + to_string(blockCounter), vector<vector<int>>(1, r2), r2.size());
            p1.writePage();
            blocksWritten[to_string(rowCounter) + "_" + to_string(blockCounter)]++;

            blockCounter++;
        }
        rowCounter++;
    }
    cout<< "Number of blocks read: " << blocksRead.size() << endl;
    cout<< "Number of blocks written: " << blocksWritten.size() << endl;
    cout<< "Number of blocks accessed: " << blocksRead.size() + blocksWritten.size()<< endl;
    return;
}