const DEFAULT_THEME = "light";
const DEFAULT_PAGE = "/pages/home.html";

function escapeHtml(html){
    var text = document.createTextNode(html);
    var p = document.createElement('p');
    p.appendChild(text);
    return p.innerHTML;
}

function setTheme(theme) {
    const style = document.documentElement.style;

    const defaults = {
        "bar-border-width": "1px",
        "page-border-width": "2px",
        "background-image": "url(/images/cherryblossom.png)",
        "opacity": "0.9",

        "top-bar-background-color": "44, 22, 64",
        "top-bar-foreground-color": "255, 208, 237",

        "bottom-bar-background-color": "44, 22, 64",
        "bottom-bar-foreground-color": "255, 208, 237",
        "bottom-bar-link-color": "75, 229, 255",

        "page-content-background-color": "23, 10, 51",
        "page-content-foreground-color": "255, 208, 237",
        "page-content-link-color": "75, 229, 255",
    };

    const themes = {
        "dark": {
            "opacity": "0.96",
        },
        "light": {
            "top-bar-background-color": "104, 83, 175",
            "top-bar-foreground-color": "255, 240, 249",

            "bottom-bar-background-color": "104, 83, 175",
            "bottom-bar-foreground-color": "255, 240, 249",
            "bottom-bar-link-color": "215, 194, 255",

            "page-content-background-color": "255, 240, 249",
            "page-content-foreground-color": "51, 25, 43",
            "page-content-link-color": "119, 90, 218",
        },
    };

    if (!(theme in themes))
        theme = DEFAULT_THEME;

    const themeVariables = { ...defaults, ...themes[theme] };
    for (const [key, value] of Object.entries(themeVariables)) {
        style.setProperty(`--${key}`, value);
    }

    localStorage.setItem("lastTheme", theme);
}

function setupLinksForDOM(dom) {
    const links = dom.querySelectorAll("a");
    links.forEach((link) => {
        link.addEventListener("click", (event) => {
            const linkUrl = new URL(link.href);

            // Check if it's a link to the current website
            if (linkUrl.host === window.location.host
                && (linkUrl.pathname === "/" || linkUrl.pathname.startsWith("/!/")))
            {
                event.preventDefault();
                const updatedUrl = linkUrl.pathname + linkUrl.search + linkUrl.hash;
                window.history.pushState({}, "", updatedUrl);
                handlePathUpdate();
            }
        });
    });
}

function handlePathUpdate() {
    const page = window.location.pathname === "/"
          ? DEFAULT_PAGE
          : window.location.pathname.substring(2);
    const raw = new URLSearchParams(window.location.search).get("raw") === "true";

    const content = document.getElementById("page-content");
    content.innerHTML = "<h1>Loading...</h1>";
    fetch(page).then((response) => {
        if (response.status === 404) {
            content.innerHTML = `<h1>404 - File not found</h1><p>It seems like the page you're looking for doesn't exist.</p>`;
            return;
        }

        response.text().then((htmlText) => {
            if (raw) {
                content.innerHTML = `<pre style="white-space:pre-wrap;">${escapeHtml(htmlText)}</pre>`;
                return;
            }

            content.innerHTML = htmlText;

            const htmlDOM = document.createElement("div");
            htmlDOM.innerHTML = htmlText;

            const metaScript = htmlDOM.querySelector('meta[name="script"]');
            if (metaScript) {
                const scriptPath = metaScript.getAttribute("content");
                const scriptElement = document.createElement("script");
                scriptElement.src = scriptPath;
                content.appendChild(scriptElement);
            }

            const metaDate = htmlDOM.querySelector('meta[name="date"]');
            if (metaDate) {
                const date = new Date(metaDate.getAttribute("content"));
                const dateElement = document.createElement("div");
                dateElement.innerHTML = `<p>Page created at ${date.toDateString()}</p><hr>`;
                content.insertBefore(dateElement, content.firstChild);
            }

            const metaCss = htmlDOM.querySelector('meta[name="css"]');
            if (metaCss) {
                const cssPath = metaCss.getAttribute("content");
                const cssElement = document.createElement("link");
                cssElement.rel = "stylesheet";
                cssElement.href = cssPath;
                content.appendChild(cssElement);
            }

            setupLinksForDOM(content);
        });
    });
}

(() => {
    setTheme(localStorage.getItem("lastTheme") || DEFAULT_THEME);
    setupLinksForDOM(document);
    window.addEventListener("popstate", handlePathUpdate);
    handlePathUpdate();
})();
