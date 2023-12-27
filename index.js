var page = "pages/home.html";

function updatePage(pg) {
    fetch(pg).then(x => {
        let content = document.getElementById("content");
        x.text().then(x => {
            content.innerHTML = x;
        });
    });
}

updatePage(page);
