#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

using namespace std::literals;

Sheet::~Sheet() {}

void Sheet::SetCell(Position pos, std::string text) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Cell position is invalid"s);
    }
    if (table_.find(pos) == table_.end()) {
        table_.emplace(pos, std::make_unique<Cell>(*this));
    }
    table_.at(pos)->Set(text);
}

const CellInterface* Sheet::GetCell(Position pos) const {
    return GetCellPtr(pos);
}
CellInterface* Sheet::GetCell(Position pos) {
    return GetCellPtr(pos);
}

const Cell* Sheet::GetCellPtr(Position pos) const {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Cell position is invalid"s);
    }
    if (table_.find(pos) == table_.end()) {
        return nullptr;
    } else {
        return table_.at(pos).get();
    }
}

Cell* Sheet::GetCellPtr(Position pos) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Cell position is invalid"s);
    }
    if (table_.find(pos) == table_.end()) {
        return nullptr;
    } else {
        return table_.at(pos).get();
    }
}

void Sheet::ClearCell(Position pos) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Cell position is invalid"s);
    }
    auto it = table_.find(pos);
    if (it != table_.end() && it->second) {
        table_.erase(it->first);
    }
}

Size Sheet::GetPrintableSize() const {
    Size result;
    for (const auto& [pos, ptr] : table_) {
        if (ptr) {
            result.rows = std::max(result.rows, pos.row + 1);
            result.cols = std::max(result.cols, pos.col + 1);
        }
    }
    return result;
}

void Sheet::PrintValues(std::ostream& output) const {
    Size printable_area = GetPrintableSize();
    for (int row = 0; row < printable_area.rows; ++row) {     
        for (int col = 0; col < printable_area.cols; ++col) {
            if (col != 0) {
                output << '\t';
            }
            auto it = table_.find({row, col});
            if (it != table_.end() && it->second && !it->second->GetText().empty()) {
                std::visit([&](const auto& obj){output << obj;},it->second->GetValue());
            }
        }
        output << '\n';
    }
}
            
void Sheet::PrintTexts(std::ostream& output) const {
   Size printable_area = GetPrintableSize();
    for (int row = 0; row < printable_area.rows; ++row) {        
        for (int col = 0; col < printable_area.cols; ++col) {
            if (col != 0) {
                output << '\t';
            }
            auto it = table_.find({row, col});
            if (it != table_.end() && it->second && !it->second->GetText().empty()) {      
                output << it->second->GetText();
            }
        }
        output << '\n';
    }
}


std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}