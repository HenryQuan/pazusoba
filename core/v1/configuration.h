/**
 * configuration.h
 * Once a board is read, some values won't change.
 * Configuration is just for this purpose.
 * 
 * by Yiheng Quan
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

class Configuration
{
    int row = 0;
    int column = 0;
    int minErase = 0;

    Configuration() {}

public:
    /// A shared instance of configuration
    static Configuration &shared()
    {
        static Configuration p;
        return p;
    }

    /// This should only be called once
    void config(int row, int column, int minErase)
    {
        this->row = row;
        this->column = column;
        this->minErase = minErase;
    }

    // MARK: - Getter
    int &getRow() { return row; }
    int &getColumn() { return column; }
    int &getMinErase() { return minErase; }
};

#endif
