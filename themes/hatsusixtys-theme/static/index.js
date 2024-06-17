const default_theme = "dark";

function setTheme(theme) {
    const style = document.documentElement.style;

    const defaults = {
        "foreground-color": "#000000",
        "background-color": "#FFFFFF",
        "header-link-color": "#B74B90",
        "header-foreground-color": "#FFFFFF",
        "header-background-color": "#B74B90",
        "header-border-color": "#FFBEE7",

        "accent-color": "#B74B90",
        "accent-alt-color": "#FFBEE7",

        "hr-color": "#ddd",
        "link-color": "blue",
        "background-image": "none lmao",
    };

    const themes = {
        "modern": {
            "code-font": "\"Lucida Console\", Monaco, monospace",
            "normal-font": "Optima, Candara, Calibri, Arial, sans-serif",

            "content-background-color": "#FFFFFF",
            "content-border-color": "#FFFFFF",
        },
        "minimal": {
            "header-background-color": "#FFF9FD",
            "header-border-color": "#FF69C9",
            "header-foreground-color": "#B74B90",

            "code-font": "\"Lucida Console\", Monaco, monospace",
            "normal-font": "Optima, Candara, Calibri, Arial, sans-serif",

            "content-background-color": "#FFFFFF",
            "content-border-color": "#FFFFFF",
        },
        "dark": {
            "header-background-color": "#62274e",
            "header-border-color": "#FFD0ED",
            "header-foreground-color": "#FFD0ED",

            "code-font": "\"Lucida Console\", Monaco, monospace",
            "normal-font": "Optima, Candara, Calibri, Arial, sans-serif",

            "accent-alt-color": "#ff82d5",

            "background-color": "#3C1830",
            "content-background-color": "#3C1830",
            "content-border-color": "#3C1830",
            "foreground-color": "#FFFFFF",
            "link-color": "#FFFF20",
        },
        "cool": {
            "code-font": "\"Lucida Console\", Monaco, monospace",
            "normal-font": "monospace",

            "background-image": "url(/cool-background.png)",
            "content-background-color": "#D8D8D8",
            "content-border-color": "#B74B90",
            "hr-color": "#8f8f8f",
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
