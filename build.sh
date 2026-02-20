#!/bin/bash

cd "$(dirname -- "${BASH_SOURCE[0]}")"

set -e

templater() {
    echo "TEMPLATER: $1 -> $2"
    ./templater/templater $@
}

###########################
# === BUILD TEMPLATER === #
###########################

echo "Building templater..."

cd templater
./build.sh
cd ..

#########################
# === BUILD WEBSITE === #
#########################

echo "Building website..."
echo "(generated files will be at \`./public\`)"

mkdir -p public/pages
find pages -type d -exec mkdir -p public/{} \;

find pages -iname '*.css' -o -iname '*.js' | while read -r file; do
    cp $file public/$file
done

find . -not \( -path "./public" -prune \) -type f -iname "*.template.html" | while read -r file; do
    templater $file public/${file/.template/}
done

#########################
# === BUILD SITEMAP === #
#########################

echo "Building sitemap..."

cd public

echo '<?xml version="1.0" encoding="UTF-8"?>' > sitemap.xml
echo '<urlset xmlns="http://www.sitemaps.org/schemas/sitemap/0.9">' >> sitemap.xml
find -iname "*.html" | while read -r file; do
    echo "  <url>" >> sitemap.xml
    printf "    <loc>https://ammienyami.com/%s</loc>\n" "$(echo $file | sed -r 's|^./||g')" >> sitemap.xml
    echo "    <lastmod>$(git log -1 --format="%at" | xargs -I{} date -d @{} +%Y-%m-%d)</lastmod>" >> sitemap.xml
    echo "  </url>" >> sitemap.xml
done
echo "</urlset>" >> sitemap.xml

echo 'Sitemap: https://ammienyami.com/sitemap.xml' > robots.txt

cd ..
