#include <gtest/gtest.h>

#include <common/sprintf.h>

#include <fastonosql/core/command_holder.h>
#include <fastonosql/core/internal/command_handler.h>

#define SET GEN_CMD_STRING("SET")
#define GET GEN_CMD_STRING("GET")
#define GET2 GEN_CMD_STRING("GET2")
#define CONFIG GEN_CMD_STRING("CONFIG")
#define GET_CONFIG GEN_CMD_STRING("GET CONFIG")
#define GET_CONFIG_INVALID GEN_CMD_STRING("GET CONFIGE")

using namespace fastonosql;

core::internal::CommandHandler* ghand = NULL;

common::Error test(core::internal::CommandHandler* handler, core::commands_args_t argv, core::FastoObject* out) {
  UNUSED(argv);

  CHECK(handler == ghand);
  CHECK(out == NULL);

  return common::Error();
}

static const std::vector<core::CommandHolder> cmds = {core::CommandHolder(SET,
                                                                          "<key> <value>",
                                                                          "Set the value of a key.",
                                                                          UNDEFINED_SINCE,
                                                                          UNDEFINED_EXAMPLE_STR,
                                                                          2,
                                                                          0,
                                                                          core::CommandInfo::Native,
                                                                          &test),
                                                      core::CommandHolder(GET_CONFIG,
                                                                          "<key>",
                                                                          "Set the value of a key.",
                                                                          UNDEFINED_SINCE,
                                                                          UNDEFINED_EXAMPLE_STR,
                                                                          1,
                                                                          0,
                                                                          core::CommandInfo::Native,
                                                                          &test),
                                                      core::CommandHolder(GET2,
                                                                          "<key>",
                                                                          "Set the value of a key.",
                                                                          UNDEFINED_SINCE,
                                                                          UNDEFINED_EXAMPLE_STR,
                                                                          1,
                                                                          0,
                                                                          core::CommandInfo::Native,
                                                                          &test)};

class FakeTranslator : public core::ICommandTranslator {
 public:
  explicit FakeTranslator(const std::vector<core::CommandHolder>& commands) : core::ICommandTranslator(commands) {}
  virtual const char* GetDBName() const override { return "Fake"; }

 private:
  virtual common::Error CreateKeyCommandImpl(const core::NDbKValue& key,
                                             core::command_buffer_t* cmdstring) const override {
    UNUSED(key);
    UNUSED(cmdstring);
    return common::Error();
  }
  virtual common::Error LoadKeyCommandImpl(const core::NKey& key,
                                           common::Value::Type type,
                                           core::command_buffer_t* cmdstring) const override {
    UNUSED(key);
    UNUSED(type);
    UNUSED(cmdstring);
    return common::Error();
  }
  virtual common::Error DeleteKeyCommandImpl(const core::NKey& key, core::command_buffer_t* cmdstring) const override {
    UNUSED(key);
    UNUSED(cmdstring);
    return common::Error();
  }
  virtual common::Error RenameKeyCommandImpl(const core::NKey& key,
                                             const core::key_t& new_name,
                                             core::command_buffer_t* cmdstring) const override {
    UNUSED(key);
    UNUSED(new_name);
    UNUSED(cmdstring);
    return common::Error();
  }
  virtual common::Error ChangeKeyTTLCommandImpl(const core::NKey& key,
                                                core::ttl_t ttl,
                                                core::command_buffer_t* cmdstring) const override {
    UNUSED(key);
    UNUSED(ttl);
    UNUSED(cmdstring);
    return common::Error();
  }
  virtual common::Error LoadKeyTTLCommandImpl(const core::NKey& key, core::command_buffer_t* cmdstring) const override {
    UNUSED(key);
    UNUSED(cmdstring);
    return common::Error();
  }

  virtual bool IsLoadKeyCommandImpl(const core::CommandInfo& cmd) const override {
    UNUSED(cmd);
    return false;
  }

  virtual common::Error PublishCommandImpl(const core::NDbPSChannel& channel,
                                           const std::string& message,
                                           core::command_buffer_t* cmdstring) const override {
    UNUSED(channel);
    UNUSED(message);
    UNUSED(cmdstring);
    return common::Error();
  }
  virtual common::Error SubscribeCommandImpl(const core::NDbPSChannel& channel,
                                             core::command_buffer_t* cmdstring) const override {
    UNUSED(channel);
    UNUSED(cmdstring);
    return common::Error();
  }
};

TEST(CommandHolder, execute) {
  FakeTranslator* ft = new FakeTranslator(cmds);
  core::internal::CommandHandler* hand = new core::internal::CommandHandler(ft);
  ghand = hand;
  const core::commands_args_t cmd_valid_set = {SET, GEN_CMD_STRING("alex"), GEN_CMD_STRING("palec")};
  common::Error err = hand->Execute(cmd_valid_set, NULL);
  ASSERT_FALSE(err);

  const core::commands_args_t cmd_invalid_set = {SET, GEN_CMD_STRING("alex")};
  err = hand->Execute(cmd_invalid_set, NULL);
  ASSERT_TRUE(err);

  const core::commands_args_t cmd_not_exists = {GET, GEN_CMD_STRING("alex")};
  err = hand->Execute(cmd_not_exists, NULL);
  ASSERT_TRUE(err);

  const core::commands_args_t cmd_get_config = {GET, CONFIG, GEN_CMD_STRING("alex")};
  err = hand->Execute(cmd_get_config, NULL);
  ASSERT_TRUE(!err);

  const core::commands_args_t cmd_get_config_invalid = {GET_CONFIG_INVALID, GEN_CMD_STRING("alex")};
  err = hand->Execute(cmd_get_config_invalid, NULL);
  ASSERT_TRUE(err);

  const core::commands_args_t cmd_get2 = {GET2, GEN_CMD_STRING("alex")};
  err = hand->Execute(cmd_get2, NULL);
  ASSERT_TRUE(!err);

  const core::commands_args_t cmd_get_config_many_args = {GET, CONFIG, GEN_CMD_STRING("last"), GEN_CMD_STRING("alex")};
  err = hand->Execute(cmd_get_config_many_args, NULL);
  ASSERT_TRUE(err);

  delete hand;
}
