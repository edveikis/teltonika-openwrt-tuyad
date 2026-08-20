<div align="center">
  <img height=300" alt="images" src="https://github.com/user-attachments/assets/e0f0de23-0e96-414a-972c-9775f839bc40" />
</div>

# 📡 Tuyad OpenWrt Package

OpenWrt/RutOS package feed that ports the Tuya IoT daemon to routers, reporting system stats to the Tuya cloud using native ubus/uci APIs instead of raw file reads.

## 📝 Overview

This repository packages the Tuya IoT daemon for the OpenWrt/RutOS build system. Unlike the standalone Linux version, this daemon pulls router data through `ubus` (system info, network interfaces) and is configured through `uci` instead of `/proc` files and CLI-only arguments. It ships as four separate OpenWrt packages plus a VuCI web API integration for remote control.

## 📦 Packages

- 🧩 **`tuyad`** - the daemon itself, built from the `src/` sources in this repo
- 🔐 **`libtuyacore`** - the Tuya IoT Core SDK, fetched from its upstream git repo and patched to build shared libraries instead of static ones
- 🧮 **`libcjson`** - the cJSON library, fetched from its upstream git repo
- 🌐 **`vuci-app-tuyad`** - a VuCI backend API package exposing `tuyad` configuration over `/tuyad/*` REST endpoints (no front-end)

## ✨ Features

- 🖥️ CPU usage sampling from `/proc/stat`
- 🧠 Total/free RAM and uptime pulled via the `system` ubus object
- 🌐 Per-interface IP, netmask, RX/TX bytes pulled via the `network.interface.*` and `network.device` ubus objects, loopback excluded
- ⚙️ Configuration read from UCI (`/etc/config/tuyad`), with CLI arguments as an override
- 🔁 `send_now` UCI flag to trigger an immediate report on demand, auto-resets after sending
- 📝 Remote `save_log` action that writes a text parameter to `/tmp/tuya_action.log`
- 🧵 Runs under `procd` as a proper init.d service, with automatic respawn
- 🪵 All messages logged to the system log, viewable with `logread`
- 🌐 Minimal REST API (via VuCI/rpcd) to read and update the daemon's configuration
- 🛑 Graceful shutdown on SIGINT, SIGTERM, SIGHUP, SIGQUIT

## ⚙️ How It Works

On startup, `tuyad` parses any CLI arguments given to it. If none were passed, it loads its device ID, device secret, product ID, interval, and `send_now` flag from the `tuyad` UCI config instead. It connects to `ubus` and to the Tuya MQTT endpoint, then loops for as long as it's running: on each interval (or immediately if `send_now` is set) it collects CPU usage, memory, uptime, and per-interface network stats into a single JSON payload and reports it to Tuya as a thing property. It also listens for a `save_log` action from the cloud, appending the given text to `/tmp/tuya_action.log`. The VuCI app package lets you read and update this same UCI configuration remotely through the router's API.

## 🛠️ Building

These are OpenWrt buildroot packages, meant to be dropped into an OpenWrt/RutOS source tree's `package/` directory (or added as a feed) and built with the standard OpenWrt build flow:

```sh
./scripts/feeds update -a
./scripts/feeds install -a
make menuconfig   # enable tuyad, libtuyacore, libcjson, vuci-app-tuyad
make package/tuyad/compile V=s
```

`libtuyacore` and `libcjson` source code is downloaded automatically from their upstream git repositories at build time, no vendored SDK code lives in this repo.

## 📁 Package Layout

- `package/tuyad/` - daemon source, init script, and default UCI config
- `package/libtuyacore/` - Makefile plus a patch that switches the SDK's CMake targets from `STATIC` to `SHARED` libraries
- `package/libcjson/` - Makefile fetching and building cJSON as a shared library
- `package/feeds/vuci/vuci-app-tuyad/` - VuCI Lua API service definition, ACL rules, and API path mapping for `tuyad`

## ▶️ Usage

Once installed, enable and configure the daemon through UCI:

```sh
uci set tuyad.main.enabled='1'
uci set tuyad.main.dev_id='<DEV_ID>'
uci set tuyad.main.dev_secret='<DEV_SECRET>'
uci set tuyad.main.product_id='<PRODUCT_ID>'
uci set tuyad.main.interval='120'
uci commit tuyad

/etc/init.d/tuyad restart
```

To force an immediate report:

```sh
uci set tuyad.main.send_now='1'
uci commit tuyad
```

It also still accepts CLI flags directly, which take priority over the UCI config when provided:

```
-d, --dev-id=DEV_ID          Device ID
-s, --dev-secret=DEV_SECRET  Device secret
-p, --product-id=PRODUCT_ID  Product ID
-D, --daemon                 Run as daemon in background
-i, --interval=INTERVAL      Set interval in seconds data will be sent to server in
-h, --help                   Give this help list
-V, --version                Print program version
```

## 📌 Notes

- Data is reported to Tuya's `m1.tuyacn.com` MQTT endpoint over TLS.
- Network interfaces are discovered from the `network` UCI config, the `loopback` interface is always skipped.
- All runtime events and errors are logged via syslog under the `tuyad` identity, view them with `logread`.
- The program exits cleanly on Ctrl+C, SIGTERM, SIGHUP, or SIGQUIT, disconnecting from Tuya and freeing ubus/uci resources before quitting.

---

## 📋 Task

Connecting the router to the Tuya IoT cloud

This task is intended to help consolidate your acquired theoretical knowledge in practice. It will also help you better to understand the structure of the OpenWRT system, create new packages for it and apply new software.

For this task you will need to change your Tuya IoT daemon program so that it would work on the RutOS system.

---

Task result:

    A daemon type program has been prepared that will send data to the Tuya IoT cloud.
    Founded library or SDK that will be used to communicate with the Tuya IoT cloud. The library/SDK is prepared as a separate package.

---

A total of two different packages must be prepared:

    A daemon type program is responsible for communicating with Tuya IoT cloud
    A library/SDK which will be used to communicate and send data to Tuya IoT cloud

---

Requirements for the task:

    The library/SDK code must be automatically downloaded from the remote server where the library/SDK was found. 
    In the OpenWRT system, the package directory must not contain this library/SDK code. 
    Code is not considered a Makefile, shell and lua scripts, patches or configuration files.
    The ubus system must be used in the daemon program to obtain data about the router. 
    You should gather and send the same data as in the previous task. If some data is missing or unable to get it from the ubus, just leave as it is. 
    The daemon program must write messages to the log system, whose messages can be viewed by executing the logread command in the router's command line. The messages should be clear and informative, allowing the user to understand about the successful and erroneous operation of the program.

    A minimal API endpoint to control your program must be implemented. Don't implement front-end with JavaScript.
    In directory vuci-examples (which is located in the root directory of the OpenWRT project) provides an example of the structure and some code examples.
    More detailed documentation on how to create an API can be found here.
    Here you can find information how you can test the API using Postman.
    If you have any questions about the implementation, just ask your internship manager/coordinator.
