const DEFAULT_THEME = "dark";
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
        "background-image": "url(/images/moriyashrine.png)",
        "opacity": "0.8",

        "link-color": "75, 229, 255",

        "top-bar-background-color": "51, 25, 43",
        "top-bar-foreground-color": "255, 208, 237",

        "bottom-bar-background-color": "51, 25, 43",
        "bottom-bar-foreground-color": "255, 208, 237",

        "page-content-background-color": "31, 15, 27",
        "page-content-foreground-color": "255, 208, 237",
    };

    const themes = {
        "dark": {},
    };

    if (!(theme in themes))
        theme = DEFAULT_THEME;

    const themeVariables = { ...defaults, ...themes[theme] };
    for (const [key, value] of Object.entries(themeVariables)) {
        style.setProperty(`--${key}`, value);
    }

    localStorage.setItem("lastTheme", theme);
}

function handlePathUpdate() {
    const query = new URLSearchParams(window.location.search);
    const page = decodeURIComponent(query.get("page") || encodeURIComponent(DEFAULT_PAGE));
    const raw = query.get("raw") === "true";

    const content = document.getElementById("page-content");
    content.innerHTML = "<h1>Loading...</h1>";
    fetch(page).then((response) => {
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
        });
    });
}

function goToPage(page) {
    if (page === "/" || page === "") {
        window.history.pushState({}, "", "/");
    } else {
        window.history.pushState({}, "", `/?page=${encodeURIComponent(page)}`);
    }
    handlePathUpdate();
}

function goToPageRaw(page) {
    if (page === "/" || page === "") {
        window.history.pushState({}, "", "/");
    } else {
        window.history.pushState({}, "", `/?page=${encodeURIComponent(page)}&raw=true`);
    }
    handlePathUpdate();
}

(() => {
    setTheme(localStorage.getItem("lastTheme") || DEFAULT_THEME);
    window.addEventListener("popstate", handlePathUpdate);
    handlePathUpdate();
})();
