#ifndef TEST_ORDER_STORAGE_HPP
#define TEST_ORDER_STORAGE_HPP

#include "orderStorage.hpp"
#include <gtest/gtest.h>
#include <iostream>
#include <sstream>

class CoutRedirect
{
  public:
    CoutRedirect(std::streambuf* new_buffer);
    ~CoutRedirect();

  private:
    std::streambuf* old;
};

class OrderStorageTest : public ::testing::Test
{
  protected:
    void SetUp() override;
};

#endif // TEST_ORDER_STORAGE_HPP
