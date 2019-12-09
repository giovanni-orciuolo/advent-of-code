#include <iostream>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <functional>
#include <array>
#include <algorithm>

// This is an heavily improved version of intcode_computer!

// Utilty functions

std::vector<std::string> split(const std::string& s, char delimiter)
{
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream ts(s);
    while (std::getline(ts, token, delimiter)) tokens.push_back(token);
    return tokens;
}
std::string join(const std::vector<std::string>& v, const std::string& delimiter) {
    std::string s;
    for (auto i = v.begin(); i != v.end(); ++i) s += (*i) + (i == v.end() - 1 ? "" : delimiter);
    return s;
}
std::string join(const std::array<std::string, 5>& v, const std::string& delimiter) {
    std::string s;
    for (auto i = v.begin(); i != v.end(); ++i) s += (*i) + (i == v.end() - 1 ? "" : delimiter);
    return s;
}

// Actual code

struct ProgramState {
    std::string code;
    int ip;
    bool got_settings;
    bool reached_end;

    ProgramState()
        : code(""), ip(0), got_settings(false), reached_end(false) {}
    ProgramState(const std::string& code)
        : code(code), ip(0), got_settings(false), reached_end(false) {}
    ProgramState(const std::string& code, int ip, bool got_settings, bool reached_end)
        : code(code), ip(ip), got_settings(got_settings), reached_end(reached_end) {}

    void reset(const std::string& program) {
        code = program;
        ip = 0;
        got_settings = false;
        reached_end = false;
    }
};

// I know macros are EVIL INCARNATE, but I need them!! :(
#define OP_ARG_STR(index) ((modes.length() > index) && modes[index] == '1') ? params[index] : tokens.at(std::stoi(params[index]))
#define OP_ARG(index) std::stoi(OP_ARG_STR(index))

// True if you want diagnostic output
const bool DEBUG = false;
const bool SHOW_STATES = false;

std::string run_program(
    std::string& code, // Remove the & to avoid modifying the original program
    const std::string& phase_settings,
    const std::string& input,
    ProgramState* state
) {
    // Instruction pointer
    int ip = state->ip;

    // Get tokens by splitting the source code
    std::vector<std::string> tokens = split(code, ',');

    int output = 0;

    if (DEBUG) std::cout << "=== RUNNING PROGRAM ===" << std::endl;

    // Opcodes
    std::unordered_map<std::string, std::function<void(const std::string&, const std::vector<std::string>&)>> opcodes {
        {"ADD", [&tokens, &ip](const std::string& modes, const std::vector<std::string>& params) -> void {
            int arg1 = OP_ARG(0);
            int arg2 = OP_ARG(1);
            int outp = std::stoi(params[2]);

            if (DEBUG) std::cout << ">>>> ADD | arg1 = " << arg1 << " (with mode " << (modes.length() > 0 ? modes[0] : '0') << ")" << (modes.length() > 0 && modes[0] != '1' ? " [" + params[0] + "]" : "") << ", arg2 = " << arg2 << " (with mode " << (modes.length() > 1 ? modes[1] : '0') << ")" << (modes.length() > 1 && modes[1] != '1' ? " [" + params[1] + "]" : "") << ", output position = " << outp << ", output value = " << arg1 + arg2 << std::endl;
            tokens[outp] = std::to_string(arg1 + arg2);
            ip += 4;
        }},
        {"MUL", [&tokens, &ip](const std::string& modes, const std::vector<std::string>& params) -> void {
            int arg1 = OP_ARG(0);
            int arg2 = OP_ARG(1);
            int outp = std::stoi(params[2]);

            if (DEBUG) std::cout << ">>>> MUL | arg1 = " << arg1 << " (with mode " << (modes.length() > 0 ? modes[0] : '0') << ")" << (modes.length() > 0 && modes[0] != '1' ? " [" + params[0] + "]" : "") << ", arg2 = " << arg2 << " (with mode " << (modes.length() > 1 ? modes[1] : '0') << ")" << (modes.length() > 1 && modes[1] != '1' ? " [" + params[1] + "]" : "") << ", output position = " << outp << ", output value = " << arg1 * arg2 << std::endl;
            tokens[outp] = std::to_string(arg1 * arg2);
            ip += 4;
        }},
        {"INP", [&tokens, &ip, phase_settings, input, &state](const std::string& modes, const std::vector<std::string>& params) -> void {
            int outp = std::stoi(params[0]);

            if (DEBUG) std::cout << ">>>> INP | Output position = " << outp << std::endl;
            tokens[outp] = state->got_settings ? input : phase_settings;

            if (!state->got_settings) {
                state->got_settings = true;
            }

            ip += 2;
        }},
        {"OUT", [&tokens, &ip, &output](const std::string& modes, const std::vector<std::string>& params) -> void {
            int arg1 = OP_ARG(0);

            if (DEBUG) std::cout << ">>>> OUT | arg1 = " << arg1 << " (with mode " << modes[0] << ")" << (modes[0] != '1' ? " [" + params[0] + "]" : "") << std::endl;
            output = arg1;
            ip += 2;
        }},
        {"JIT", [&tokens, &ip](const std::string& modes, const std::vector<std::string>& params) -> void {
            int arg1 = OP_ARG(0);
            int arg2 = OP_ARG(1);

            if (DEBUG) std::cout << ">>>> JIT | arg1 = " << arg1 << " (with mode " << modes[0] << ")" << (modes[0] != '1' ? " [" + params[0] + "]" : "") << ", arg2 = " << arg2 << " (with mode " << modes[1] << ")" << (modes[1] != '1' ? " [" + params[1] + "]" : "") << ", output value = " << (arg1 == 0 ? "is zero" : "not zero") << ", ip = " << ip << std::endl;
            ip = arg1 != 0 ? arg2 : ip + 3;
        }},
        {"JIF", [&tokens, &ip](const std::string& modes, const std::vector<std::string>& params) -> void {
            int arg1 = OP_ARG(0);
            int arg2 = OP_ARG(1);

            if (DEBUG) std::cout << ">>>> JIF | arg1 = " << arg1 << " (with mode " << modes[0] << ")" << (modes[0] != '1' ? " [" + params[0] + "]" : "") << ", arg2 = " << arg2 << " (with mode " << modes[1] << ")" << (modes[1] != '1' ? " [" + params[1] + "]" : "") << ", output value = " << (arg1 == 0 ? "is zero" : "not zero") << ", ip = " << ip << std::endl;
            ip = arg1 == 0 ? arg2 : ip + 3;
        }},
        {"SLT", [&tokens, &ip](const std::string& modes, const std::vector<std::string>& params) -> void {
            int arg1 = OP_ARG(0);
            int arg2 = OP_ARG(1);
            int outp = std::stoi(params[2]);

            if (DEBUG) std::cout << ">>>> SLT | arg1 = " << arg1 << " (with mode " << modes[0] << ")" << (modes[0] != '1' ? " [" + params[0] + "]" : "") << ", arg2 = " << arg2 << " (with mode " << modes[1] << ")" << (modes[1] != '1' ? " [" + params[1] + "]" : "") << ", output position = " << outp << ", output value = " << (arg1 < arg2 ? "less" : "greater") << std::endl;
            tokens[outp] = arg1 < arg2 ? "1" : "0";
            ip += 4;
        }},
        {"SIE", [&tokens, &ip](const std::string& modes, const std::vector<std::string>& params) -> void {
            int arg1 = OP_ARG(0);
            int arg2 = OP_ARG(1);
            int outp = std::stoi(params[2]);

            if (DEBUG) std::cout << ">>>> SIE | arg1 = " << arg1 << " (with mode " << modes[0] << ")" << (modes[0] != '1' ? " [" + params[0] + "]" : "") << ", arg2 = " << arg2 << " (with mode " << modes[1] << ")" << (modes[1] != '1' ? " [" + params[1] + "]" : "") << ", output position = " << outp << ", output value = " << (arg1 == arg2 ? "equals" : "not equals") << std::endl;
            tokens[outp] = arg1 == arg2 ? "1" : "0";
            ip += 4;
        }},
    };

    const auto end_program = [&](bool end) -> std::string {
        code = join(tokens, ",");
        state->ip = ip;
        state->reached_end = end;
        return std::to_string(output);
    };

    try {
        for (; ip < tokens.size();) {
            // Parse instruction
            std::string instruction = tokens[ip];
            std::string opcode = instruction.length() > 2 ? instruction.substr(instruction.length() - 2) : instruction;
            std::string parameter_modes = instruction.length() > 2 ? instruction.substr(0, instruction.length() - 2) : "";
            std::reverse(parameter_modes.begin(), parameter_modes.end());

            int token = std::stoi(opcode.length() == 2 ? opcode.substr(1) : opcode);
            if (DEBUG) std::cout << ">>>> OPCODE: " << opcode << " | PARAM MODES: " << parameter_modes << " | IP: " << ip << std::endl;

            switch (token) {
                case 1: opcodes["ADD"](parameter_modes, { tokens[ip + 1], tokens[ip + 2], tokens[ip + 3] }); break;
                case 2: opcodes["MUL"](parameter_modes, { tokens[ip + 1], tokens[ip + 2], tokens[ip + 3] }); break;
                case 3: opcodes["INP"](parameter_modes, { tokens[ip + 1] });                                 break;
                case 4: opcodes["OUT"](parameter_modes, { tokens[ip + 1] });                                 break;
                case 5: opcodes["JIT"](parameter_modes, { tokens[ip + 1], tokens[ip + 2] });                 break;
                case 6: opcodes["JIF"](parameter_modes, { tokens[ip + 1], tokens[ip + 2] });                 break;
                case 7: opcodes["SLT"](parameter_modes, { tokens[ip + 1], tokens[ip + 2], tokens[ip + 3] }); break;
                case 8: opcodes["SIE"](parameter_modes, { tokens[ip + 1], tokens[ip + 2], tokens[ip + 3] }); break;
                case 9: return end_program(true);
            }
            
            if (token == 3 && state->got_settings && ip != state->ip) {
                // Pause this! Wait for an input
                return end_program(false);
            }

            if (SHOW_STATES) std::cout << std::endl << join(tokens, ",") << std::endl << std::endl;
        }

        std::cout << "Didn't find a 99 opcode. Returning from input end..." << std::endl;
        return end_program(true);
    } catch (const std::exception& e) {
        std::cerr << "ERROR! Failed to parse code: " << e.what() << std::endl;
        return {};
    }
}

int main() {
    const std::string program = "3,26,1001,26,-4,26,3,27,1002,27,2,27,1,27,26,27,4,27,1001,28,-1,28,1005,28,6,99,0,0,5";

    int max_output = 0;

    // { "0", "1", "2", "3", "4" } if you don't want feedback loop mode
    std::array<std::string, 5> phase_settings { "5", "6", "7", "8", "9" }, best_phase_settings{};
    std::array<ProgramState*, 5> program_states {
        new ProgramState(program),
        new ProgramState(program),
        new ProgramState(program),
        new ProgramState(program),
        new ProgramState(program)
    };

    do {
        std::string current_input = "0";
        
        while (true) {
            for (int i = 0; i < phase_settings.size(); ++i) {
                std::string phase = phase_settings[i];
                current_input = run_program(program_states[i]->code, phase, current_input, program_states[i]);
            }
            if (program_states[4]->reached_end) break;
        }

        int final_output = std::stoi(current_input);
        if (final_output > max_output) {
            max_output = final_output;
            best_phase_settings = phase_settings;
        }

        for (auto* s : program_states) s->reset(program);
    } while (std::next_permutation(phase_settings.begin(), phase_settings.end()));

    std::cout << "Max output: " << max_output << ", with phase settings: " << join(best_phase_settings, ",") << std::endl;

    for (auto* s : program_states) delete s;

    return 0;
}