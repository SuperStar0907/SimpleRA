#include "matrix.h"

class MatrixCatalogue
{
    unordered_map<string, Matrix *> matrices;

public:
    MatrixCatalogue() {}
    void deleteMatrix(string matrixName);
    Matrix *getMatrix(string matrixName);
    void insertMatrix(Matrix *matrix);
    bool isMatrix(string matrixName);
    void print();
    ~MatrixCatalogue();
};