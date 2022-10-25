#ifndef FIGCONE_INI_PARSER_H
#define FIGCONE_INI_PARSER_H

#include <figcone_tree/iparser.h>
#include <string>
#include <map>
#include <functional>
#include <memory>

namespace figcone::ini{

class Parser : public IParser {
    class Impl;

public:
    Parser();
    TreeNode parse(std::istream& stream) override;
    ~Parser() override;
    Parser(const Parser&) = delete;
    Parser(Parser&&) = delete;
    Parser& operator=(const Parser&) = delete;
    Parser& operator=(Parser&&) = delete;

private:
    std::unique_ptr<Impl> impl_;
};

}

#endif //FIGCONE_INI_PARSER_H