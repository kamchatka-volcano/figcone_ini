#ifndef FIGCONE_INI_PARSER_H
#define FIGCONE_INI_PARSER_H

#include <figcone_tree/iparser.h>
#include <functional>
#include <map>
#include <memory>
#include <string>

namespace figcone::ini {

class Parser : public IParser {
    class Impl;

public:
    Parser();
    Tree parse(std::istream& stream) override;
    ~Parser() override;
    Parser(const Parser&) = delete;
    Parser(Parser&&) = delete;
    Parser& operator=(const Parser&) = delete;
    Parser& operator=(Parser&&) = delete;

private:
    std::unique_ptr<Impl> impl_;
};

} //namespace figcone::ini

#endif //FIGCONE_INI_PARSER_H