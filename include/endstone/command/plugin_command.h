//
// Created by Vincent on 25/08/2023.
//

#ifndef ENDSTONE_PLUGIN_COMMAND_H
#define ENDSTONE_PLUGIN_COMMAND_H

#include "command.h"
#include "endstone/command/server_command_sender.h"
#include "endstone/plugin/plugin.h"
#include "endstone/server.h"

class CommandExecutor;

class PluginCommand : public Command {
public:
    friend class PythonPluginDescription;
    friend class CppPluginDescription;

protected:
    PluginCommand(const std::string &name, Plugin &owner) noexcept : Command(name), owner_(owner)
    {
        usage_ = "";
    }

public:
    bool execute(CommandSender &sender, const std::string &label, const std::vector<std::string> &args) const override
    {
        if (!owner_.isEnabled()) {
            throw std::runtime_error("Cannot execute command '" + label + "' in plugin " +
                                     owner_.getDescription().getFullName() + " - plugin is disabled.");
        }

        // TODO: permission check
        //  if (!testPermission(sender)) {
        //      return true;
        //  }

        bool success;
        try {
            success =
                getExecutor().onCommand(sender, *this, label, std::forward<const std::vector<std::string> &>(args));
        }
        catch (std::exception &e) {
            throw std::runtime_error("Unhandled exception executing command '" + label + "' in plugin " +
                                     owner_.getDescription().getFullName() + ": " + e.what());
        }

        if (!success && !usage_.empty()) {
            std::size_t start = 0;
            std::size_t end;
            auto usage = fmt::format(usage_, fmt::arg("command", label));

            while (start < usage.length()) {
                end = usage.find('\n', start);
                if (end == std::string::npos) {
                    end = usage.length();
                }

                auto line = usage.substr(start, end - start);

                if (dynamic_cast<ServerCommandSender *>(&sender)) {
                    sender.getServer().getLogger()->error("Usage: {}", line);
                }
                else {
                    sender.sendMessage("Usage: {}", line);
                }
                start = end + 1;
            }
        }

        return success;
    }

    /**
     * Sets the CommandExecutor to run when parsing this command
     *
     * @param executor New executor to run
     */
    void setExecutor(const std::shared_ptr<CommandExecutor> &executor) noexcept
    {
        executor_ = executor;
    }

    /**
     * Gets the {@link CommandExecutor} associated with this command
     *
     * @return CommandExecutor object linked to this command
     */
    CommandExecutor &getExecutor() const noexcept
    {
        if (executor_) {
            return *executor_;
        }
        else {
            return owner_;
        }
    }

    /**
     * Gets the owner of this PluginCommand
     *
     * @return Plugin that owns this command
     */
    Plugin &getPlugin() const noexcept
    {
        return owner_;
    }

private:
    Plugin &owner_;
    std::shared_ptr<CommandExecutor> executor_;
};

#endif // ENDSTONE_PLUGIN_COMMAND_H
