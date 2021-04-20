# zcu-kiv-psi-2

This is the second KIV/PSI project. A very simple HTTP server that demonstrates one thread per client model.

## Implementation

The server is implemented in C and uses only the POSIX standard library.
It should work on any POSIX-compatible platform.
Windows operating system is not supported.

## Building

First of all, make sure that `gcc` compiler and `make` tool are installed.
On Debian-based systems, you only need to install the `build-essential` package.

Next, clone this repository to some directory and switch to it.
Then run the following command to compile the server:

```
make
```

You should now see the resulting `http_server` executable in the current directory.

## Usage

The server can be started as follows:

```
./http_server
```

To connect to the running server, either open <http://localhost:12345> in a web browser or use the following command:

```
curl http://localhost:12345
```

Here is the hard-coded web page that the server uses to respond to valid GET requests:

```html
<!DOCTYPE html>
<html lang="en">
 <head>
  <meta charset="UTF-8">
  <title>/</title>
 </head>
 <body>
  <h2>Hello [::1]:53812</h2>
 </body>
</html>
```

The server also logs each processed request to standard error output:

```
Listening on port 12345
[::1]:53808 GET /
[::1]:53810 GET /
[::1]:53812 GET /
[::ffff:127.0.0.1]:48646 GET /
[::ffff:127.0.0.1]:48648 GET /
[::1]:53818 GET /
[::1]:53820 GET /favicon.ico
[::1]:53822 GET /
[::1]:53824 GET /favicon.ico
[::1]:53826 GET /
[::1]:53828 GET /favicon.ico
```
