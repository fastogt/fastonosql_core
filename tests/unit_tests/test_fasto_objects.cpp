#include <gtest/gtest.h>

#include <fastonosql/core/global.h>

using namespace fastonosql::core;

TEST(FastoObject, LifeTime) {
  FastoObjectIPtr obj = FastoObject::CreateRoot(GEN_CMD_STRING("root"));
  obj.reset();
  FastoObject* ptr = obj.get();
  ASSERT_TRUE(ptr == NULL);
}

TEST(FastoObject, LifeTimeScope) {
  common::StringValue* obj = common::Value::CreateStringValue(GEN_CMD_STRING("Sasha"));
  {
    FastoObjectIPtr root = FastoObject::CreateRoot(GEN_CMD_STRING("root"));
    FastoObject* ptr = new FastoObject(root.get(), obj, "/n");
    root->AddChildren(ptr);
  }
}
