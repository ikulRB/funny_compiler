#ifndef AST_CONTEXT_H
#define AST_CONTEXT_H

#include <memory>
#include <utility>
#include <vector>

struct ASTNode;

class ASTContext {
public:
    ASTContext() = default;

    ASTContext(const ASTContext&) = delete;
    ASTContext& operator=(const ASTContext&) = delete;

    ASTContext(ASTContext&&) = default;
    ASTContext& operator=(ASTContext&&) = default;

    template <typename T, typename... Args>
    T *make(Args &&...args) {
        auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
        T *raw = ptr.get();
        nodes_.push_back(std::move(ptr));
        return raw;
    }

private:
    std::vector<std::unique_ptr<ASTNode>> nodes_;
};

#endif  // AST_CONTEXT_H