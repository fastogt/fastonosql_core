#include <gtest/gtest.h>

#include <fastonosql/core/types.h>

using namespace fastonosql::core;

TEST(ReadableString, TextConversion) {
  using namespace fastonosql::core;
  ReadableString r;
  ASSERT_EQ(r.GetData(), readable_string_t());
  const readable_string_t valid = {'H', 'e', 'l', 'l', 'o'};
  r.SetData(valid);
  ASSERT_EQ(r.GetType(), ReadableString::TEXT_DATA);
  ASSERT_EQ(r.GetData(), valid);
  ASSERT_EQ(r.GetHumanReadable(), valid);

  const readable_string_t valid_with_rn = {'H', 'e', 'l', 'l', 'o', '\r', '\n'};
  r.SetData(valid_with_rn);
  ASSERT_EQ(r.GetType(), ReadableString::TEXT_DATA);
  ASSERT_EQ(r.GetData(), valid_with_rn);
  ASSERT_EQ(r.GetHumanReadable(), valid_with_rn);

  const readable_string_t valid_bin = {'H', 'e', 'l', 'l', 'o', 0, '1', '2', '3'};
  const readable_string_t valid_bin_str = {'\\', 'x', '4', '8', '\\', 'x', '6', '5', '\\', 'x', '6', 'c',
                                                '\\', 'x', '6', 'c', '\\', 'x', '6', 'f', '\\', 'x', '0', '0',
                                                '\\', 'x', '3', '1', '\\', 'x', '3', '2', '\\', 'x', '3', '3'};
  r.SetData(valid_bin);
  ASSERT_EQ(r.GetType(), ReadableString::BINARY_DATA);
  ASSERT_EQ(r.GetData(), valid_bin);
  ASSERT_EQ(r.GetHumanReadable(), valid_bin_str);
}
