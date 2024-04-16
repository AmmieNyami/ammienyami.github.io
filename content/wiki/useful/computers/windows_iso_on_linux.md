---
title: 'Creating a Windows ISO out of a folder in Linux'
date: 2024-02-13T12:00:00-03:00
---

In this page I will be showing how to create a Windows ISO out of a folder in Linux. **For this, you will need to have genisoimage installed.**

# How to

In orther to generate a Windows ISO, you will need to run the following command:

```console
$ genisoimage -b <boot image> -no-emul-boot -boot-load-size 8 -iso-level 2 -udf -joliet -D -N -relaxed-filenames -o <output iso> <folder containing the windows files>
```

- &lt;boot image&gt; - Boot image. Generally is `boot/etfsboot.com`.
- &lt;output iso&gt; - The name you want the generated ISO to have.
- &lt;folder containing the windows files&gt; - Folder containing the ISO files.

I tested it with a Windows 8.1 ISO.
