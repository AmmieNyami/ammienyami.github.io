---
title: 'Programming Languages'
date: 2024-01-09T12:00:00-03:00
---
This page is dedicated to some simple programming languages I made.

# Nolang
[Project's Source Code](https://github.com/HatsuSixty/nolang)

This was one of the first, if not the first, programming languages I made. So if for some reason you decide to read the source code for this language, please don't judge much.

It is a simple stack-based programming language I made inspired by [Tsoding](https://www.youtube.com/@TsodingDaily)'s [Porth Series](https://www.youtube.com/playlist?list=PLpM-Dvs8t0VbMZA7wW9aR3EtBqe2kinu4).

The compiler generates assembly and then compiles with an assembler, generating the final executable.

# Loisp
[Project's Source Code](https://github.com/HatsuSixty/loisp)

This is a Lisp-like (S-expression based) programming language I made also inspired by a video [Tsoding](https://www.youtube.com/@TsodingDaily) made. The original video was deleted for some reason.

The compiler is written in Rust and, like the previous one, compiles to assembly. It also has an interpreter and a REPL.

# North
[Project's Source Code](https://github.com/HatsuSixty/north)

This is the latest one I made. It's written in Python (yes, [this Python](https://www.python.org/)) and is almost a [Porth](https://gitlab.com/tsoding/porth) clone. This one can compile to assembly and C.

The C backend is because I wanted this language to be able to call to C functions, but I didn't know that the main stack was also the call stack, so I had some problems while trying to get assembly to call to libc functions.
