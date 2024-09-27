# grep

## NAME

grep - print lines that match patterns

## SYNOPSIS

```
grep [-R] [-F] [-v] _pattern_ [file...]
```

## DESCRIPTION

`grep` searches for the specified pattern in each file. If no file is provided,
`grep` matches against standard input. Patterns are a limited regular
expression, using `^` for the beginning of a line, `$` for the end of a line,
`.` to denote any character, and `\*` for Kleene stars.

If `-R` or multiple files are specified, then the path name is printed in red
before the matching line.

## OPTIONS

`-R`	Read all files under each directory recursively.

`-F`	Use a fixed string as the pattern rather than a regular expression. Can
	yield slight performance boost.

`-v`	Invert the matching, printing only the lines that _don't_ match

Any other flag will fail.

## EXIT STATUS

`grep` exits with 0 if at least one line matched (or didn't with `-v`), 1 if no
lines match (or all with `-v`), and 2 if an error occured.

## BUGS/CONSIDERATIONS

* Flags _must_ be specified before the pattern, and must be specified
  individually.
* 2 is returned if _any_ error occurs, even if there is a match. Generally a
  problem when matching against multiple files, and one matches while another
  errors.
* Running `grep -R _pattern_` is **not** equivalent to `grep -R _pattern_ .`,
  instead being equivalent to `grep _pattern_`. This is a bug.
* Recursion is depth-first, and is ordered by inode creation time rather than
  lexically.
* Breaks up lines longer than 1024 characters. Shouldn't affect most people.

## EXAMPLES

`$ grep -R pattern dir`

matches `pattern` against all files in `dir` and its subdirectories (that can
be opened).

`$ grep -F F.g README.md`

matches README.md against the fixed string `F.g`. The pattern is not
interpreted as a regular expression, so `Fog` does not match.

`$ grep -v Fog README.md`

matches README.md and prints the lines that _don't_ match the pattern `Fog`.

`$ ls | grep README`

finds a file in the current directory whose name matches the pattern `README`
