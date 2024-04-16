---
title: 'AppDrawer'
date: 2024-01-09T12:00:00-03:00
---

[Project's Source Code](https://github.com/HatsuSixty/AppDrawer)

AppDrawer is a simple display server written from scratch in C++ with no dependencies (besides raylib for rendering). It manages windows by itself so it can also be considered a simple window manager.

# How it works

AppDrawer initially has 2 threads: a rendering thread and a server thread. The rendering thread renders the state of the server, and the server thread waits for connections. Each connection creates a separate thread for listening to commands from the client. The clients can send commands such as "get window display" or "create window". The so called "window display" is a shared memory region containing RGBA data that gets rendered into the actual window. To receive events, clients must tell the server to start sending events to a specific client. When the server starts sending events to a client, it creates a separate socket for that client, and sends events through it.

The definitions of the structures used to communicate with the server are defined in a C header called "RudeDrawer".

A library called LibDraw is provided as an abstraction layer on top of raw socket communication between client and server.

# Inspiration

I started this project planning to eventually build all of the other components that make a graphical operating system, effectively building a full operating system from scratch, with the only third-party component being the kernel (the Linux kernel).

This operating system would be inspired by the 90's/early 2000's productivity software user interfaces, like [SerenityOS](https://serenityos.org/) is.
