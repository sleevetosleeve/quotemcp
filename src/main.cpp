#include <iostream>
#include <random>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace {

constexpr const char* kProtocolVersion = "2025-06-18";
constexpr const char* kServerName = "quotemcp";
constexpr const char* kServerVersion = "0.1.0";

const std::vector<std::string> kQuotes = {
    "The only way to do great work is to love what you do. - Steve Jobs",
    "It always seems impossible until it's done. - Nelson Mandela",
    "Don't watch the clock; do what it does. Keep going. - Sam Levenson",
    "The future belongs to those who believe in the beauty of their dreams. - Eleanor Roosevelt",
    "Well done is better than well said. - Benjamin Franklin",
    "Start where you are. Use what you have. Do what you can. - Arthur Ashe",
    "Everything you've ever wanted is on the other side of fear. - George Addair",
    "What lies behind us and what lies before us are tiny matters compared to what lies within us. - Ralph Waldo Emerson",
    "One child, one teacher, one book, one pen can change the world. - Malala Yousafzai",
    "We realize the importance of our voices only when we are silenced. - Malala Yousafzai",
    "The rights of democracy are not reserved for a select group within society; they are the rights of all the people. - Olof Palme",
    "For us, democracy is a question of human dignity. - Olof Palme",
    "It's not the daily increase but daily decrease. Hack away at the unessential. - Bruce Lee",
    "Do not pray for an easy life, pray for the strength to endure a difficult one. - Bruce Lee",
    "What you seek is seeking you. - Rumi",
    "The wound is the place where the Light enters you. - Rumi",
};

std::mt19937& rng() {
    static std::mt19937 engine{std::random_device{}()};
    return engine;
}

void send(const json& message) {
    std::cout << message.dump() << '\n';
    std::cout.flush();
}

void sendResult(const json& id, const json& result) {
    send(json{{"jsonrpc", "2.0"}, {"id", id}, {"result", result}});
}

void sendError(const json& id, int code, const std::string& message) {
    send(json{{"jsonrpc", "2.0"},
              {"id", id},
              {"error", {{"code", code}, {"message", message}}}});
}

std::string stringField(const json& object, const char* key) {
    if (object.contains(key) && object[key].is_string()) {
        return object[key].get<std::string>();
    }
    return "";
}

const json& paramsOf(const json& message) {
    static const json kEmptyParams = json::object();
    if (message.contains("params") && message["params"].is_object()) {
        return message["params"];
    }
    return kEmptyParams;
}

json textContent(const std::string& text) {
    return {{"type", "text"}, {"text", text}};
}

json handleInitialize(const json& params) {
    std::string version = kProtocolVersion;
    const std::string requested = stringField(params, "protocolVersion");
    if (!requested.empty()) {
        version = requested;
    }
    return {
        {"protocolVersion", version},
        {"capabilities", {{"tools", {{"listChanged", false}}}}},
        {"serverInfo", {{"name", kServerName}, {"version", kServerVersion}}},
    };
}

json toolList() {
    return json::array({
        {
            {"name", "random_quote"},
            {"description", "Returns a random inspirational quote from the built-in list."},
            {"inputSchema", {
                {"type", "object"},
                {"properties", json::object()},
                {"required", json::array()},
            }},
        },
    });
}

json callRandomQuote() {
    std::uniform_int_distribution<std::size_t> pick(0, kQuotes.size() - 1);
    return {
        {"content", json::array({textContent(kQuotes[pick(rng())])})},
        {"isError", false},
    };
}

json handleToolsCall(const json& params) {
    const std::string name = stringField(params, "name");
    if (name == "random_quote") {
        return callRandomQuote();
    }
    return {
        {"content", json::array({textContent("Unknown tool: " + name)})},
        {"isError", true},
    };
}

void handleMessage(const json& message) {
    const bool isRequest = message.contains("id");
    const std::string method = stringField(message, "method");
    const json& params = paramsOf(message);

    if (method == "initialize") {
        if (isRequest) {
            sendResult(message["id"], handleInitialize(params));
        }
    } else if (method == "notifications/initialized") {
    } else if (method == "ping") {
        if (isRequest) {
            sendResult(message["id"], json::object());
        }
    } else if (method == "tools/list") {
        if (isRequest) {
            sendResult(message["id"], {{"tools", toolList()}});
        }
    } else if (method == "tools/call") {
        if (isRequest) {
            sendResult(message["id"], handleToolsCall(params));
        }
    } else if (isRequest) {
        sendError(message["id"], -32601, "Method not found: " + method);
    }
}

}  // namespace

int main() {
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            continue;
        }
        json message;
        try {
            message = json::parse(line);
        } catch (const json::parse_error& error) {
            std::cerr << "invalid JSON-RPC line: " << error.what() << '\n';
            sendError(nullptr, -32700, "Parse error");
            continue;
        }
        if (!message.is_object()) {
            sendError(nullptr, -32600, "Invalid Request");
            continue;
        }
        handleMessage(message);
    }
    return 0;
}
