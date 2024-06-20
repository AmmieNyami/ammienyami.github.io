const default_theme = "dark";

function setTheme(theme) {
    const style = document.documentElement.style;

    const defaults = {
        "rly-hard-to-ignore": "red",

        "content-background-color": "#FFFFFF",
        "content-border-color": "#FFFFFF",

        "foreground-color": "#000000",

        "background-color-1": "#FFFFFF",
        "background-color-2": "#DDDDDD",

        "header-link-color": "#B74B90",
        "header-foreground-color": "#FFFFFF",
        "header-background-color": "#B74B90",
        "header-border-color": "#FFBEE7",

        "title-accent-color": "#B74B90",

        "accent-gradient-from": "#FFFFFF",
        "accent-gradient-to": "#FFBEE7",

        "code-highlight-color": "#eee",
        "hr-color": "#ddd",
        "link-color": "blue",

        "code-font": "\"Lucida Console\", Monaco, monospace",
        "normal-font": "Optima, Candara, Calibri, Arial, sans-serif",
    };

    const themes = {
        "modern": {},
        "minimal": {
            "header-background-color": "#FFF9FD",
            "header-border-color": "#FF69C9",
            "header-foreground-color": "#B74B90",
        },
        "dark": {
            "rly-hard-to-ignore": "lawngreen",

            "content-background-color": "#3C1830",
            "content-border-color": "#3C1830",

            "foreground-color": "#FFFFFF",
            "background-color-1": "#3C1830",
            "background-color-2": "#702c58",

            "header-link-color": "#FFD0ED",
            "header-background-color": "#62274E",
            "header-border-color": "#FFD0ED",
            "header-foreground-color": "#FFD0ED",

            "accent-gradient-from": "#702c58",
            "accent-gradient-to": "#B74B90",

            "code-highlight-color": "#62274E",
            "link-color": "#FFFF20",
        },
    };

    if (!(theme in themes)) {
        theme = default_theme;
    }

    const currentTheme = themes[theme];
    for (const [key, value] of Object.entries(currentTheme)) {
        style.setProperty("--" + key, value);
    }

    for (const [key, value] of Object.entries(defaults)) {
        if (!(key in currentTheme)) {
            style.setProperty("--" + key, value);
        }
    }

    localStorage.setItem("lastTheme", theme);
}

setTheme(localStorage.getItem("lastTheme") || default_theme);
