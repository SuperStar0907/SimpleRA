class Matrix
{

public:
    unsigned int blocksPerRow = 0;
    unsigned int columnCount = 0;
    bool isLastChunk = false;
    bool isFirstChunk = true;
    string matrixName = "";
    unsigned int maxColumnsPerBlock = 0;
    vector<unsigned int> PerBlockCount;
    long long int rowCount = 0;
    string sourceFileName = "";

    Matrix();
    Matrix(Matrix *matrix);
    Matrix(string matrixName);

    bool blockify();
    bool extractColumnNames(string firstLine);
    bool isPermanent();
    bool load();
    void makePermanent();
    void print();

    /**
 * @brief Static function that takes a vector of valued and prints them out in a
 * comma seperated format.
 *
 * @tparam T current usaages include int and string
 * @param row 
 */
    template <typename T>
    void writeRow(vector<T> row, ostream &fout)
    {
        logger.log("Matrix::printRow");
        for (int columnCounter = 0; columnCounter < row.size(); columnCounter++)
        {
            if (columnCounter != 0 && this->isFirstChunk)
                fout << ", ";
            fout << row[columnCounter];
        }
        if (this->isLastChunk)
            fout << endl;
    }

    /**
 * @brief Static function that takes a vector of valued and prints them out in a
 * comma seperated format.
 *
 * @tparam T current usaages include int and string
 * @param row 
 */
    template <typename T>
    void writeRow(vector<T> row)
    {
        logger.log("Matrix::printRow");
        ofstream fout(this->sourceFileName, ios::app);
        this->writeRow(row, fout);
        fout.close();
    }
};