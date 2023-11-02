#include"global.h"
/**
 * @brief File contains method to process SORT commands.
 * 
 * syntax:
 * R <- SORT relation_name BY column_name IN sorting_order
 * 
 * sorting_order = ASC | DESC 
 */
bool syntacticParseSORT(){
    logger.log("syntacticParseSORT");
    int numOfSortColumns = (tokenizedQuery.size()-4)/2;

    if(tokenizedQuery.size()< 6 || tokenizedQuery[2] != "BY" || tokenizedQuery[3+numOfSortColumns] != "IN"){
        cout<<"SYNTAX ERROR"<<endl;
        return false;
    }
    parsedQuery.queryType = SORT;
    parsedQuery.sortRelationName = tokenizedQuery[1];
    parsedQuery.sortColumns.clear();
    for(int i=tokenizedQuery.size()-1;i>=tokenizedQuery.size()-numOfSortColumns;i--){
        if (tokenizedQuery.at(i) != "ASC" && tokenizedQuery.at(i) != "DESC")
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }        
        parsedQuery.sortColumns.emplace_back(tokenizedQuery.at(i-numOfSortColumns-1),tokenizedQuery.at(i));
    }
    return true;
}

bool semanticParseSORT(){
    logger.log("semanticParseSORT");

    if(!tableCatalogue.isTable(parsedQuery.sortRelationName)){
        cout<<"SEMANTIC ERROR: Relation doesn't exist"<<endl;
        return false;
    }

    for(auto col:parsedQuery.sortColumns){
        if(!tableCatalogue.isColumnFromTable(col.first, parsedQuery.sortRelationName)){
            cout<<"SEMANTIC ERROR: Column doesn't exist in relation"<<endl;
            return false;
        }
    }

    return true;
}

void executeSORT(){
    logger.log("executeSORT");

    Table* table = tableCatalogue.getTable(parsedQuery.sortRelationName);
    table->sortTable(parsedQuery.sortColumns);

    return;
}