const DEFAULT_PAGE = "pages/home.html";
var currentPage = DEFAULT_PAGE;

function sharePage() {
    const url = window.location.origin;
    navigator.clipboard.writeText(url + '/?page=' + encodeURI(currentPage))
        .then(() => {
            alert('The generated link was copied to the clipboard!');
        });
}

function updatePage(pg) {
    const content = document.getElementById("content");
    content.innerHTML = "<h1>Loading...</h1>";
    fetch(pg).then(x => {
        x.text().then(x => {
            const sharePageLink =
                  `<a class="sharelink" href="javascript:sharePage()">
                     Share this page!
                   </a></br>`;
            content.innerHTML = sharePageLink + x + "</br></br>" + sharePageLink;
        });
    });
    currentPage = pg;
}

function setTheme(theme) {
    const style = document.documentElement.style;
    style.setProperty("--header-title-padding", "20px");
    style.setProperty("--content-padding", "20px");
    style.setProperty("--margin", "20px");

    const themes = {
        "normal": {
            "background-bright-color": "#933c73",
            "background-dark-color": "#7B3260",
            "content-border-color": "#2F1325",
            "content-background-color": "#B74B90",
            "content-foreground-color": "#FFFFFF",
            "link-background-color": "#ff69c9"
        },
        "simple": {
            "background-bright-color": "#d8d8d8",
            "background-dark-color": "#e7e7e7",
            "content-border-color": "#000000",
            "content-background-color": "#FFFFFF",
            "content-foreground-color": "#000000",
            "link-background-color": "#FFFF00"
        },
        "dark": {
            "background-bright-color": "#0b0b0b",
            "background-dark-color": "#000000",
            "content-border-color": "#FFFFFF",
            "content-background-color": "#000000",
            "content-foreground-color": "#FFFFFF",
            "link-background-color": "#000000"
        }
    };

    const currentTheme = themes[theme];
    for (const [key, value] of Object.entries(currentTheme)) {
        style.setProperty("--" + key, value);
    }

    localStorage.setItem("lastTheme", theme);
}

{
    setTheme(localStorage.getItem("lastTheme") || "normal");

    const params = new URLSearchParams(window.location.search);
    const pageParam = params.get("page");
    if (pageParam) {
        updatePage(decodeURI(pageParam));
    } else {
        updatePage(DEFAULT_PAGE);
    }
}
