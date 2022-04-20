#pragma once
#include <figcone_tree/streamposition.h>
#include <string>
#include <istream>

namespace figcone::ini::detail{

class Stream{
public:
    explicit Stream(std::istream& stream, const StreamPosition& startPosition = StreamPosition{1, 1})
        : stream_(stream)
        , startPosition_(startPosition)
    {
    }

    Stream(Stream&&) = default;
    Stream(const Stream&) = delete;
    Stream& operator=(const Stream&) = delete;

    void skip(int size)
    {
        read(size);
    }

    std::string read(int size = 1)
    {
        auto result = std::string{};
        auto ch = char{};
        for (auto i = 0; i < size; ++i){
            if (!stream_.get(ch))
                return {};
            if (ch == '\n'){
                (*position_.line)++;
                (*position_.column) = 0;
            }
            else if (ch == '\t')
                (*position_.column) += 4;
            else
                (*position_.column)++;
            result.push_back(ch);
        }
        return result;
    }

    std::string peek(int size = 1)
    {
        auto result = std::string{};
        auto ch = char{};
        auto pos = stream_.tellg();
        for (auto i = 0; i < size; ++i){
            if (!stream_.get(ch)){
                stream_.clear();
                result.clear();
                break;
            }
            result.push_back(ch);
        }
        stream_.seekg(pos);
        return result;
    }

    bool atEnd()
    {
        return peek().empty();
    }

    StreamPosition position() const
    {
        return {*startPosition_.line + *position_.line,
                *startPosition_.column + *position_.column};
    }


private:
    void skipLine()
    {
        auto ch = char{};
        auto prevPos = stream_.tellg();
        while (stream_.get(ch)) {
            if (ch == '\n') {
                stream_.seekg(prevPos);
                break;
            }
            prevPos = stream_.tellg();
        }
    }

private:
    std::istream& stream_;
    StreamPosition position_ = {0, 0};
    StreamPosition startPosition_ = {0, 0};
};

}
