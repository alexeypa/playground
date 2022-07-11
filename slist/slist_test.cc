#include "slist/slist.h"

#include <algorithm>
#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <iterator>

using namespace testing;

struct item_t
{
    item_t* next{};
    int data{};
};

TEST(SlistTest, Basic)
{
    slist<item_t> foo;
    EXPECT_TRUE(foo.empty());

    item_t item1, item2, item3;

    foo.push_back(&item2);
    EXPECT_FALSE(foo.empty());
    EXPECT_EQ(&foo.front(), &item2);
    EXPECT_EQ(&foo.back(), &item2);
    EXPECT_EQ(item2.next, &item2);

    foo.push_front(&item1);
    EXPECT_FALSE(foo.empty());
    EXPECT_EQ(&foo.front(), &item1);
    EXPECT_EQ(&foo.back(), &item2);
    EXPECT_EQ(item1.next, &item2);
    EXPECT_EQ(item2.next, &item1);

    foo.push_back(&item3);
    EXPECT_FALSE(foo.empty());
    EXPECT_EQ(&foo.front(), &item1);
    EXPECT_EQ(&foo.back(), &item3);
    EXPECT_EQ(item1.next, &item2);
    EXPECT_EQ(item2.next, &item3);
    EXPECT_EQ(item3.next, &item1);

    EXPECT_EQ(foo.pop_front(), &item1);
    EXPECT_FALSE(foo.empty());
    EXPECT_EQ(item1.next, nullptr);
    EXPECT_EQ(&foo.front(), &item2);
    EXPECT_EQ(&foo.back(), &item3);
    EXPECT_EQ(item2.next, &item3);
    EXPECT_EQ(item3.next, &item2);

    EXPECT_EQ(foo.pop_front(), &item2);
    EXPECT_FALSE(foo.empty());
    EXPECT_EQ(item1.next, nullptr);
    EXPECT_EQ(item2.next, nullptr);
    EXPECT_EQ(&foo.front(), &item3);
    EXPECT_EQ(item3.next, &item3);

    EXPECT_EQ(foo.pop_front(), &item3);
    EXPECT_TRUE(foo.empty());
    EXPECT_EQ(item1.next, nullptr);
    EXPECT_EQ(item2.next, nullptr);
    EXPECT_EQ(item3.next, nullptr);

    EXPECT_EQ(foo.pop_front(), nullptr);
    EXPECT_TRUE(foo.empty());
}

#if !defined(NDEBUG)

TEST(SlistTest, Asserts)
{
    slist<item_t> foo;

    ASSERT_DEATH(foo.front(), "last");
    ASSERT_DEATH(foo.back(), "last");

    item_t item1;
    item1.next = &item1;

    ASSERT_DEATH(foo.push_back(&item1), "!item->next");
    ASSERT_DEATH(foo.push_front(&item1), "!item->next");
}

#endif

TEST(SlistTest, ConstIterator)
{
    slist<item_t> foo;

    item_t item1, item2, item3;
    item1.data = 1;
    item2.data = 2;
    item3.data = 3;

    auto i = foo.cbegin();
    EXPECT_EQ(i, foo.cbegin());
    EXPECT_EQ(i, foo.cend());

    foo.push_back(&item1);
    i = foo.cbegin();
    EXPECT_EQ(i->data, 1);
    auto j = i;
    EXPECT_EQ(++j, foo.cend());
    EXPECT_NE(i++, foo.cend());
    EXPECT_EQ(i, foo.cend());

    foo.push_back(&item2);
    i = foo.cbegin();
    EXPECT_EQ(i->data, 1);
    ++i;
    EXPECT_EQ(i->data, 2);
    i++;
    EXPECT_EQ(i, foo.cend());

    foo.push_back(&item3);
    i = foo.cbegin();
    EXPECT_EQ((*i).data, 1);
    i++;
    EXPECT_EQ((*i).data, 2);
    ++i;
    EXPECT_EQ((*i).data, 3);
    i++;
    EXPECT_EQ(i, foo.cend());

    std::vector<int> result;
    for (const auto& item : foo) {
        result.push_back(item.data);
    }
    EXPECT_THAT(result, ElementsAre(1, 2, 3));

    std::transform(foo.cbegin(),
                   foo.cend(),
                   std::back_inserter(result),
                   [](const item_t& item) { return item.data * 3; });
    EXPECT_THAT(result, ElementsAre(1, 2, 3, 3, 6, 9));
}

TEST(SlistTest, Iterator)
{
    slist<item_t> foo;

    EXPECT_EQ(foo.begin(), foo.cbegin());
    EXPECT_EQ(foo.end(), foo.cend());

    item_t item1, item2, item3;
    item1.data = 1;
    item2.data = 2;
    item3.data = 3;

    foo.push_back(&item1);
    foo.push_back(&item2);
    foo.push_back(&item3);

    auto i = foo.begin();
    EXPECT_EQ(i, foo.cbegin());
    EXPECT_EQ(i->data, 1);
    i++;
    EXPECT_EQ((*i).data, 2);
    ++i;
    EXPECT_EQ((*i).data, 3);
    i++;
    EXPECT_EQ(i, foo.cend());
}

TEST(SlistTest, Insert)
{
    slist<item_t> foo;

    item_t item1, item2, item3, item4;
    item1.data = 1;
    item2.data = 2;
    item3.data = 3;
    item4.data = 4;

    auto i = foo.insert(foo.begin(), &item3);
    EXPECT_EQ(i, foo.begin());
    EXPECT_EQ(i->data, 3);

    i = foo.insert(foo.end(), &item4);
    EXPECT_NE(i, foo.begin());
    EXPECT_NE(i, foo.end());
    EXPECT_EQ(i->data, 4);

    i = foo.insert(foo.begin(), &item1);
    EXPECT_EQ(i, foo.begin());
    EXPECT_EQ(i->data, 1);

    ++i;
    EXPECT_EQ(i->data, 3);
    i = foo.insert(i, &item2);

    EXPECT_EQ(&foo.front(), &item1);
    EXPECT_EQ(&foo.back(), &item4);
    EXPECT_EQ(item1.next, &item2);
    EXPECT_EQ(item2.next, &item3);
    EXPECT_EQ(item3.next, &item4);
    EXPECT_EQ(item4.next, &item1);
}

TEST(SlistTest, Erase)
{
    slist<item_t> foo;
    EXPECT_EQ(foo.erase(foo.begin()), foo.end());
    EXPECT_TRUE(foo.empty());
    EXPECT_EQ(foo.erase(foo.end()), foo.end());
    EXPECT_TRUE(foo.empty());

    item_t item1, item2, item3, item4;
    item1.data = 1;
    item2.data = 2;
    item3.data = 3;
    item4.data = 4;

    foo.push_back(&item1);
    foo.push_back(&item2);
    foo.push_back(&item3);
    foo.push_back(&item4);

    /*
     * Try deleting the last element.
     */
    auto i = foo.begin();
    ++i;
    ++i;
    ++i;
    EXPECT_EQ(i->data, 4);
    i = foo.erase(i);
    EXPECT_EQ(i, foo.end());
    EXPECT_EQ(item4.next, nullptr);

    EXPECT_FALSE(foo.empty());
    EXPECT_EQ(&foo.front(), &item1);
    EXPECT_EQ(item1.next, &item2);
    EXPECT_EQ(item2.next, &item3);
    EXPECT_EQ(item3.next, &item1);
    EXPECT_EQ(&item3, &foo.back());

    /*
     * Delete in the middle of the list.
     */
    i = foo.erase(++foo.begin());
    EXPECT_EQ(i->data, 3);
    EXPECT_EQ(item2.next, nullptr);

    /*
     * Delete in the first element.
     */
    i = foo.erase(foo.begin());
    EXPECT_EQ(i->data, 3);
    EXPECT_EQ(item1.next, nullptr);

    i = foo.erase(i);
    EXPECT_EQ(i, foo.end());
    EXPECT_EQ(item3.next, nullptr);
    EXPECT_TRUE(foo.empty());
}
