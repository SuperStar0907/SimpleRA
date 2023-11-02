# CS4.401 - Data Systems

# Phase 1
## Extending SimpleRA to include Matrices

### **We were tasked with adding functionality for loading, exporting, and transposing matrices in a program called SimpleRA. Here's how we did it:**

#### **LOAD MATRIX [MATRIX_NAME]**

- To efficiently handle potentially very large rows of numbers in a matrix, we divided each row into smaller blocks, rather than dividing the entire matrix. These blocks contain sections of rows. For organizing and managing these matrix blocks, we created a special folder named "Matrix." Inside this folder, we used a specific naming format like "matrixname_Block_rownumber_blocknumber" to avoid any naming conflicts with other data. We also implemented specific code for matrix operations, much like what was done for tables in the program.

#### **TRANSPOSE MATRIX [MATRIX_NAME]**

- To perform the transpose operation on a matrix (essentially flipping it), we did it directly within the original matrix, without creating a new one. We used a formula to figure out which block each element should be in after the transpose. This involved traversing the matrix in a specific way, similar to looking at the upper part of a triangle, and swapping corresponding elements like "swap(a[i][j],a[j][i])."

#### **EXPORT MATRIX [MATRIX_NAME]**

- For exporting matrices, we chose to follow a similar strategy as we did for other data, like relations. We read the matrix blocks one by one and wrote them to files in CSV format, maintaining compatibility with external tools and not changing our export approach for relations.

#### **PRINT MATRIX [MATRIX_NAME]**

- For printing matrices, we chose to follow a similar strategy as we did for other data, like relations. We read the matrix blocks one by one and printed them to the console, maintaining compatibility with external tools and not changing our print approach for relations.
- If rows are greater than 20, then we print the first 20 rows and columns.

#### **COMPUTE [MATRIX_NAME]**

- For Compute matrices, we retrieve a matrix from a catalog and create a copy of it as resultMatrix. Then, we perform a transpose operation on the original matrix by swapping elements based on specific rules. The resulting transposed matrix is stored in resultMatrix.
- Then we calculate the difference between the original and transposed matrices, effectively finding the result of A - A^T. We update resultMatrix with this difference.
- Finally, we add the resultMatrix to the matrix catalog as a new matrix.

#### **CHECKSYMMETRY [MATRIX_NAME]**

- We retrieve the matrix from a catalog and iterate through its rows and columns. For each pair of elements (i, j) where i <= j, we calculate block numbers and indices within blocks. We compare the elements (i, j) and (j, i) within the matrix. If any pair differs, we print "NO" to indicate non-symmetry and return. Otherwise, if all pairs are equal, we print "YES" to signify symmetry. This function efficiently checks matrix symmetry and provides a result accordingly.

#### **RENAME MATRIX [CURRENT_MATRIX_NAME] [NEW_MATRIX_NAME]**

- We first retrive the matrix, we make a new matrix using a copy constructor which uses pointer instead of object, change the name and sourceFileName and then we add this matrix to catalog as well as we give pages and files according to the new name from the catalog and iterate over all the files of the old ones to delete them.

# Phase 2
## EXTERNAL SORTING and Its Application to GroupBy, Join, and OrderBy

### **We were tasked with adding functionality for external sorting and its application to GroupBy, Join, and OrderBy in a program called SimpleRA. Here's how we did it:**

#### Technical Implementation: External Sorting

The provided code snippet demonstrates an implementation of external sorting, which is a technique used to sort large datasets that do not fit entirely in memory. This technique is essential for efficiently managing and processing data in scenarios where the available memory is limited. The following components and processes are involved in this implementation:

1. **Custom Comparator**:
   - The `CustomComparator` structure is designed to facilitate sorting based on user-defined columns and sorting strategies. It defines a custom comparison function that considers multiple columns and their sorting orders (ASC or DESC).
   
2. **Sorting Phase**:
   - The `sorting_phase` function handles the initial sorting of data within individual pages. It retrieves data from the original table, sorts it based on specified columns and sorting orders, and writes the sorted data to temporary runs (subfiles).
   
3. **Merging Phase**:
   - The `merging_phase` function manages the merging of runs (subfiles) created in the sorting phase. It performs a multi-step merge process, where each step involves merging a certain number of subfiles into new runs.
   
4. **Initialization and Cursor Management**:
   - The code handles cursor management for reading data from runs and initializing cursors to access different parts of the runs. Cursors are used to access and read data from runs during the merging phase.
   
5. **Heap-Based Merging**:
   - Data from different runs is merged using a priority queue (heap) to maintain the order of records. The heap ensures that records are selected and merged in the correct order, based on the custom comparison function defined in the `CustomComparator`.
   
6. **File I/O and Cleanup**:
   - The code efficiently manages file I/O for reading and writing data to and from runs. It also includes logic for creating and cleaning up temporary tables (runs) used during the sorting and merging phases.
   
7. **Updating Table Metadata**:
   - After sorting and merging are complete, the metadata for the original table is updated to reflect the changes, such as the new block count.

8. **Finalizing Sorted Data**:
   - The sorted data is written back to the original table, effectively replacing the unsorted data with the sorted version.

#### Technical Implementation: GROUP BY

This section outlines the technical details of our system's implementation for performing aggregate operations and GROUP BY functionality. The key components and processes involved are as follows:

1. **Aggregate Operation**:
   - The `applyAggregateOperation` function handles various aggregate operations, such as SUM, COUNT, AVG, MIN, and MAX. It utilizes an unordered map to map operation names to lambda functions that calculate the aggregate values.
   - When a specific aggregate operation is requested, the corresponding lambda function is executed on a given vector of values, returning the result.

2. **Having Condition Evaluation**:
   - The `evaluateHavingCondition` function evaluates a having condition using comparison operators like greater than, less than, equal to, greater than or equal to, and less than or equal to. Similar to the aggregate operations, it employs an unordered map with lambda functions.
   - The function returns a boolean result based on the operator applied to an aggregate result and an integer value.

3. **Grouping Rows by Column**:
   - The `groupRowsByColumn` function groups rows of a table based on a specified column. It uses an unordered map to associate the unique values of the grouping column with the corresponding rows.
   - This function is essential for preparing data for the subsequent GROUP BY operation.

4. **Compute Aggregate Results**:
   - The `computeAggregateResults` function calculates the aggregate results for each group obtained through the GROUP BY operation. It processes the grouped rows and applies the specified aggregate operation to the values in the aggregate column.
   - If a HAVING condition is specified, the results are filtered accordingly.

5. **Writing Aggregate Results to File**:
   - The `writeAggregateResultsToFile` function writes the computed aggregate results to a file. It specifies the group-by column, aggregate function, and the corresponding values in the result file.
   
6. **Grouping Rows by Group Column**:
   - The `groupRowsByGroupColumn` function performs an additional grouping step to organize the results by the group column. This step is necessary to prepare the data for writing to the result file.

7. **Reloading Table**:
   - The `reloadTable` function is responsible for clearing the buffer manager, deleting the original table, and reloading the result table. It ensures that the changes are reflected in the system.

8. **Executing GROUP BY**:
   - The `executeGROUPBY` function orchestrates the entire process. It retrieves the relevant tables, prepares a new table for the results, performs the GROUP BY operation, calculates aggregate results, and writes the final results to a file.
   - The result table is then reloaded to update the system with the new data.

#### Technical Implementation: JOIN Operation

In our system, we have successfully implemented a powerful JOIN operation, allowing the combination of data from two tables. The key components of this implementation are as follows:

1. **Table Preparation**:
   - The `executeJOIN()` function retrieves the two tables, `tableOne` and `tableTwo`, from the `tableCatalogue`. These tables are chosen based on user input and queries.
   - The indices of the columns that will participate in the JOIN are determined.

2. **Result Table Setup**:
   - To store the joined data, a new table, `resultTable`, is created. It's essential that this table has the appropriate schema to accommodate data from both input tables.
   - The schema of `resultTable` is established by combining the column names from `tableOne` and `tableTwo`. This schema is then written to the result file.

3. **Joining Process**:
   - The core of the JOIN operation is managed by the `joinTables()` function. It iterates through the rows of both `tableOne` and `tableTwo`, comparing values based on the specified binary operator, which is provided through `parsedQuery.joinBinaryOperator`.
   - When the condition is satisfied, the corresponding rows are merged into a new row, which is then written to the result table. This process continues until all possible combinations have been processed.

4. **Comparison Function**:
   - The `comperator()` function plays a pivotal role in the process. It accepts two integers and a binary operator as parameters and returns a boolean result based on the operator type. This function allows the handling of various JOIN conditions such as less than, greater than, equal, and more.

5. **Result Table Loading**:
   - After the JOIN operation is complete, the result table is loaded and ready for further operations.


### Implementation of ORDER BY operation

The `executeORDERBY()` function is an integral part of our system's data processing capabilities. It plays a pivotal role in arranging data within a table according to specific sorting criteria defined by users. Importantly, this function leverages the power of **external sorting** to efficiently handle large datasets that exceed the available memory.

Here are the key components of the implementation:

1. **Table Retrieval**:
   - The function starts by retrieving the target table from the `tableCatalogue` based on the user's request. This table contains the data that needs to be sorted.

2. **Sorting Criteria**:
   - The sorting criteria are determined by the `parsedQuery`, including the column to sort by (`parsedQuery.orderByColumnName`) and the sorting strategy (e.g., ascending or descending).

3. **Temporary Table Creation**:
   - A new table, denoted by `newTable`, is created to store the sorted results. This table is defined based on the `parsedQuery.orderByResultRelationName` and is loaded into the `tableCatalogue`.

4. **Data Duplication**:
   - To preserve the original data, the function makes a copy of the source table's data file using the `system` command, ensuring that the sorting operation is applied to the new table.

5. **External Sorting**:
   - The core of the ordering process involves invoking the `sortTable` method of the `newTable`. This is where the system employs **external sorting techniques** to efficiently sort the data according to the specified criteria. The implementation utilizes sorting phases and merging phases to manage the data sorting operation, handling situations where the dataset doesn't fit entirely in memory.

6. **Completion and Metadata Update**:
   - Once the external sorting is complete, the sorted data is saved to the `newTable`. The function finalizes the sorting process and ensures that the metadata of the `newTable` is updated accordingly.

