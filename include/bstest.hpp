#ifndef BSTEST_HPP
#define BSTEST_HPP

#include <exception>
#include <functional>
#include <map>
#include <memory>
#include <sf/sformat.hpp>

#if defined(_MSC_VER)
#define __CURRENT_FUNC__ __FUNCSIG__
#elif defined(__clang__) || defined(__GNUC__)
#define __CURRENT_FUNC__ __PRETTY_FUNCTION__
#else
#define __CURRENT_FUNC__ __func__
#endif

namespace bstest
{
    class assert_failed : public std::exception
    {
    private:
        std::string msg;

    public:
        assert_failed(const std::string& msg) : msg(msg) {}
        ~assert_failed() override {}

        const char* what() const noexcept override { return msg.c_str(); }
    };

    namespace impl
    {
        inline void check(bool value, const std::string& msg)
        {
            if (!value) throw assert_failed{ msg };
        }

        inline std::string make_line_info(const char* func, const char* file, std::size_t line)
        {
            return sf::sprint("File: {}\nLine: {}\nFunc: {}", file, line, func);
        }

        template <typename T1, typename T2>
        inline std::string make_eqmsg(T1&& expected, T2&& actual, const char* func, const char* file, std::size_t line)
        {
            return sf::sprint("Expected: <{:b}> as {}\nActual: <{:b}> as {}\n{}", std::forward<T1>(expected), typeid(T1).name(), std::forward<T2>(actual), typeid(T2).name(), make_line_info(func, file, line));
        }
        template <typename T1, typename T2>
        inline std::string make_neqmsg(T1&& expected, T2&& actual, const char* func, const char* file, std::size_t line)
        {
            return sf::sprint("Expected not: <{:b}> as {}\nActual: <{:b}> as {}\n{}", std::forward<T1>(expected), typeid(T1).name(), std::forward<T2>(actual), typeid(T2).name(), make_line_info(func, file, line));
        }
        template <typename T>
        inline std::string make_msg(T&& expected, const char* func, const char* file, std::size_t line)
        {
            return sf::sprint("Expected: <{:b}> as {}\n{}", std::forward<T>(expected), typeid(T).name(), make_line_info(func, file, line));
        }
        template <typename T>
        inline std::string make_msg(T&& expected, const char* expr, const char* func, const char* file, std::size_t line)
        {
            return sf::sprint("Expected: <{:b}> as {}\nExpression: <{}>\n{}", std::forward<T>(expected), typeid(T).name(), expr, make_line_info(func, file, line));
        }
    } // namespace impl

    inline void is_true(bool value, const char* expr, const char* func, const char* file, std::size_t line) { impl::check(value, impl::make_msg(true, expr, func, file, line)); }
    inline void is_false(bool value, const char* expr, const char* func, const char* file, std::size_t line) { impl::check(!value, impl::make_msg(false, expr, func, file, line)); }

    template <typename T1, typename T2>
    inline void are_equal(T1&& expected, T2&& actual, const char* func, const char* file, std::size_t line)
    {
        impl::check(expected == actual && actual == expected, impl::make_eqmsg(expected, actual, func, file, line));
    }
    template <typename T1, typename T2>
    inline void are_noteq(T1&& expected, T2&& actual, const char* func, const char* file, std::size_t line)
    {
        impl::check(expected != actual && actual != expected, impl::make_neqmsg(expected, actual, func, file, line));
    }

    template <typename Ex, typename Action, typename Pred>
    inline void throws_ex(Action&& action, Pred&& pred, const char* func, const char* file, std::size_t line)
    {
        try
        {
            std::forward<Action>(action)();
        }
        catch (Ex e)
        {
            impl::check(pred(e), impl::make_msg(true, func, file, line));
        }
        catch (...)
        {
            impl::check(false, impl::make_msg(typeid(Ex).name(), func, file, line));
        }
    }

#define __call_assert_func(func, ...) func(__VA_ARGS__, __CURRENT_FUNC__, __FILE__, __LINE__)
#define is_true(value) __call_assert_func(is_true, !!value, #value)
#define is_false(value) __call_assert_func(is_false, !!value, #value)
#define are_equal(expected, actual) __call_assert_func(are_equal, expected, actual)
#define are_noteq(expected, actual) __call_assert_func(are_noteq, expected, actual)
#define throws_ex(type, action, pred) __call_assert_func(throws_ex<type>, action, pred)

    class test_base
    {
    private:
        std::map<std::string, std::function<void()>> m_tests;

    public:
        virtual ~test_base() {}

        void run()
        {
            for (auto& pair : m_tests)
            {
                (pair.second)();
            }
        }
        void run(const std::string& name)
        {
            auto it{ m_tests.find(name) };
            if (it != m_tests.end())
            {
                (it->second)();
            }
        }

    protected:
        void add_test(std::string_view name, std::function<void()> func) { m_tests.emplace(name, move(func)); }
    };

#define add_test(name) add_test(#name, [this]() { name(); })

    class test_manager
    {
    private:
        std::map<std::string, std::unique_ptr<test_base>> m_test_classes;

    public:
        template <typename T, typename... Args>
        void add(Args&&... args)
        {
            m_test_classes.emplace(typeid(T).name(), std::make_unique<T>(std::forward<Args>(args)...));
        }

        void run()
        {
            for (auto& pair : m_test_classes)
            {
                pair.second->run();
            }
        }
        template <typename T = test_base>
        T* get_test(const std::string& name)
        {
            auto it{ m_test_classes.find(name) };
            if (it != m_test_classes.end())
            {
                return (T*)(it->second.get());
            }
            return nullptr;
        }
        void run(const std::string& name)
        {
            if (test_base * cls{ get_test(name) })
            {
                cls->run();
            }
        }
        void run(const std::string& name, const std::string& test_name)
        {
            if (test_base * cls{ get_test(name) })
            {
                cls->run(test_name);
            }
        }
    };
} // namespace bstest

#endif