#include "global.h"
/**
 * @brief 
 * SYNTAX: <new_table> <- ORDER BY <attribute> ASC|DESC ON <table_name>
 */
bool syntacticParseORDERBY()
{
    logger.log("syntacticParseORDERBY");
    if (tokenizedQuery.size() == 8 && tokenizedQuery[1] == "<-" && tokenizedQuery[2] == "ORDER" && tokenizedQuery[3] == "BY" && tokenizedQuery[6] == "ON" && (tokenizedQuery[5] == "ASC" || tokenizedQuery[5] == "DESC"))
    {
        parsedQuery.queryType = ORDERBY;
        parsedQuery.orderByResultRelationName = tokenizedQuery[0];
        parsedQuery.orderByColumnName = tokenizedQuery[4];
        parsedQuery.orderByTableName = tokenizedQuery[7];
        parsedQuery.orderBySortingStrategy = tokenizedQuery[5];
        return true;    
    }
    cout << "SYNTAX ERROR" << endl;
    return false;
}

bool semanticParseORDERBY()
{
    logger.log("semanticParseOrderBy");
    if (!tableCatalogue.isTable(parsedQuery.orderByTableName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }
    if (!tableCatalogue.isColumnFromTable(parsedQuery.orderByColumnName, parsedQuery.orderByTableName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    return true;
}
void executeORDERBY()
{
    logger.log("executeOrderBy");
    Table table = *tableCatalogue.getTable(parsedQuery.orderByTableName);

    vector<pair<string, string>> sortCols;
    sortCols.resize(1);
    sortCols[0].first = parsedQuery.orderByColumnName;
    sortCols[0].second = parsedQuery.orderBySortingStrategy;

    Table *newTable = new Table(parsedQuery.orderByResultRelationName);
    if (newTable->load())
    {
        tableCatalogue.insertTable(newTable);
    }

    system(("cp ../data/" + parsedQuery.orderByTableName + ".csv ../data/" + parsedQuery.orderByResultRelationName + ".csv").c_str());

    newTable->sortTable(sortCols);

    return;
}