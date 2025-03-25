# chttpd
This is a simple HTTP server implementation in C that supports static file serving and configurable routes (it was written in order to understand how web-server works). The server uses a hash map to store routes and supports both string-based and regex-based mappings

## Features
- Serves static files
- Configurable routing through a configuration file
- Supports both exact and regex-based URL mappings

## Installation
### Prerequisites
Ensure you have the following dependencies installed:
- GCC (or Clang)
- GNU Make

### Build
To compile the server, run:
```sh
make
```

This will generate an executable named `server`

## Running the Server
To start the server, use:
```sh
./server
```

By default, the server reads the configuration from `server.conf`. You can specify a custom configuration file:
```sh
./server /path/to/config.conf
```

## Configuration
The server uses a structured configuration file (`server.conf`) to define routing and server settings

### Example Configuration
```conf
http {
    port: 14880;
    web_root: ./www/;

    route {
        web_root: ./www/subfolder/;
        mapping: /200;
        file_path: 200.html;
    }

    route {
        mapping: /;
        file_path: index.html;
    }

    route {
        mapping[regex]: /help*;
        file_path: index.html;
    }
}
```

### Configuration Options
- `port`: Specifies the port the server will listen on
- `web_root`: The root directory for serving files
- `route`: Defines individual routes with the following properties:
  - `mapping`: URL path to match
  - `mapping[regex]`: Specifies a regex-based route
  - `file_path`: The file that will be served for this route
  - `web_root` (optional): Overrides the global `web_root` for this route

