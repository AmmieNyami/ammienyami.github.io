const DEFAULT_PAGE = "pages/home.html";

function updatePage(pg) {
    let content = document.getElementById("content");
    content.innerHTML = "<h1>Loading...</h1>";
    fetch(pg).then(x => {
        x.text().then(x => {
            content.innerHTML = x;
        });
    });
}

updatePage(DEFAULT_PAGE);
