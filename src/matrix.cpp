#include "global.h"

/**
 * @brief Construct a new Matrix:: Matrix object
 *
 */
Matrix::Matrix()
{
    logger.log("Matrix::Matrix");
}

/**
 * @brief Construct a new Matrix:: Matrix object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param tableName 
 */
Matrix::Matrix(string matrixName)
{
    logger.log("Matrix::Matrix");
    this->matrixName = matrixName;
    this->sourceFileName = "../data/" + matrixName + ".csv";
}

/**
 * @brief Construct a new Matrix as a copy of another Matrix object
 *
 * @param tableName 
 */
Matrix::Matrix(Matrix *matrix)
{
    logger.log("Matrix::Matrix");
    this->blocksPerRow = matrix->blocksPerRow;
    this->columnCount = matrix->columnCount;
    this->isFirstChunk = matrix->isFirstChunk;
    this->isLastChunk = matrix->isLastChunk;
    this->matrixName = matrix->matrixName;
    this->maxColumnsPerBlock = matrix->maxColumnsPerBlock;
    this->rowCount = matrix->rowCount;
    this->sourceFileName = matrix->sourceFileName;
}

/**
 * @brief Function extracts column names from the header line of the .csv 
 *
 * @param line 
 * @return true if column names successfully extracted (i.e. no column name
 * repeats)
 * @return false otherwise
 */
bool Matrix::extractColumnNames(string firstLine)
{
    logger.log("Matrix::extractColumnNames");

    this->columnCount = 0;
    this->rowCount = 0;

    string word;
    stringstream s(firstLine);
    while (getline(s, word, ','))
    {
        this->columnCount += 1;
    }

    fstream fin(this->sourceFileName, ios::in);
    string line;
    
    while (getline(fin, line)){
        this->rowCount += 1;
    }
    if (this->rowCount != this->columnCount){
        return false;
    }
    fin.close();
    
    if ((unsigned int)((BLOCK_SIZE * 1000) / (32 * this->columnCount)) == 0){
        this->maxColumnsPerBlock = BLOCK_SIZE * 1000;
    }
    else{
        this->maxColumnsPerBlock = this->columnCount;
    }

    this->blocksPerRow = ceil((double)this->columnCount / this->maxColumnsPerBlock);

    return true;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size. 
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Matrix::blockify()
{
    logger.log("Matrix::blockify");
    ifstream fin(this->sourceFileName, ios::in);

    string line, word;
    vector<int> row(this->maxColumnsPerBlock, 0);
    vector<vector<int>> rowsInPage(1, row);

    int rowCounter = 0;
    while (getline(fin, line))
    {
        stringstream s(line);
        rowCounter += 1;
        int blockCounter = 0;
        while(blockCounter< this->blocksPerRow){
            int columnsForThisBlock = this->maxColumnsPerBlock;
            if (blockCounter == this->blocksPerRow - 1){
                columnsForThisBlock = this->columnCount % this->maxColumnsPerBlock;
            }
            if (columnsForThisBlock == 0){
                columnsForThisBlock = this->maxColumnsPerBlock;
            }
            int columnCounter = 0;
            while(columnCounter < columnsForThisBlock){
                if (!getline(s, word, ',')){
                    return false;
                }
                rowsInPage[0][columnCounter] = stoi(word);
                columnCounter += 1;
            }
            bufferManager.writePage(this->matrixName, to_string(rowCounter) + "_" + to_string(blockCounter + 1), rowsInPage, columnsForThisBlock);
            blockCounter++;
        }
    }

    if (this->rowCount != this->columnCount)
        return false;
    return true;
}

/**
 * @brief Function to check if table is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Matrix::isPermanent()
{
    logger.log("Matrix::isPermanent");
    if (this->sourceFileName == "../data/" + this->matrixName + ".csv")
        return true;
    return false;
}

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks
 *
 * @return true if the table has been successfully loaded 
 * @return false if an error occurred 
 */
bool Matrix::load()
{
    logger.log("Matrix::load");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if (getline(fin, line))
    {
        fin.close();
        if (!this->extractColumnNames(line)){
            return false;
        }
        if (!this->blockify())
        {
            return false;
        }
        return true;
    }
    fin.close();
    return false;
}
/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
void Matrix::makePermanent()
{
    logger.log("Matrix::makePermanent");
    bufferManager.deleteFile(this->sourceFileName);

    this->isFirstChunk = true;
    this->isLastChunk = false;
    
    int rowCounter = 1;
    while (rowCounter <= this->rowCount)
    {
        int blockCounter = 1;
        while (blockCounter <= this->blocksPerRow)
        {
            if (blockCounter == this->blocksPerRow)
                this->isLastChunk = true;
            vector<int> r = Page(this->matrixName, to_string(rowCounter) + "_" + to_string(blockCounter)).getRow(0);
            this->writeRow(r);
            this->isFirstChunk = false;
            blockCounter++;
        }
        this->isLastChunk = false;
        rowCounter++;
    }
}

/**
 * @brief Function to print the Matrix on the console. 
 *
 */
void Matrix::print()
{
    logger.log("Matrix::print");
    unsigned int count = min((long long)PRINT_COUNT, this->rowCount);
    int rowCounter = 1;
    while (rowCounter <= count)
    {
        int blockCounter = 1;
        while (blockCounter <= this->blocksPerRow)
        {
            vector<int> r = Page(this->matrixName, to_string(rowCounter) + "_" + to_string(blockCounter)).getRow(0);
            for (int columnCounter = 0; columnCounter < r.size(); columnCounter++)
            {
                cout << r[columnCounter] << " ";
            }
            blockCounter++;
        }
        cout << endl;
        rowCounter++;
    }
}