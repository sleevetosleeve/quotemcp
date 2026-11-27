# quotemcp

A hand-rolled [Model Context Protocol](https://modelcontextprotocol.io) (MCP) server in C++17.
No MCP SDK: it speaks JSON-RPC 2.0 directly over the stdio transport (newline-delimited JSON
on stdin/stdout), using [nlohmann/json](https://github.com/nlohmann/json) for serialization.
Built as a learning proof of concept.

## Tools

- `random_quote` — returns a random inspirational quote from a built-in list.

Planned next steps: a `quote_id` that hands out each quote at most once per ID, returning an
empty result when the ID's quotes are exhausted.

## Protocol surface (stdio)

The server reads one JSON-RPC 2.0 message per line from stdin and writes one response per
line to stdout. Diagnostics go to stderr — stdout is reserved for the protocol stream.

Handled methods:

| Method | Kind | Behavior |
| --- | --- | --- |
| `initialize` | request | replies with `protocolVersion`, `capabilities.tools`, `serverInfo` |
| `notifications/initialized` | notification | ignored (no response) |
| `ping` | request | replies with an empty result |
| `tools/list` | request | replies with the tool definitions and input schemas |
| `tools/call` | request | dispatches to a tool handler, e.g. `random_quote` |
| anything else with an `id` | request | JSON-RPC error `-32601` (method not found) |

Malformed JSON gets a JSON-RPC error `-32700` (parse error) with a `null` id.

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

`nlohmann/json` is used via `find_package` when installed, otherwise fetched with CMake
FetchContent (pinned to `v3.11.3`).

## Run

The binary is a stdio server: run it and pipe JSON-RPC lines into it.

Try it with the official MCP Inspector:

```sh
npx @modelcontextprotocol/inspector ./build/quote_mcp
```

Or register it in a host (Claude Desktop, Cursor, ...):

```json
{
  "mcpServers": {
    "quotemcp": {
      "command": "/absolute/path/to/quotemcp/build/quote_mcp"
    }
  }
}
```

Manual smoke test:

```sh
printf '%s\n' \
  '{"jsonrpc":"2.0","id":1,"method":"initialize","params":{"protocolVersion":"2025-06-18","capabilities":{},"clientInfo":{"name":"smoke","version":"0.0.1"}}}' \
  '{"jsonrpc":"2.0","method":"notifications/initialized"}' \
  '{"jsonrpc":"2.0","id":2,"method":"tools/list"}' \
  '{"jsonrpc":"2.0","id":3,"method":"tools/call","params":{"name":"random_quote","arguments":{}}}' \
  | ./build/quote_mcp
```
