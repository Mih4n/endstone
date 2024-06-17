// Copyright (c) 2024, The Endstone Project. (https://endstone.dev) All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#pragma once

#include "bedrock/server/commands/command.h"
#include "endstone/command/command_sender.h"
#include "endstone/detail/permissions/permissible_base.h"
#include "endstone/detail/server.h"

namespace endstone::detail {

class CommandSenderAdapter : public ServerCommandSender {
public:
    CommandSenderAdapter(const CommandOrigin &origin, CommandOutput &output);
    void sendMessage(const std::string &message) const override;
    void sendMessage(const Translatable &message) const override;
    void sendErrorMessage(const std::string &message) const override;
    void sendErrorMessage(const Translatable &message) const override;
    [[nodiscard]] std::string getName() const override;
    [[nodiscard]] bool isOp() const override;
    void setOp(bool value) override;

private:
    const CommandOrigin &origin_;
    CommandOutput &output_;
};

class CommandAdapter : public ::Command {
public:
    void execute(const CommandOrigin &origin, CommandOutput &output) const override;

private:
    friend class EndstoneCommandMap;
    friend class ::CommandRegistry;
    std::vector<std::string> args_;
};

}  // namespace endstone::detail
