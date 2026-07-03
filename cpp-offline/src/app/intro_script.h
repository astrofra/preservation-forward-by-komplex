#ifndef FORWARD_OFFLINE_APP_INTRO_SCRIPT_H
#define FORWARD_OFFLINE_APP_INTRO_SCRIPT_H

#include <cstddef>
#include <string>
#include <vector>

namespace forward_offline {

struct ScriptCommand {
    unsigned int song_position_hex;
    std::string verb;
    std::string target;
    std::string argument;
};

class IntroScript {
public:
    IntroScript();

    const std::vector<ScriptCommand>& commands() const;
    std::string next_position_hex(std::size_t next_index) const;

private:
    static std::vector<ScriptCommand> build_commands();

    std::vector<ScriptCommand> commands_;
};

class KukotScript {
public:
    KukotScript();

    const std::vector<ScriptCommand>& commands() const;
    std::string next_position_hex(std::size_t next_index) const;

private:
    static std::vector<ScriptCommand> build_commands();

    std::vector<ScriptCommand> commands_;
};

}  // namespace forward_offline

#endif  // FORWARD_OFFLINE_APP_INTRO_SCRIPT_H
