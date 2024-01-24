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
    let content = document.getElementById("content");
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

{
    const params = new URLSearchParams(window.location.search);
    const pageParam = params.get("page");
    if (pageParam) {
        updatePage(decodeURI(pageParam));
    } else {
        updatePage(DEFAULT_PAGE);
    }
}
