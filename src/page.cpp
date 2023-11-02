#include "global.h"
/**
 * @brief Construct a new Page object. Never used as part of the code
 *
 */
Page::Page()
{
    this->pageName = "";
    this->tableName = "";
    this->pageIndex = -1;
    this->rowCount = 0;
    this->columnCount = 0;
    this->rows.clear();
}

/**
 * @brief Construct a new Page:: Page object given the table name and page
 * index. When tables are loaded they are broken up into blocks of BLOCK_SIZE
 * and each block is stored in a different file named
 * "<tablename>_Page<pageindex>". For example, If the Page being loaded is of
 * table "R" and the pageIndex is 2 then the file name is "R_Page2". The page
 * loads the rows (or tuples) into a vector of rows (where each row is a vector
 * of integers).
 *
 * @param tableName 
 * @param pageIndex 
 */
Page::Page(string tableName, int pageIndex)
{
    logger.log("Page::Page");
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->pageName = "../data/temp/" + this->tableName + "_Page" + to_string(pageIndex);
    Table table = *tableCatalogue.getTable(tableName);
    this->columnCount = table.columnCount;
    unsigned int maxRowCount = table.maxRowsPerBlock;
    vector<int> row(columnCount, 0);
    this->rows.assign(maxRowCount, row);

    ifstream fin(pageName, ios::in);
    this->rowCount = table.rowsPerBlockCount[pageIndex];
    int number;
    for (unsigned int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < columnCount; columnCounter++)
        {
            fin >> number;
            this->rows[rowCounter][columnCounter] = number;
        }
    }
    fin.close();
}
/**
 * @brief Construct a new Page:: Page object given the matrix name, row number
 * and block number. When matrices are loaded they are broken up into blocks of
 * BLOCK_SIZE and each block is stored in a different file named
 * "<matrixname>_Block<blocknumber>". For example, If the Page being loaded is
 * of matrix "M" and the row number is 2 and block number is 3 then the file
 * name is "M_Block3". The page loads the rows (or tuples) into a vector of rows
 * (where each row is a vector of integers).
 *
 * @param matrixName 
 * @param rowNumber 
 * @param blockNumber 
 */
Page::Page(string matrixName, string blockName)
{
    this->tableName = matrixName;
    this->pageName = "../data/temp/Matrix/" + this->tableName + "_Block" + blockName;
    this->rows.assign(1, vector<int>());
    this->rowCount = 1;
    this->columnCount = 0;
    ifstream fin(this->pageName, ios::in);
    string line;
    if (getline(fin, line))
    {
        stringstream s(line);
        string word;
        while (getline(s, word, ' '))
        {
            rows[0].push_back(stoi(word));
            this->columnCount += 1;
        }
    }

    fin.close();
}
/**
 * @brief Get row from page indexed by rowIndex
 * 
 * @param rowIndex 
 * @return vector<int> 
 */
vector<int> Page::getRow(int rowIndex)
{
    logger.log("Page::getRow");
    vector<int> result;
    result.clear();
    if (rowIndex >= this->rowCount)
        return result;
    return this->rows[rowIndex];
}

Page::Page(string tableName, int pageIndex, vector<vector<int>> rows, int rowCount)
{
    logger.log("Page::Page");
    this->tableName = tableName;
    this->pageIndex = pageIndex;
    this->rows = rows;
    this->rowCount = rowCount;
    this->columnCount = rows[0].size();
    this->pageName = "../data/temp/"+this->tableName + "_Page" + to_string(pageIndex);
}

Page::Page(string matrixName, string blockName, vector<vector<int>> rows, int columnCount)
{
    logger.log("Page::Page");
    this->tableName = matrixName;
    this->rows = rows;
    this->rowCount = 1;
    this->columnCount = columnCount;
    this->pageName = "../data/temp/Matrix/" + this->tableName + "_Block" + blockName;
}
/**
 * @brief writes current page contents to file.
 * 
 */
void Page::writePage()
{
    logger.log("Page::writePage");
    ofstream fout(this->pageName, ios::trunc);
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (columnCounter != 0)
                fout << " ";
            fout << this->rows[rowCounter][columnCounter];
        }
        fout << endl;
    }
    fout.close();
}

/**
 * @brief clears the page contents
 * 
 */
void Page::clear()
{
    logger.log("Page::clear");
    this->rows.clear();
    this->rowCount = 0;
    this->columnCount = 0;
}

/**
 * @brief adds a row to the page
 * 
 * @param row 
 */
void Page::addRow(vector<int> row)
{
    logger.log("Page::addRow");
    this->rows.push_back(row);
    this->rowCount += 1;
    this->columnCount = row.size();
}

/**
 * @brief Get all the rows
 * 
 * @param tableName 
 * @return int 
 */
vector<vector<int>> Page::getRows()
{
    logger.log("Page::getRows");
    return this->rows;
}