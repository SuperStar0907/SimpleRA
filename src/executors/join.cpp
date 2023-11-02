#include "global.h"
/**
 * @brief 
 * SYNTAX: R <- JOIN relation_name1, relation_name2 ON column_name1 bin_op column_name2
 */
bool syntacticParseJOIN()
{
    logger.log("syntacticParseJOIN");
    if (tokenizedQuery.size() != 9 || tokenizedQuery[5] != "ON")
    {
        cout << "SYNTAC ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = JOIN;
    parsedQuery.joinResultRelationName = tokenizedQuery[0];
    parsedQuery.joinFirstRelationName = tokenizedQuery[3];
    parsedQuery.joinSecondRelationName = tokenizedQuery[4];
    parsedQuery.joinFirstColumnName = tokenizedQuery[6];
    parsedQuery.joinSecondColumnName = tokenizedQuery[8];

    string binaryOperator = tokenizedQuery[7];
    if (binaryOperator == "<")
        parsedQuery.joinBinaryOperator = LESS_THAN;
    else if (binaryOperator == ">")
        parsedQuery.joinBinaryOperator = GREATER_THAN;
    else if (binaryOperator == ">=" || binaryOperator == "=>")
        parsedQuery.joinBinaryOperator = GEQ;
    else if (binaryOperator == "<=" || binaryOperator == "=<")
        parsedQuery.joinBinaryOperator = LEQ;
    else if (binaryOperator == "==")
        parsedQuery.joinBinaryOperator = EQUAL;
    else if (binaryOperator == "!=")
        parsedQuery.joinBinaryOperator = NOT_EQUAL;
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    return true;
}

bool semanticParseJOIN()
{
    logger.log("semanticParseJOIN");

    if (tableCatalogue.isTable(parsedQuery.joinResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.joinFirstRelationName) || !tableCatalogue.isTable(parsedQuery.joinSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.joinFirstColumnName, parsedQuery.joinFirstRelationName) || !tableCatalogue.isColumnFromTable(parsedQuery.joinSecondColumnName, parsedQuery.joinSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    return true;
}

bool comperator(int a, int b, BinaryOperator op)
{
    switch (op)
    {
    case LESS_THAN:
        return a < b;
    case GREATER_THAN:
        return a > b;
    case LEQ:
        return a <= b;
    case GEQ:
        return a >= b;
    case EQUAL:
        return a == b;
    case NOT_EQUAL:
        return a != b;
    default:
        return false;
    }
}

void joinTables(Table* tableOne, Table* tableTwo, int firstColIndex, int secondColIndex, const BinaryOperator& binaryOperator, Table* resultantTable, ofstream& outFile) {
    Cursor tableOneCursor(tableOne->tableName, 0);
    vector<int> tableOneRow = tableOneCursor.getNext();

    while (!tableOneRow.empty()) {
        Cursor tableTwoCursor(tableTwo->tableName, 0);
        vector<int> tableTwoRow = tableTwoCursor.getNext();

        while (!tableTwoRow.empty()) {
            if (comperator(tableOneRow[firstColIndex], tableTwoRow[secondColIndex], binaryOperator)) {
                vector<int> combinedRow = tableOneRow;
                combinedRow.insert(combinedRow.end(), tableTwoRow.begin(), tableTwoRow.end());
                resultantTable->writeRow(combinedRow, outFile);
            }
            tableTwoRow = tableTwoCursor.getNext();
        }

        tableOneRow = tableOneCursor.getNext();
    }
}

void executeJOIN()
{
    logger.log("executeJOIN");

    Table* tableOne = tableCatalogue.getTable(parsedQuery.joinFirstRelationName);
    Table* tableTwo = tableCatalogue.getTable(parsedQuery.joinSecondRelationName);

    // Step 2: prepare the result table
    int firstColIndex = tableOne->getColumnIndex(parsedQuery.joinFirstColumnName);
    int secondColIndex = tableTwo->getColumnIndex(parsedQuery.joinSecondColumnName);

    Table *resultTable = new Table(parsedQuery.joinResultRelationName);
    tableCatalogue.insertTable(resultTable);    
    ofstream outFile(resultTable->sourceFileName, ios::trunc);

    vector<string> allCols = tableOne->columns;
    allCols.insert(allCols.end(), tableTwo->columns.begin(), tableTwo->columns.end());
    resultTable->writeRow(allCols, outFile);

    // Step 3: join the tables
    joinTables(tableOne, tableTwo, firstColIndex, secondColIndex, parsedQuery.joinBinaryOperator, resultTable, outFile);

    resultTable->load();

    return;
}