#pragma once

#include <cstddef>
#include <cstdint>
#include <istream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

namespace bitonic
{
namespace multi_match
{

using count_type = std::uint32_t;
using state_type = std::uint32_t;

constexpr std::size_t kAlphabetSize = 256;

class PatternDesc
{
public:
    std::size_t declared_size{};
    std::string value;

    void validate(std::size_t index_1based) const
    {
        if (value.empty())
            throw std::runtime_error(
                "Pattern #" + std::to_string(index_1based) + " must not be empty");

        if (value.size() != declared_size)
            throw std::runtime_error(
                "Pattern #"             + std::to_string(index_1based) +
                " has declared size "   + std::to_string(declared_size) +
                ", but actual size is " + std::to_string(value.size()));
    }
};

class TaskData
{
public:
    std::size_t              declared_text_size{};
    std::string              text;
    std::vector<PatternDesc> patterns;

    void validate() const
    {
        if (text.size() != declared_text_size)
            throw std::runtime_error(
                "Text has declared size " + std::to_string(declared_text_size) +
                ", but actual size is "   + std::to_string(text.size()));

        for (std::size_t i = 0; i < patterns.size(); ++i)
            patterns[i].validate(i + 1);
    }

    std::size_t text_size()     const noexcept {return text.size();}
    std::size_t pattern_count() const noexcept {return patterns.size();}

    std::vector<std::string> pattern_strings() const
    {
        std::vector<std::string> result;
        result.reserve(patterns.size());

        for (std::size_t i = 0; i < patterns.size(); ++i)
            result.push_back(patterns[i].value);

        return result;
    }
};

class MatchResult
{
public:
    std::vector<count_type> counts;

    MatchResult() = default;

    explicit MatchResult(std::size_t n)
        : counts(n, 0) {}

    std::size_t size() const noexcept {return counts.size();}

    bool equals(const MatchResult &other) const noexcept
    {
        return counts == other.counts;
    }
};

class TaskIo
{
public:
    static TaskData read(std::istream &in)
    {
        TaskData task;

        in >> task.declared_text_size;
        in >> task.text;

        std::size_t pattern_count{};
        in >> pattern_count;

        task.patterns.reserve(pattern_count);

        for (std::size_t i = 0; i < pattern_count; ++i)
        {
            PatternDesc p;
            in >> p.declared_size;
            in >> p.value;
            task.patterns.push_back(p);
        }

        task.validate();
        return task;
    }

    static void write(const MatchResult &result, std::ostream &out)
    {
        for (std::size_t i = 0; i < result.counts.size(); ++i)
            out << (i + 1) << ' ' << result.counts[i] << '\n';
    }

    static MatchResult read_answer(std::istream &in)
    {
        MatchResult result;

        std::size_t idx{};
        count_type  count{};

        while (in >> idx >> count)
        {
            if (idx != result.counts.size() + 1)
                throw std::runtime_error("Answer file has invalid numbering");

            result.counts.push_back(count);
        }

        return result;
    }
};

} // namespace multi_match
} // namespace bitonic