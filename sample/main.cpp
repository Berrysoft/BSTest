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
        are_equal(1, 2);
        are_noteq(1, 2);
    }
    void test2()
    {
        throws_ex(const exception&, []() { throw exception{}; }, [](const exception&) { return true; });
    }
};

int main()
{
    test_manager manager{};
    manager.add<test_class1>();
    try
    {
        manager.run();
        println("Test success!");
    }
    catch (const assert_failed& e)
    {
        println(cerr, "Test failed:\n{}", e.what());
    }
    return 0;
}
