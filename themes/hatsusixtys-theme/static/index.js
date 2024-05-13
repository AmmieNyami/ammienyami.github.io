function setTheme(theme) {
    const style = document.documentElement.style;

    const defaults = {
        "header-link-color": "#B74B90",
        "header-foreground-color": "#FFFFFF",
        "header-background-color": "#B74B90",
        "header-border-color": "#FFBEE7",
        "accent-color": "#B74B90",
        "accent-alt-color": "#FFBEE7",
    };

    const themes = {
        "modern": {
            "code-font": "\"Lucida Console\", Monaco, monospace",
            "normal-font": "Optima, Candara, Calibri, Arial, sans-serif",

            "background-image": "none lmao",
            "content-background-color": "#FFFFFF",
            "content-border-color": "#FFFFFF",
            "hr-color": "#ddd",
        },
        "minimal": {
            "header-background-color": "#FFFFFF",
            "header-border-color": "#FF69C9",
            "header-foreground-color": "#FF69C9",

            "code-font": "\"Lucida Console\", Monaco, monospace",
            "normal-font": "Optima, Candara, Calibri, Arial, sans-serif",

            "background-image": "none lmao",
            "content-background-color": "#FFFFFF",
            "content-border-color": "#FFFFFF",
            "hr-color": "#ddd",
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

setTheme(localStorage.getItem("lastTheme") || "modern");
