#pragma once

#include <array>
#include <queue>
#include <stdexcept>
#include <string>
#include <vector>

#include "multi_match_common.h"

namespace bitonic
{
namespace multi_match
{
namespace cpu
{

class NaiveMatcher
{
public:
    MatchResult match(const TaskData &task) const
    {
        task.validate();

        MatchResult result(task.pattern_count());

        for (std::size_t pat_id = 0; pat_id < task.patterns.size(); ++pat_id)
        {
            const std::string &pat = task.patterns[pat_id].value;

            if (pat.size() > task.text.size())
            {
                result.counts[pat_id] = 0;
                continue;
            }

            count_type count = 0;

            for (std::size_t pos = 0; pos + pat.size() <= task.text.size(); ++pos)
            {
                bool ok = true;

                for (std::size_t j = 0; j < pat.size(); ++j)
                {
                    if (task.text[pos + j] != pat[j])
                    {
                        ok = false;
                        break;
                    }
                }

                if (ok)
                    ++count;
            }

            result.counts[pat_id] = count;
        }

        return result;
    }
};

class AhoNode
{
public:
    std::array<int, kAlphabetSize> next_;
    int                            link_{};
    std::vector<std::uint32_t>     out_;

    AhoNode() {next_.fill(-1);}
};

class AhoCorasickMatcher
{
private:
    std::vector<AhoNode> nodes_;
    std::size_t          pattern_count_{0};

    void build_trie(const std::vector<std::string> &patterns)
    {
        nodes_.clear();
        nodes_.push_back(AhoNode{});

        for (std::size_t pat_id = 0; pat_id < patterns.size(); ++pat_id)
        {
            const std::string &pat = patterns[pat_id];

            if (pat.empty())
                throw std::runtime_error("Empty pattern is not allowed");

            int state = 0;

            for (std::size_t i = 0; i < pat.size(); ++i)
            {
                const unsigned char ch = static_cast<unsigned char>(pat[i]);

                if (nodes_[static_cast<std::size_t>(state)].next_[ch] == -1)
                {
                    const int new_state = static_cast<int>(nodes_.size());
                    nodes_[static_cast<std::size_t>(state)].next_[ch] = new_state;
                    nodes_.push_back(AhoNode{});
                }

                state = nodes_[static_cast<std::size_t>(state)].next_[ch];
            }

            nodes_[static_cast<std::size_t>(state)].out_.push_back(
                static_cast<std::uint32_t>(pat_id));
        }
    }

    void build_links()
    {
        std::queue<int> q;

        for (std::size_t c = 0; c < kAlphabetSize; ++c)
        {
            int &to = nodes_[0].next_[c];

            if (to == -1)
                to = 0;
            else
            {
                nodes_[static_cast<std::size_t>(to)].link_ = 0;
                q.push(to);
            }
        }

        while (!q.empty())
        {
            const int v = q.front();
            q.pop();

            for (std::size_t c = 0; c < kAlphabetSize; ++c)
            {
                int &to = nodes_[static_cast<std::size_t>(v)].next_[c];

                if (to == -1)
                {
                    to = nodes_[static_cast<std::size_t>(
                         nodes_[static_cast<std::size_t>(v)].link_)].next_[c];
                    continue;
                }

                const int link_state =
                    nodes_[static_cast<std::size_t>(
                        nodes_[static_cast<std::size_t>(v)].link_)].next_[c];

                nodes_[static_cast<std::size_t>(to)].link_ = link_state;

                const std::vector<std::uint32_t> &suffix_out =
                    nodes_[static_cast<std::size_t>(link_state)].out_;

                nodes_[static_cast<std::size_t>(to)].out_.insert(
                    nodes_[static_cast<std::size_t>(to)].out_.end(),
                    suffix_out.begin(),
                    suffix_out.end());

                q.push(to);
            }
        }
    }

public:
    explicit AhoCorasickMatcher(const std::vector<std::string> &patterns)
        : pattern_count_(patterns.size())
    {
        build_trie(patterns);
        build_links();
    }

    MatchResult match(const std::string &text) const
    {
        MatchResult result(pattern_count_);

        int state = 0;

        for (std::size_t i = 0; i < text.size(); ++i)
        {
            const unsigned char ch = static_cast<unsigned char>(text[i]);
            state = nodes_[static_cast<std::size_t>(state)].next_[ch];

            const std::vector<std::uint32_t> &out =
                nodes_[static_cast<std::size_t>(state)].out_;

            for (std::size_t k = 0; k < out.size(); ++k)
                ++result.counts[out[k]];
        }

        return result;
    }

    MatchResult match(const TaskData &task) const
    {
        task.validate();
        return match(task.text);
    }

    std::size_t state_count() const noexcept
    {
        return nodes_.size();
    }
};

class CpuMatcher
{
public:
    MatchResult match_reference(const TaskData &task) const
    {
        return NaiveMatcher{}.match(task);
    }

    MatchResult match_fast(const TaskData &task) const
    {
        task.validate();
        AhoCorasickMatcher matcher(task.pattern_strings());
        return matcher.match(task.text);
    }
};

} // namespace cpu
} // namespace multi_match
} // namespace bitonic