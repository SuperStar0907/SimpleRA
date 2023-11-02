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
