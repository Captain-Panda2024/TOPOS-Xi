#ifndef TOPOS_COMPILER_ERROR_ERROR_HPP
#define TOPOS_COMPILER_ERROR_ERROR_HPP

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

namespace topos {
namespace compiler {

// エラーの重要度
enum class ErrorSeverity {
    Info,
    Warning,
    Error,
    Fatal
};

// ソースコードの位置情報
struct SourceLocation {
    std::string filename;
    size_t line;
    size_t column;

    SourceLocation(std::string fname, size_t l, size_t c)
        : filename(std::move(fname)), line(l), column(c) {}

    std::string toString() const {
        return filename + ":" + std::to_string(line) + ":" + std::to_string(column);
    }
};

// コンパイラエラーの基底クラス
class CompilerError : public std::runtime_error {
public:
    CompilerError(std::string message,
                 SourceLocation location,
                 ErrorSeverity severity = ErrorSeverity::Error)
        : std::runtime_error(message),
          message_(std::move(message)),
          location_(std::move(location)),
          severity_(severity) {}

    const std::string& getMessage() const { return message_; }
    const SourceLocation& getLocation() const { return location_; }
    ErrorSeverity getSeverity() const { return severity_; }

    virtual std::string formatError() const {
        std::string severityStr;
        switch (severity_) {
            case ErrorSeverity::Info: severityStr = "info"; break;
            case ErrorSeverity::Warning: severityStr = "warning"; break;
            case ErrorSeverity::Error: severityStr = "error"; break;
            case ErrorSeverity::Fatal: severityStr = "fatal error"; break;
        }

        return location_.toString() + ": " + severityStr + ": " + message_;
    }

private:
    std::string message_;
    SourceLocation location_;
    ErrorSeverity severity_;
};

// 構文エラー
class SyntaxError : public CompilerError {
public:
    SyntaxError(const std::string& message,
                const SourceLocation& location)
        : CompilerError(message, location) {}
};

// 型エラー
class TypeError : public CompilerError {
public:
    TypeError(const std::string& message,
             const SourceLocation& location)
        : CompilerError(message, location) {}
};

// 量子状態エラー
class QuantumError : public CompilerError {
public:
    QuantumError(const std::string& message,
                const SourceLocation& location)
        : CompilerError(message, location) {}
};

// トポロジカルエラー
class TopologyError : public CompilerError {
public:
    TopologyError(const std::string& message,
                 const SourceLocation& location)
        : CompilerError(message, location) {}
};

// エラーハンドラ
class ErrorHandler {
public:
    void addError(std::unique_ptr<CompilerError> error) {
        errors_.push_back(std::move(error));
    }

    bool hasErrors() const {
        return !errors_.empty();
    }

    bool hasFatalErrors() const {
        for (const auto& error : errors_) {
            if (error->getSeverity() == ErrorSeverity::Fatal) {
                return true;
            }
        }
        return false;
    }

    void clear() {
        errors_.clear();
    }

    std::string getErrorReport() const {
        std::string report;
        for (const auto& error : errors_) {
            report += error->formatError() + "\n";
        }
        return report;
    }

    const std::vector<std::unique_ptr<CompilerError>>& getErrors() const {
        return errors_;
    }

private:
    std::vector<std::unique_ptr<CompilerError>> errors_;
};

} // namespace compiler
} // namespace topos

#endif // TOPOS_COMPILER_ERROR_ERROR_HPP
