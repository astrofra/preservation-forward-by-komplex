#include "app/intro_script.h"

#include <cctype>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>

namespace forward_offline {

namespace {

const char* const kIntroScriptLines[] = {
    "init mute95",
    "init domina",
    "loaded",
    "mod 1",
    "_000",
    "show mute95",
    "go 13",
    "_300 msg mute95 saviour",
    "_500 msg mute95 jmagic",
    "_700 msg mute95 jugi",
    "_900 msg mute95 anis",
    "_b00 msg mute95 carebear",
    "_d00 shutdown",
    "show domina",
    "go 16",
    "_f00 msg domina fade2black",
    "_1024",
    "clear24 0",
    "mod 2",
    "killmod 1",
    "__0 filmbox",
    "kill domina",
    "kill mute95"
};

const char* const kKukotScriptLines[] = {
    "_900 msg kukot suh",
    "__10 msg kukot suh",
    "__10 msg kukot suh",
    "__10 msg kukot suh",
    "_900 msg kukot suh2",
    "_a00 msg kukot suh1",
    "_b00 msg kukot suh0",
    "__04 msg kukot suh",
    "__04 msg kukot suh",
    "__04 msg kukot suh",
    "__10 msg kukot suh0",
    "__10 msg kukot suh0",
    "__04 msg kukot suh",
    "__04 msg kukot suh",
    "__04 msg kukot suh",
    "__10 msg kukot suh0",
    "__04 msg kukot suh1",
    "__04 msg kukot suh1",
    "__04 msg kukot suh1",
    "_c00 msg kukot suh0",
    "__10 msg kukot suh0",
    "__10 msg kukot suh0",
    "__04 msg kukot suh",
    "__04 msg kukot suh",
    "__04 msg kukot suh",
    "__10 msg kukot suh1",
    "__04 msg kukot suh2",
    "__04 msg kukot suh2",
    "__04 msg kukot suh2",
    "_d00 shutdown"
};

const char* const kMakuScriptLines[] = {
    "_d00 msg maku go 160.5",
    "msg maku speed -3.0",
    "_e00 msg maku go 25.5",
    "msg maku speed 2",
    "_e20 msg maku go 0",
    "msg maku speed 2.5",
    "_f00 msg maku go 42.5",
    "msg maku speed -2",
    "_f20 msg maku ksor",
    "msg maku go 55.5",
    "msg maku speed 4",
    "__8 msg maku ksor",
    "__8 msg maku ksor",
    "__4 msg maku ksor",
    "__4 msg maku ksor",
    "__4 msg maku ksor",
    "_1000 shutdown"
};

const char* const kWatercubeScriptLines[] = {
    "_1004 msg watercube pum",
    "__4 msg watercube rok",
    "__4 msg watercube suh",
    "_1030 msg watercube pum",
    "_1100 msg watercube rok",
    "msg watercube pum",
    "__10 msg watercube suh0",
    "__18 msg watercube suh0",
    "__8 msg watercube suh0",
    "_1200 msg watercube suh1",
    "msg watercube pum",
    "msg watercube rok",
    "__10 msg watercube suh0",
    "msg watercube tex0",
    "__10 msg watercube suh1",
    "msg watercube tex1",
    "__10 msg watercube suh0",
    "msg watercube tex2",
    "_1300 shutdown"
};

const char* const kFetaScriptLines[] = {
    "_1230 msg feta 1",
    "_1300 show feta",
    "go 21",
    "_1520 msg feta blackfeta",
    "_1530 msg feta blackmuna",
    "_1600 shutdown"
};

std::string trim(const std::string& value) {
    std::size_t start = 0;
    while (start < value.size() &&
           std::isspace(static_cast<unsigned char>(value[start])) != 0) {
        ++start;
    }

    std::size_t end = value.size();
    while (end > start &&
           std::isspace(static_cast<unsigned char>(value[end - 1])) != 0) {
        --end;
    }

    return value.substr(start, end - start);
}

ScriptCommand make_command(unsigned int song_position_hex, const std::string& raw_command) {
    ScriptCommand command;
    command.song_position_hex = song_position_hex;

    std::istringstream stream(raw_command);
    stream >> command.verb;
    if (stream >> command.target) {
        std::string remainder;
        std::getline(stream, remainder);
        command.argument = trim(remainder);
    }

    return command;
}

unsigned int parse_song_position_hex(const std::string& token) {
    return static_cast<unsigned int>(std::strtoul(token.c_str(), NULL, 16));
}

std::vector<ScriptCommand> build_script_commands(const char* const* lines, std::size_t count) {
    std::vector<ScriptCommand> commands;
    unsigned int current_song_position = 0;

    for (std::size_t index = 0; index < count; ++index) {
        const std::string line = trim(lines[index]);
        if (line.empty() || line[0] == '#') {
            continue;
        }

        if (line[0] == '_') {
            std::size_t prefix_length = 1;
            bool relative = false;
            if (line.size() > 1 && line[1] == '_') {
                prefix_length = 2;
                relative = true;
            }

            const std::size_t separator = line.find(' ');
            const std::string position_token = separator == std::string::npos
                                                   ? line.substr(prefix_length)
                                                   : line.substr(prefix_length, separator - prefix_length);
            const unsigned int parsed_position = parse_song_position_hex(position_token);
            current_song_position = relative ? current_song_position + parsed_position : parsed_position;

            if (separator != std::string::npos) {
                commands.push_back(make_command(
                    current_song_position,
                    trim(line.substr(separator + 1))));
            }
            continue;
        }

        commands.push_back(make_command(current_song_position, line));
    }

    return commands;
}

}  // namespace

IntroScript::IntroScript() : commands_(build_commands()) {
}

const std::vector<ScriptCommand>& IntroScript::commands() const {
    return commands_;
}

std::string IntroScript::next_position_hex(std::size_t next_index) const {
    if (next_index >= commands_.size()) {
        return std::string();
    }

    std::ostringstream builder;
    builder << "0x" << std::hex << commands_[next_index].song_position_hex;
    return builder.str();
}

std::vector<ScriptCommand> IntroScript::build_commands() {
    return build_script_commands(kIntroScriptLines,
                                 sizeof(kIntroScriptLines) / sizeof(kIntroScriptLines[0]));
}

KukotScript::KukotScript() : commands_(build_commands()) {
}

const std::vector<ScriptCommand>& KukotScript::commands() const {
    return commands_;
}

std::string KukotScript::next_position_hex(std::size_t next_index) const {
    if (next_index >= commands_.size()) {
        return std::string();
    }

    std::ostringstream builder;
    builder << "0x" << std::hex << commands_[next_index].song_position_hex;
    return builder.str();
}

std::vector<ScriptCommand> KukotScript::build_commands() {
    return build_script_commands(kKukotScriptLines,
                                 sizeof(kKukotScriptLines) / sizeof(kKukotScriptLines[0]));
}

MakuScript::MakuScript() : commands_(build_commands()) {
}

const std::vector<ScriptCommand>& MakuScript::commands() const {
    return commands_;
}

std::string MakuScript::next_position_hex(std::size_t next_index) const {
    if (next_index >= commands_.size()) {
        return std::string();
    }

    std::ostringstream builder;
    builder << "0x" << std::hex << commands_[next_index].song_position_hex;
    return builder.str();
}

std::vector<ScriptCommand> MakuScript::build_commands() {
    return build_script_commands(kMakuScriptLines,
                                 sizeof(kMakuScriptLines) / sizeof(kMakuScriptLines[0]));
}

WatercubeScript::WatercubeScript() : commands_(build_commands()) {
}

const std::vector<ScriptCommand>& WatercubeScript::commands() const {
    return commands_;
}

std::string WatercubeScript::next_position_hex(std::size_t next_index) const {
    if (next_index >= commands_.size()) {
        return std::string();
    }

    std::ostringstream builder;
    builder << "0x" << std::hex << commands_[next_index].song_position_hex;
    return builder.str();
}

std::vector<ScriptCommand> WatercubeScript::build_commands() {
    return build_script_commands(kWatercubeScriptLines,
                                 sizeof(kWatercubeScriptLines) / sizeof(kWatercubeScriptLines[0]));
}

FetaScript::FetaScript() : commands_(build_commands()) {
}

const std::vector<ScriptCommand>& FetaScript::commands() const {
    return commands_;
}

std::string FetaScript::next_position_hex(std::size_t next_index) const {
    if (next_index >= commands_.size()) {
        return std::string();
    }

    std::ostringstream builder;
    builder << "0x" << std::hex << commands_[next_index].song_position_hex;
    return builder.str();
}

std::vector<ScriptCommand> FetaScript::build_commands() {
    return build_script_commands(kFetaScriptLines,
                                 sizeof(kFetaScriptLines) / sizeof(kFetaScriptLines[0]));
}

}  // namespace forward_offline
