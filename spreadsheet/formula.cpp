#include "formula.h"

#include "FormulaAST.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <sstream>

using namespace std::literals;

FormulaError::FormulaError(Category category) : category_(category) {}

FormulaError::Category FormulaError::GetCategory() const {
    return category_;
}

bool FormulaError::operator==(FormulaError rhs) const {
    return category_ == rhs.category_;
}

std::string_view FormulaError::ToString() const {
    switch (category_) {
        case Category::Ref:
            return "#REF!";
        case Category::Value:
            return "#VALUE!";
        case Category::Div0:
            return "#DIV/0!";
    }
    return "";
}

std::ostream& operator<<(std::ostream& output, FormulaError fe) {
    return output << fe.ToString();
}

namespace {
class Formula : public FormulaInterface {
public:
    
    explicit Formula(std::string expression) try : ast_(ParseFormulaAST(expression)) {
        }
        catch (const std::exception& e) {
            std::throw_with_nested(FormulaException(e.what()));
        }
    
    Value Evaluate(const SheetInterface& sheet) const override {
        const std::function<double(Position)> args = [&sheet](const Position pos)
            -> double {
            if (!pos.IsValid()) {
                throw FormulaError(FormulaError::Category::Ref);
            }
            const auto* cell = sheet.GetCell(pos);
            if (!cell) {
                return 0;
            }
            const auto value = cell->GetValue();
            if (std::holds_alternative<double>(value)) {
                return std::get<double>(value);
            } else if (std::holds_alternative<std::string>(value)) {
                const std::string str = std::get<std::string>(value);
                double result = 0;
                std::istringstream in(str);
                if (!(in >> result) || !in.eof()) {
                    throw FormulaError(FormulaError::Category::Value);
                }
                return result;
            }
            throw FormulaError(std::get<FormulaError>(value)); 
        };
        
        
        try {
            return ast_.Execute(args);
        } catch (FormulaError& evaluate_error) {
            return evaluate_error;
        }
    }
    
    std::vector<Position> GetReferencedCells() const override {
        std::vector<Position> result;
        for (Position pos : ast_.GetCells()) {
            if (pos.IsValid()) {
                result.push_back(pos);
            }
        }
        auto it = std::unique(result.begin(), result.end());
        result.resize(it - result.begin());
        return result;
    }
    
    std::string GetExpression() const override {
        std::ostringstream result;
        ast_.PrintFormula(result);
        return result.str();
    }

private:
    FormulaAST ast_;
};
}  // namespace

std::unique_ptr<FormulaInterface> ParseFormula(std::string expression) {
    try {
        return std::make_unique<Formula>(std::move(expression));
    } catch (...) {
        throw FormulaException("");
    }
}