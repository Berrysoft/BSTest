#include <bstest.hpp>

using namespace std;
using namespace sf;
using namespace bstest;

class test_class1 : public test_base
{
public:
    test_class1()
    {
        add_test(test1);
        add_test(test2);
    }

private:
    void test1()
    {
        is_true(true == false);
        is_false(false);
    }
    void test2()
    {
        throws_ex(const exception&, []() { throw int{}; }, [](const exception&) { return true; });
    }
};

class test_class2 : public test_base
{
public:
    test_class2()
    {
        add_test(test);
    }

private:
    void test()
    {
        are_equal(1, size_t(2));
        are_noteq(1, 2);
    }
};

int main()
{
    test_manager manager{};
    manager.add<test_class1>();
    manager.add<test_class2>();
    println("manager.run():");
    for (auto& e : manager.run())
    {
        println(e.what());
        println();
    }
    println("manager.run<test_class2>():");
    for (auto& e : manager.run<test_class2>())
    {
        println(e.what());
        println();
    }
    println("manager.run_throw<test_class1>(\"test2\"):");
    try
    {
        manager.run_throw<test_class1>("test2");
    }
    catch (const assert_failed& e)
    {
        println(e.what());
    }
    return 0;
}
