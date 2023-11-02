#include "global.h"
/**
 * @brief 
 * SYNTAX: <new_table> <- GROUP BY <grouping_attribute> FROM <table_name> HAVING <aggregate(attribute)> <bin_op> <attribute_value> RETURN <aggregate_func(attribute)>
 */
bool syntacticParseGROUPBY()
{
    logger.log("syntacticParseGROUP");
    if (tokenizedQuery.size() == 13 && tokenizedQuery[1] == "<-" && tokenizedQuery[2] == "GROUP" && tokenizedQuery[3] == "BY" && tokenizedQuery[5] == "FROM" && tokenizedQuery[7] == "HAVING" && tokenizedQuery[11] == "RETURN")
    {
        parsedQuery.queryType = GROUPBY;
        parsedQuery.groupByResultRelationName = tokenizedQuery[0];
        parsedQuery.groupByColumnName = tokenizedQuery[4];
        parsedQuery.groupByTableName = tokenizedQuery[6];

        // Parse aggregate operation
        string aggregateOperation = tokenizedQuery[8].substr(0, tokenizedQuery[8].find('('));
        string aggregateColumn = tokenizedQuery[8].substr(tokenizedQuery[8].find('(') + 1, tokenizedQuery[8].find(')') - tokenizedQuery[8].find('(') - 1);

        // Parse having condition
        parsedQuery.groupByHavingBinaryOperator = tokenizedQuery[9];
        parsedQuery.groupByHavingIntLiteral = stoi(tokenizedQuery[10]);

        // Parse aggregate function
        string aggregateFunction = tokenizedQuery[12].substr(0, tokenizedQuery[12].find('('));
        string aggregateFunctionColumn = tokenizedQuery[12].substr(tokenizedQuery[12].find('(') + 1, tokenizedQuery[12].find(')') - tokenizedQuery[12].find('(') - 1);

        parsedQuery.groupByAggregateOperation = aggregateOperation;
        parsedQuery.groupByAggregateColumnName = aggregateColumn;
        parsedQuery.groupByAggregateFunction = aggregateFunction;
        parsedQuery.groupByAggregateFunctionColumnName = aggregateFunctionColumn;
        return true;    
    }
    cout << "SYNTAX ERROR" << endl;
    return false;
}

bool semanticParseGROUPBY() {
    logger.log("semanticParseGROUP");

    if (tableCatalogue.isTable(parsedQuery.groupByResultRelationName)) {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    Table* table = tableCatalogue.getTable(parsedQuery.groupByTableName);

    if (!table) {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    const string& groupByColumnName = parsedQuery.groupByColumnName;
    const string& groupByAggregateColumnName = parsedQuery.groupByAggregateColumnName;

    if (!table->isColumn(groupByColumnName) || !table->isColumn(groupByAggregateColumnName)) {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }

    return true;
}

int applyAggregateOperation(const string &aggregateOperation, const vector<int> &values) {
    static const unordered_map<string, function<int(const vector<int> &)>> operations = {
        {"SUM", [](const vector<int> &values) { return accumulate(values.begin(), values.end(), 0); }},
        {"COUNT", [](const vector<int> &values) { return values.size(); }},
        {"AVG", [](const vector<int> &values) {
            if (!values.empty()) {
                return accumulate(values.begin(), values.end(), 0) / values.size();
            }
            return (long unsigned int)0; // Handle division by zero
        }},
        {"MIN", [](const vector<int> &values) { return *min_element(values.begin(), values.end()); }},
        {"MAX", [](const vector<int> &values) { return *max_element(values.begin(), values.end()); }}
    };

    auto operationIt = operations.find(aggregateOperation);
    if (operationIt != operations.end()) {
        return operationIt->second(values);
    }

    return 0; // Default
}

bool evaluateHavingCondition(int aggregateResult, const string &operatorStr, int intValue) {
    static const unordered_map<string, function<bool(int, int)>> operators = {
        {">", greater<int>()},
        {"<", less<int>()},
        {"==", equal_to<int>()},
        {">=", greater_equal<int>()},
        {"<=", less_equal<int>()}
    };

    auto operatorIt = operators.find(operatorStr);
    if (operatorIt != operators.end()) {
        return operatorIt->second(aggregateResult, intValue);
    }

    return false; // Default
}

unordered_map<int, vector<vector<int>>> 
groupRowsByColumn(Table* table, const string& columnName) {
    int groupByColumnIndex = table->getColumnIndex(columnName);
    unordered_map<int, vector<vector<int>>> groupedRows;

    Cursor cursor(table->tableName, 0);
    for (int i = 0; i < table->rowCount; i++) {
        vector<int> row = cursor.getNext();
        if (row.empty()) {
            break;
        }
        groupedRows[row[groupByColumnIndex]].push_back(row);
    }

    return groupedRows;
}

vector<vector<int>> computeAggregateResults(const unordered_map<int, vector<vector<int>>>& groupedRows, Table* table, const ParsedQuery& query) {
    vector<vector<int>> resultRows;
    int groupByColumnIndex = table->getColumnIndex(query.groupByColumnName);
    int aggregateColumnIndex = table->getColumnIndex(query.groupByAggregateColumnName);

    for (const auto& [groupValue, groupRows] : groupedRows) {
        vector<int> aggregateValues;
        for (const vector<int>& row : groupRows) {
            aggregateValues.push_back(row[aggregateColumnIndex]);
        }

        int aggregateResult = applyAggregateOperation(query.groupByAggregateOperation, aggregateValues);

        if (evaluateHavingCondition(aggregateResult, query.groupByHavingBinaryOperator, query.groupByHavingIntLiteral)) {
            vector<int> newRow = {groupValue, aggregateResult};
            resultRows.push_back(newRow);
        }
    }

    return resultRows;
}

void writeAggregateResultsToFile(Table* table, string& resultFileName, const string& groupByColumnName, const string& aggregateFunction, const string& aggregateFunctionColumnName, const map<int, vector<vector<int>>>& groupMap) {
    ofstream outFile("../data/" + resultFileName + ".csv");
    outFile << groupByColumnName << "," << aggregateFunction << aggregateFunctionColumnName << "\n";

    int groupColumnIndex = table->getColumnIndex(groupByColumnName);
    int groupOperationIndex = table->getColumnIndex(aggregateFunctionColumnName);
    for (const auto& [groupValue, groupRows] : groupMap) {
        vector<int> aggregateValues;
        for (const vector<int>& row : groupRows) {
            aggregateValues.push_back(row[groupOperationIndex]);
        }
        int aggregateResult = applyAggregateOperation(aggregateFunction, aggregateValues);
        outFile << groupValue << ", " << aggregateResult << endl;
    }
}

map<int, vector<vector<int>>> groupRowsByGroupColumn(Table* newTable, const vector<vector<int>>& resultRows) {
    vector<int> firstColumn;
    for (const auto& row : resultRows) {
        firstColumn.push_back(row[0]);
    }

    int groupColumnIndex = newTable->getColumnIndex(parsedQuery.groupByColumnName);
    int groupOperationIndex = newTable->getColumnIndex(parsedQuery.groupByAggregateFunctionColumnName);
    Cursor cursor1(newTable->tableName, 0);
    vector<int> row;

    map<int, vector<vector<int>>> groupMap;

    for (int i = 0; i < newTable->rowCount; i++) {
        vector<int> row = cursor1.getNext();
        if (row.empty()) {
            break;
        }
        if (find(firstColumn.begin(), firstColumn.end(), row[groupColumnIndex]) != firstColumn.end()) {
            groupMap[row[groupColumnIndex]].push_back(row);
        }
    }
    return groupMap;
}

void reloadTable(const string& tableName) {
    bufferManager.clear();
    tableCatalogue.deleteTable(tableName);
    bufferManager.clear();
    
    Table* final = new Table(tableName);

    if (final->load()) {
        tableCatalogue.insertTable(final);
    }

    final->print();
}

void executeGROUPBY()
{
    logger.log("executeGROUP");
    
    Table table = *tableCatalogue.getTable(parsedQuery.groupByTableName);
    system(("cp ../data/" + parsedQuery.groupByTableName + ".csv ../data/" + parsedQuery.groupByResultRelationName + ".csv").c_str());

    Table *newTable = new Table(parsedQuery.groupByResultRelationName);
    if (newTable->load())
    {
        tableCatalogue.insertTable(newTable);
    }

    vector<pair<string, string>> sortColumns = {{parsedQuery.groupByColumnName, "ASC"}};
    newTable->sortTable(sortColumns);

    static const unordered_set<string> validOperations = {"MAX", "MIN", "SUM", "COUNT", "AVG"};
    if (!validOperations.count(parsedQuery.groupByAggregateOperation) > 0) {
        cout << "Error: Aggregate operation not supported." << endl;
        return;
    }

    unordered_map<int, vector<vector<int>>> groupedRows = groupRowsByColumn(newTable, parsedQuery.groupByColumnName);

    vector<vector<int>> resultRows = computeAggregateResults(groupedRows, newTable, parsedQuery);
    
    map<int, vector<vector<int>>> groupMap = groupRowsByGroupColumn(newTable, resultRows);
    
    writeAggregateResultsToFile(newTable,parsedQuery.groupByResultRelationName, parsedQuery.groupByColumnName, parsedQuery.groupByAggregateFunction, parsedQuery.groupByAggregateFunctionColumnName, groupMap);

    reloadTable(parsedQuery.groupByResultRelationName);

    return;
}