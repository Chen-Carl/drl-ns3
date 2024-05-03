#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/fmt/ostr.h>

#include <iostream>

using namespace spdlog;

class Test
{
private:
    std::string m_msg;

public:
    Test(const std::string &msg = "Hello, Spdlog!") : m_msg(msg) {}

    std::string getMsg() const { return m_msg; }

    friend std::ostream &operator<<(std::ostream &os, const Test &test)
    {
        os << test.m_msg;
        return os;
    }
};

template <>
struct fmt::formatter<Test> : fmt::formatter<std::string> 
{
    auto format(Test my, format_context &ctx) -> decltype(ctx.out()) 
    {
        return fmt::format_to(ctx.out(), "[Test i={}]", my.getMsg());
    }
};

int main()
{
    Test test;
    spdlog::info("Hello, Spdlog!");
    spdlog::info("user defined type: {}", test);
    return 0;
}