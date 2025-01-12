#!/bin/sh

if [ -f sitemap.txt ]; then
    rm sitemap.txt
fi

echo "https://hatsusixty.in/" >> sitemap.txt
find pages/ \
     -iname "*.html" \
     \! -path pages/home.html \
     -exec printf "https://hatsusixty.in/!/%s\n" {} + >> sitemap.txt
