#include "global.h"

/**
 * @brief Construct a new Table:: Table object
 *
 */
Table::Table()
{
    logger.log("Table::Table");
}

/**
 * @brief Construct a new Table:: Table object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param tableName 
 */
Table::Table(string tableName)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/" + tableName + ".csv";
    this->tableName = tableName;
}

/**
 * @brief Construct a new Table:: Table object used when an assignment command
 * is encountered. To create the table object both the table name and the
 * columns the table holds should be specified.
 *
 * @param tableName 
 * @param columns 
 */
Table::Table(string tableName, vector<string> columns)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/temp/" + tableName + ".csv";
    this->tableName = tableName;
    this->columns = columns;
    this->columnCount = columns.size();
    this->maxRowsPerBlock = (unsigned int)((BLOCK_SIZE * 1000) / (sizeof(int) * columnCount));
    this->writeRow<string>(columns);
}

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates table
 * statistics.
 *
 * @return true if the table has been successfully loaded 
 * @return false if an error occurred 
 */
bool Table::load()
{
    logger.log("Table::load");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if (getline(fin, line))
    {
        fin.close();
        if (this->extractColumnNames(line))
            if (this->blockify())
                return true;
    }
    fin.close();
    return false;
}

/**
 * @brief Function extracts column names from the header line of the .csv data
 * file. 
 *
 * @param line 
 * @return true if column names successfully extracted (i.e. no column name
 * repeats)
 * @return false otherwise
 */
bool Table::extractColumnNames(string firstLine)
{
    logger.log("Table::extractColumnNames");
    unordered_set<string> columnNames;
    string word;
    stringstream s(firstLine);
    while (getline(s, word, ','))
    {
        word.erase(remove_if(word.begin(), word.end(), ::isspace), word.end());
        if (columnNames.count(word))
            return false;
        columnNames.insert(word);
        this->columns.emplace_back(word);
    }
    this->columnCount = this->columns.size();
    this->maxRowsPerBlock = (unsigned int)((BLOCK_SIZE * 1000) / (sizeof(int) * this->columnCount));
    return true;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size. 
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Table::blockify()
{
    logger.log("Table::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;
    vector<int> row(this->columnCount, 0);
    vector<vector<int>> rowsInPage(this->maxRowsPerBlock, row);
    int pageCounter = 0;
    unordered_set<int> dummy;
    dummy.clear();
    this->distinctValuesInColumns.assign(this->columnCount, dummy);
    this->distinctValuesPerColumnCount.assign(this->columnCount, 0);
    getline(fin, line);
    while (getline(fin, line))
    {
        stringstream s(line);
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (!getline(s, word, ','))
                return false;
            row[columnCounter] = stoi(word);
            rowsInPage[pageCounter][columnCounter] = row[columnCounter];
        }
        pageCounter++;
        this->updateStatistics(row);
        if (pageCounter == this->maxRowsPerBlock)
        {
            bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
            this->blockCount++;
            this->rowsPerBlockCount.emplace_back(pageCounter);
            pageCounter = 0;
        }
    }
    if (pageCounter)
    {
        bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
        this->blockCount++;
        this->rowsPerBlockCount.emplace_back(pageCounter);
        pageCounter = 0;
    }

    if (this->rowCount == 0)
        return false;
    this->distinctValuesInColumns.clear();
    return true;
}

/**
 * @brief Given a row of values, this function will update the statistics it
 * stores i.e. it updates the number of rows that are present in the column and
 * the number of distinct values present in each column. These statistics are to
 * be used during optimisation.
 *
 * @param row 
 */
void Table::updateStatistics(vector<int> row)
{
    this->rowCount++;
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (!this->distinctValuesInColumns[columnCounter].count(row[columnCounter]))
        {
            this->distinctValuesInColumns[columnCounter].insert(row[columnCounter]);
            this->distinctValuesPerColumnCount[columnCounter]++;
        }
    }
}

/**
 * @brief Checks if the given column is present in this table.
 *
 * @param columnName 
 * @return true 
 * @return false 
 */
bool Table::isColumn(string columnName)
{
    logger.log("Table::isColumn");
    for (auto col : this->columns)
    {
        if (col == columnName)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Renames the column indicated by fromColumnName to toColumnName. It is
 * assumed that checks such as the existence of fromColumnName and the non prior
 * existence of toColumnName are done.
 *
 * @param fromColumnName 
 * @param toColumnName 
 */
void Table::renameColumn(string fromColumnName, string toColumnName)
{
    logger.log("Table::renameColumn");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (columns[columnCounter] == fromColumnName)
        {
            columns[columnCounter] = toColumnName;
            break;
        }
    }
    return;
}

/**
 * @brief Function prints the first few rows of the table. If the table contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed.
 *
 */
void Table::print()
{
    logger.log("Table::print");
    unsigned int count = min((long long)PRINT_COUNT, this->rowCount);

    //print headings
    this->writeRow(this->columns, cout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < count; rowCounter++)
    {
        row = cursor.getNext();
        this->writeRow(row, cout);
    }
    printRowCount(this->rowCount);
}



/**
 * @brief This function returns one row of the table using the cursor object. It
 * returns an empty row is all rows have been read.
 *
 * @param cursor 
 * @return vector<int> 
 */
void Table::getNextPage(Cursor *cursor)
{
    logger.log("Table::getNext");

        if (cursor->pageIndex < this->blockCount - 1)
        {
            cursor->nextPage(cursor->pageIndex+1);
        }
}



/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
void Table::makePermanent()
{
    logger.log("Table::makePermanent");
    if(!this->isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->tableName + ".csv";
    ofstream fout(newSourceFile, ios::out);

    //print headings
    this->writeRow(this->columns, fout);

    Cursor cursor(this->tableName, 0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        row = cursor.getNext();
        this->writeRow(row, fout);
    }
    fout.close();
}

/**
 * @brief Function to check if table is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Table::isPermanent()
{
    logger.log("Table::isPermanent");
    if (this->sourceFileName == "../data/" + this->tableName + ".csv")
    return true;
    return false;
}

/**
 * @brief The unload function removes the table from the database by deleting
 * all temporary files created as part of this table
 *
 */
void Table::unload(){
    logger.log("Table::~unload");
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
        bufferManager.deleteFile(this->tableName, pageCounter);
    if (!isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
}

/**
 * @brief Function that returns a cursor that reads rows from this table
 * 
 * @return Cursor 
 */
Cursor Table::getCursor()
{
    logger.log("Table::getCursor");
    Cursor cursor(this->tableName, 0);
    return cursor;
}
/**
 * @brief Function that returns the index of column indicated by columnName
 * 
 * @param columnName 
 * @return int 
 */
int Table::getColumnIndex(string columnName)
{
    logger.log("Table::getColumnIndex");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (this->columns[columnCounter] == columnName)
            return columnCounter;
    }
}

struct CustomComparator {
    vector<int> Columns;
    vector<string> sorting_strategies;

    CustomComparator(const vector<int>& allColumns, const vector<string>& sortingStrategies)
        : Columns(allColumns), sorting_strategies(sortingStrategies) {}

    bool operator()(const pair<vector<int>, int>& a, const pair<vector<int>, int>& b) const {
        for (int i = 0; i < Columns.size(); ++i) {
            int col = Columns[i];
            if (a.first[col] != b.first[col]) {
                if (sorting_strategies[i] == "ASC") {
                    return compareAscending(a.first[col], b.first[col]);
                } else {
                    return compareDescending(a.first[col], b.first[col]);
                }
            }
        }
        return false;
    }

    bool compareAscending(int a, int b) const {
        return a > b;
    }

    bool compareDescending(int a, int b) const {
        return a < b;
    }
};

void processRemainingData(vector<int> &remainingCounts, pair<vector<int>, int> &currentRow,vector<Cursor *> &cursors, priority_queue<pair<vector<int>, int>, vector<pair<vector<int>, int>>, CustomComparator> &rowQueue) {
    if (remainingCounts[currentRow.second] > 0) {
        vector<int> newRow = cursors[currentRow.second]->getNext();

        if (!newRow.empty()) {
            rowQueue.push(make_pair(newRow, currentRow.second));
        } else {
            remainingCounts[currentRow.second] = 0;
        }
    }
}

int merge(vector<Cursor *> &cursors, vector<vector<int>> &mergedData, vector<int> &colIndices, vector<string> &sortStrategies, int maxRecords, vector<int> &remainingCounts, priority_queue<pair<vector<int>, int>, vector<pair<vector<int>, int>>, CustomComparator> &rowQueue) {
    mergedData.clear();
    int hasRemainingRecords = 0;

    while (maxRecords > 0 && !rowQueue.empty()) {
        pair<vector<int>, int> currentRow = rowQueue.top();
        rowQueue.pop();
        mergedData.push_back(currentRow.first);

        processRemainingData(remainingCounts, currentRow, cursors, rowQueue);

        maxRecords--;

        if (!rowQueue.empty()) {
            hasRemainingRecords = 1;
        }
    }

    return hasRemainingRecords;
}

Table* createAndInsertPreviousRunTable(Table* originalTable) {
    Table* prevRun = new Table(originalTable->tableName + "_previous_run");
    tableCatalogue.insertTable(prevRun);
    prevRun->sourceFileName = "../data/" + prevRun->tableName + ".csv";

    return prevRun;
}

Table* createAndInsertNextRunTable(Table* originalTable, int iteration, vector<Table*>& runs) {
    Table* nextRun = new Table(originalTable->tableName + "_run_" + to_string(iteration));
    tableCatalogue.insertTable(nextRun);
    runs.push_back(nextRun);
    nextRun->sourceFileName = "../data/" + nextRun->tableName + ".csv";
    nextRun->blockCount = 0;
    return nextRun;
}

ofstream createAndOpenOutputFile(const string& fileName) {
    ofstream outFile(fileName, ios::trunc);
    return outFile;
}

int calculateNumPages(int blockCount) {
    const int blockSize = 10;
    return static_cast<int>(ceil(static_cast<double>(blockCount) / static_cast<double>(blockSize)));
}

vector<vector<int>> fetchRowsPerPage(Table* originalTable, int pageCounter) {
    const int blockSize = 10;
    const int startBlock = (pageCounter - 1) * blockSize;
    const int endBlock = min(blockSize, (int)originalTable->blockCount);

    Cursor cursor(originalTable->tableName, startBlock);
    vector<vector<int>> rowsPerPage;

    for (int currentBlock = startBlock; currentBlock < endBlock; ++currentBlock) {
        for (int i = 0; i < originalTable->rowsPerBlockCount[currentBlock]; ++i) {
            rowsPerPage.push_back(cursor.getNext());
        }
    }

    return rowsPerPage;
}

void sortRowsPerPage(vector<vector<int>>& rowsPerPage, const vector<int>& columnIndices, const vector<string>& sortingOrders) {
    auto customSort = [&columnIndices, &sortingOrders](const vector<int>& a, const vector<int>& b) {
        for (size_t j = 0; j < columnIndices.size(); ++j) {
            int col = columnIndices[j];
            string order = sortingOrders[j];
            if (a[col] == b[col]) continue;
            return (order == "ASC") ? (a[col] < b[col]) : (a[col] > b[col]);
        }
        return false;
    };

    sort(rowsPerPage.begin(), rowsPerPage.end(), customSort);
}

void initializeCursors(vector<Cursor*>& cursors, const int i, const int j,const int mergeFactor, const int updatedMergeFactor, const vector<Table*>& runs,int blockSize) {
    const int offset = 10 * static_cast<int>(pow(blockSize - 1, i - 1));
    
    for (int itr = 0; itr < updatedMergeFactor; itr++) {
        cursors.push_back(new Cursor(runs[i - 1]->tableName, offset * ((j - 1) * mergeFactor + itr)));
    }
}

void populateRowHeap(vector<Cursor*>& cursors, vector<int>& remainingRecords, priority_queue<pair<vector<int>, int>, vector<pair<vector<int>, int>>, CustomComparator>& rowHeap) {
    for (int itr = 0; itr < cursors.size(); itr++) {
        vector<int> record = cursors[itr]->getNext();
        remainingRecords[itr]--;

        if (!record.empty()) {
            rowHeap.push(make_pair(record, itr));
        }
    }
}

void cleanupTempTables(vector<Table*>& runs) {
    for (int i = 0; i < runs.size(); i++) {
        runs[i]->unload();
        string path = "../data/" + runs[i]->tableName + ".csv";
        remove(path.c_str());
        tableCatalogue.deleteTable(runs[i]->tableName);
    }
    runs.clear();
}

void updateTableMetadata(string tableName) {
    tableCatalogue.deleteTable(tableName);

    Table* newTable = new Table(tableName);
    tableCatalogue.insertTable(newTable);
    newTable->sourceFileName = "../data/" + newTable->tableName + ".csv";
    newTable->blockCount = 0;
    newTable->load();
}

void sorting_phase(vector<Table *> &runList, Table *originalTable, vector<int> &columnIndices, vector<string> &sortingOrders) {
    logger.log("Table::performSortingPhase");
    
    Table* prevRun = createAndInsertPreviousRunTable(originalTable);
    ofstream outFile = createAndOpenOutputFile(prevRun->sourceFileName);
    originalTable->writeRow(originalTable->columns, outFile);

    const int numPages = calculateNumPages(originalTable->blockCount);

    for (int pageCounter = 1; pageCounter <= numPages; ++pageCounter) {
        vector<vector<int>> rowsPerPage = fetchRowsPerPage(originalTable, pageCounter);

        sortRowsPerPage(rowsPerPage, columnIndices, sortingOrders);

        for (const auto& row : rowsPerPage) {
            originalTable->writeRow(row, outFile);
        }
    }

    outFile.close();
    prevRun->load();
    runList.push_back(prevRun);
}

void merging_phase(vector<Table *> &runs, vector<int> &columnIndices, vector<string> &sortingStrategies, Table *originalTable) {
    logger.log("Table::executeMergingPhase");

    int blockSize = 10;
    int mergeFactor = blockSize - 1;
    int numPages = ceil((double)originalTable->blockCount / (double)blockSize);
    int iterations = ceil(double(log10(numPages)) / double(log10(mergeFactor)));
    int subfilesCount = numPages;

    for (int i = 1; i <= iterations; ++i) {
        Table *nextRun = createAndInsertNextRunTable(originalTable, i, runs);

        ofstream outputFile = createAndOpenOutputFile(runs[i]->sourceFileName);
        
        runs[i]->writeRow(originalTable->columns, outputFile);

        int j = 1;
        int segments = ceil(double(subfilesCount) / double(mergeFactor));

        while (j <= segments) {
            int updatedMergeFactor = (j == segments)
             ? (subfilesCount % mergeFactor) : mergeFactor;
            
            vector<Cursor*> cursors;
            initializeCursors(cursors, i, j,mergeFactor, updatedMergeFactor, runs,blockSize);

            int flag = 1;
            vector<int> remainingRecords(updatedMergeFactor, originalTable->maxRowsPerBlock * 10 * pow(mergeFactor, i - 1));
            priority_queue<pair<vector<int>, int>, vector<pair<vector<int>, int>>, CustomComparator> rowHeap(CustomComparator(columnIndices, sortingStrategies));

            populateRowHeap(cursors, remainingRecords, rowHeap);

            while (flag) {
                vector<vector<int>> mergedBlock;
                flag = merge(cursors, mergedBlock, columnIndices, sortingStrategies, originalTable->maxRowsPerBlock, remainingRecords, rowHeap);

                for (int itr = 0; itr < mergedBlock.size(); itr++) {
                    runs[itr]->writeRow(mergedBlock[itr], outputFile);
                }
            }

            j++;
            updatedMergeFactor = mergeFactor;
        }

        runs[i]->load();
        outputFile.close();
        subfilesCount = segments;
    }
}

void Table::writeSortedDataBack(const vector<Table*>& runs) {
    this->blockCount = 0;
    this->rowsPerBlockCount.clear();
    bufferManager.clear();

    string sourceFileName = this->sourceFileName;
    ofstream fout;
    fout.open(sourceFileName, ios::trunc);
    this->writeRow(this->columns, fout);

    Cursor cursor(runs.back()->tableName, 0);
    vector<int> row;

    for (int rowCounter = 0; rowCounter < runs.back()->rowCount; rowCounter++) {
        row = cursor.getNext();
        this->writeRow(row, fout);
    }

    fout.close();
}

void Table::sortTable(vector<pair<string, string>> columns)
{
    logger.log("Table::sortTable");

    // Step 1: sort and merge
    vector<Table *> runs;
    vector<int> columnIndices;
    vector<string> SortingStrategies;

    for (auto col : columns)
    {
        columnIndices.push_back(this->getColumnIndex(col.first));
        SortingStrategies.push_back(col.second);
    }

    sorting_phase(runs, this, columnIndices, SortingStrategies);
    merging_phase(runs, columnIndices, SortingStrategies, this);

    // Step 2: write sorted data from runs to original table
    this->writeSortedDataBack(runs);

    // Step 3: CLEANUP temp files
    cleanupTempTables(runs);

    // Step 4: update table statistics
    updateTableMetadata(this->tableName);
}