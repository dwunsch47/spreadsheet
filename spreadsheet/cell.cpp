#include "cell.h"
#include "sheet.h"

#include <cassert>
#include <iostream>
#include <string>
#include <stack>
#include <optional>

class Cell::Impl {
public:
    virtual ~Impl() = default;
    virtual Value GetValue() const = 0;
    virtual std::string GetText() const = 0;
    virtual std::vector<Position> GetReferencedCells() const {
        return {};
    }
    virtual void InvalidateCache() {}
};

class Cell::EmptyImpl : public Impl {
public:
    Value GetValue() const override {
        return "";
    }
    
    std::string GetText() const override {
        return "";
    }
};

class Cell::TextImpl : public Impl {
public:
    TextImpl(std::string text) : text_(std::move(text)) {}
    
    Value GetValue() const override {
        if (text_[0] == ESCAPE_SIGN) {
            return text_.substr(1);
        }
        return text_;
    }
    
    std::string GetText() const override {
        return text_;
    }
        
private:
    std::string text_;
};

class Cell::FormulaImpl : public Impl {
public:
    FormulaImpl(std::string expression, const SheetInterface& sheet) : formula_ptr_(ParseFormula(expression.substr(1))), sheet_(sheet) {}
    
    Value GetValue() const override {
        if (!cache_) {
            cache_ = formula_ptr_->Evaluate(sheet_);
        }
        auto value = formula_ptr_->Evaluate(sheet_);
        if (std::holds_alternative<double>(value)) {
            return std::get<double>(value);
        } else {
            return std::get<FormulaError>(value);
        }
    }
    
    std::string GetText() const override {
        return FORMULA_SIGN + formula_ptr_->GetExpression();
    }
    
    void InvalidateCache() override {
        cache_.reset();
    }
    
    std::vector<Position> GetReferencedCells() const {
        return formula_ptr_->GetReferencedCells();
    }
    
private:
    std::unique_ptr<FormulaInterface> formula_ptr_;
    const SheetInterface& sheet_;
    mutable std::optional<FormulaInterface::Value> cache_;
};


// Реализуйте следующие методы
Cell::Cell(Sheet& sheet) : impl_(std::make_unique<EmptyImpl>()), sheet_(sheet) {}

Cell::~Cell() {}

void Cell::Set(std::string text) {
    std::unique_ptr<Impl> tmp_impl;
    
    if (text.empty()) {
        tmp_impl = std::make_unique<EmptyImpl>();
    } else if (text[0] == FORMULA_SIGN && text.size() > 1) {
        tmp_impl = std::make_unique<FormulaImpl>(text, sheet_);
    } else {
        tmp_impl = std::make_unique<TextImpl>(text);
    }
    
    if (CausesCircularDependency(*tmp_impl)) {
        throw CircularDependencyException("This formula causes circular dependency");
    }
    impl_ = std::move(tmp_impl);
    for (Cell* outgoing : r_nodes_) {
        outgoing->l_nodes_.erase(this);
    }

    r_nodes_.clear();
    
    for (const auto& pos : impl_->GetReferencedCells()) {
        Cell* outgoing = sheet_.GetCellPtr(pos);
        
        if (!outgoing) {
            sheet_.SetCell(pos, "");
            outgoing = sheet_.GetCellPtr(pos);
        }

        r_nodes_.insert(outgoing);
        outgoing->l_nodes_.insert(this);
    }

    RecursiveCacheInvalidation();
}

void Cell::RecursiveCacheInvalidation() {
    impl_->InvalidateCache();
    for (Cell* in : l_nodes_) {
        in->RecursiveCacheInvalidation();
    }
}

bool Cell::CausesCircularDependency(const Impl& impl) const {
    if (impl.GetReferencedCells().empty()) {
        return false;
    }
    
    std::unordered_set<const Cell*> referenced;
    for (const auto& pos : impl.GetReferencedCells()) {
        referenced.insert(sheet_.GetCellPtr(pos));
    }

    std::unordered_set<const Cell*> visited;
    std::stack<const Cell*> to_visit;
    to_visit.push(this);
    while (!to_visit.empty()) {
        const Cell* current = to_visit.top();
        to_visit.pop();
        visited.insert(current);

        if (referenced.find(current) != referenced.end()) {
            return true;
        }

        for (const Cell* incoming : current->l_nodes_) {
            if (visited.find(incoming) == visited.end()) {
                to_visit.push(incoming);
            }
        }
    }

    return false;
}

void Cell::Clear() {
    impl_ = std::make_unique<EmptyImpl>();
}

std::vector<Position> Cell::GetReferencedCells() const {
    return impl_->GetReferencedCells();
}

Cell::Value Cell::GetValue() const {
    return impl_->GetValue();
}
std::string Cell::GetText() const {
    return impl_->GetText();
}