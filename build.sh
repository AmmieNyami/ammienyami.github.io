#!/bin/sh

function pushd() {
    builtin pushd $1 2>&1 > /dev/null
}

function popd() {
    builtin popd $1 2>&1 > /dev/null
}

function templater() {
    echo "TEMPLATER: $1 -> $2"
    ./templater/templater $@
}

set -e

echo "Building templater..."

pushd templater
./build.sh
popd

echo "Building website..."
echo "(generated files will be at \`./public\`)"

mkdir -p public/pages
find pages -type d -exec mkdir -p public/{} \;

find pages -iregex '.*\.\(css\|js\)' | while read -r file; do
    cp $file public/$file
done

find . -not \( -path "./public" -prune \) -type f -iname "*.template.html" | while read -r file; do
    templater $file public/${file/.template/}
done

echo "Building sitemap..."

pushd public

echo '<?xml version="1.0" encoding="UTF-8"?>' > sitemap.xml
echo '<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">' >> sitemap.xml
find -iname "*.html" | while read -r file; do
    echo "  <url>" >> sitemap.xml
    printf "    <loc>https://hatsusixty.in/%s</loc>\n" "$(echo $file | sed -r 's|^./||g')" >> sitemap.xml
    echo "    <lastmod>$(date +%Y-%m-%d)</lastmod>" >> sitemap.xml
    echo "  </url>" >> sitemap.xml
done
echo "</urlset>" >> sitemap.xml

echo 'Sitemap: https://hatsusixty.in/sitemap.xml' > robots.txt

popd
