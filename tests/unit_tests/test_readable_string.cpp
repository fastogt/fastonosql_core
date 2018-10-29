#include <gtest/gtest.h>

#include <fastonosql/core/types.h>

using namespace fastonosql::core;

TEST(ReadableString, TextConversion) {
  using namespace fastonosql::core;
  ReadableString r;
  ASSERT_EQ(r.GetData(), readable_string_data_t());
  const readable_string_data_t valid = {'H', 'e', 'l', 'l', 'o'};
  const readable_string_t valid_str = "Hello";
  r.SetData(valid);
  ASSERT_EQ(r.GetType(), ReadableString::TEXT_DATA);
  ASSERT_EQ(r.GetData(), valid);
  ASSERT_EQ(r.GetHumanReadable(), valid_str);

  const readable_string_data_t valid_with_rn = {'H', 'e', 'l', 'l', 'o', '\r', '\n'};
  const readable_string_t valid_with_rn_str = "Hello\r\n";
  r.SetData(valid_with_rn);
  ASSERT_EQ(r.GetType(), ReadableString::TEXT_DATA);
  ASSERT_EQ(r.GetData(), valid_with_rn);
  ASSERT_EQ(r.GetHumanReadable(), valid_with_rn_str);

  const readable_string_data_t valid_bin = {'H', 'e', 'l', 'l', 'o', 0, '1', '2', '3'};
  const readable_string_t valid_bin_str = "\\x48\\x65\\x6c\\x6c\\x6f\\x00\\x31\\x32\\x33";
  r.SetData(valid_bin);
  ASSERT_EQ(r.GetType(), ReadableString::BINARY_DATA);
  ASSERT_EQ(r.GetData(), valid_bin);
  ASSERT_EQ(r.GetHumanReadable(), valid_bin_str);
}
