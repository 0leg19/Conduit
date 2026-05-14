Conduit is a minimal framework for remote browser UI control.
A C++ server injects the Conduit bridge into any HTML page at startup.
From that point, a single POST request drives live updates in the browser -
no polling, no page reloads, just WebSocket-backed event routing.
