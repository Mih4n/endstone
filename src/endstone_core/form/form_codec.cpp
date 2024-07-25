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

#include "endstone/detail/form/form_codec.h"

#include <entt/entt.hpp>
#include <nlohmann/json.hpp>

#include "endstone/form/action_form.h"
#include "endstone/form/controls/button.h"
#include "endstone/form/message_form.h"
#include "endstone/message.h"

namespace endstone::detail {

template <>
nlohmann::json FormCodec::toJson(const Message &message)
{
    return std::visit(entt::overloaded{[&](const std::string &arg) -> nlohmann::json { return arg; },
                                       [&](const Translatable &arg) -> nlohmann::json {
                                           nlohmann::json json;
                                           json["rawtext"].push_back({
                                               {"translate", arg.getTranslationKey()},
                                               {"with", arg.getParameters()},
                                           });
                                           return json;
                                       }},
                      message);
}

/**
 * Controls
 */
template <>
nlohmann::json FormCodec::toJson(const Button &button)
{
    nlohmann::json btn;
    btn["text"] = toJson(button.getText());
    auto icon = button.getIcon();
    if (icon.has_value()) {
        if (icon.value().rfind("http://", 0) == 0 || icon.value().rfind("https://", 0) == 0) {
            btn["image"]["type"] = "url";
        }
        else {
            btn["image"]["type"] = "path";
        }
        btn["image"]["data"] = icon.value();
    }
    return btn;
}

/**
 * Forms
 */
template <>
nlohmann::json FormCodec::toJson(const MessageForm &form)
{
    nlohmann::json json;
    json["type"] = "modal";
    json["title"] = toJson(form.getTitle());
    json["content"] = toJson(form.getContent());
    json["button1"] = toJson(form.getButton1());
    json["button2"] = toJson(form.getButton2());
    return json;
}

template <>
nlohmann::json FormCodec::toJson(const ActionForm &form)
{
    nlohmann::json json;
    json["type"] = "form";
    json["title"] = toJson(form.getTitle());
    json["content"] = toJson(form.getContent());
    for (const auto &button : form.getButtons()) {
        json["buttons"].push_back(toJson(button));
    }
    return json;
}

}  // namespace endstone::detail
