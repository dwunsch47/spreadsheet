#pragma once

#include "cell.h"
#include "common.h"

#include <functional>
#include <unordered_map>

class CellHasher {
public:
    size_t operator()(Position pos) const {
        //return std::hash<int>()(pos.row) + 37 * std::hash<int>()(pos.col);
        return std::hash<std::string>()(pos.ToString());
    }
};

class CellComparator {
public:
    bool operator()(const Position& lhs, const Position& rhs) const {
        return lhs == rhs;
    }
};

class Sheet : public SheetInterface {
public:
    
    using Table = std::unordered_map<Position, std::unique_ptr<Cell>, CellHasher, CellComparator>;
    
    ~Sheet();

    void SetCell(Position pos, std::string text) override;

    const CellInterface* GetCell(Position pos) const override;
    CellInterface* GetCell(Position pos) override;
    const Cell* GetCellPtr(Position pos) const;
    Cell* GetCellPtr(Position pos);

    void ClearCell(Position pos) override;

    Size GetPrintableSize() const override;

    void PrintValues(std::ostream& output) const override;
    void PrintTexts(std::ostream& output) const override;

	// Можете дополнить ваш класс нужными полями и методами


private:
	Table table_;
};