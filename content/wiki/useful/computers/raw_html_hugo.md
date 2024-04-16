---
title: 'Using raw html in markdown files with Hugo'
date: 2024-03-15T12:00:00-03:00
---

In this page I will be showing how to use raw html in markdown files with Hugo.  

# How to

Create a file called `rawhtml.html` inside your `layouts/shortcodes/` folder. The file should contain the following code inside it:
```html
{{ .Inner }}
```

With that file created, you can now use raw html in markdown, like the following:
```markdown
# Cool title

Cool **text**.

{{</* rawhtml */>}}
  <p>This is raw html!</p>
{{</* /rawhtml */>}}
```
