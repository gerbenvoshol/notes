# notes

A fast and minimal knowledge-management system written in C and rendered in HTML.

It was inspired by memex engine [Oscean](https://github.com/xxiivv/Oscean) and is powered by the markdown parser [MD4C](https://github.com/mity/md4c).

Features:
* Parsing of notes in markdown format.
* Tag-based note organization.
* Sort the note order however you like before running.
* Only notes marked public are also copied to a `remote` directory.
* Generates in less than a second.

[https://tagg.link/notes/remote/](https://tagg.link/notes/remote/)

Usage:
`notes my-note.md [my-second-note.md my-third-note.md ...]`

TODO:
* Add a build script written in bash
* Render HTML entities properly in markdown
* Style external links differently from local links
