# OVERVIEW OF COVERS DIRECTORY

A Strategy contains a trick number for each distribution of the 
opposing cards.  In this directory we attempt to describe each
strategy verbally in a way that makes sense to human players.

Sometimes the strategy has a single description such as
"West has the top".  Quite often several partial explanations are
needed to describe the result, such as:

>blockquote
Always take at least 0 tricks, and more when
* Either opponent has exactly one top; or
  Each opponent has 2-4 cards
* The suit splits 3=3

We call this description a *tableau*.  It consists in general of
a number of cover rows (two in this example).  Each row may consist
of one or more covers that are **OR**-ed together, so if any of 
the conditions apply, the row applies (but only once, even if
multiple covers apply).  The tableau is then the sum of the rows.

Each element of a row is called a *cover*.

## HIGHEST LEVEL

`Covers` contains a number of `Cover`'s in `CoverStore`.
There is one `Covers` per Distribution, so multiple combinations
in general share the same `Covers`.

The covers are generated algorithmically, although there is also
a manual way that applies specifically when only the highest of
the opponents' tops is relevant to the description.  These
manual covers are set in `Manual` and they are stored directly
in a list of `CoverRow` entries in `Covers`.

The `Covers` class can explain a trick vector either manually or
algorithmically.  In both cases an exhaustive but orderly search
is conducted to find the "simplest" description of the trick vector.

Simplicity is related to `Complexity`.

## THE COVER

A `Distribution` is described by (among other things) a sum profile
which is stored in a `Profile`.  If the opponents have a total number
of 5 cards, of which 3 are of the highest kind and 2 of the next
kind, then the sum profile is "5: 3, 2".

A `Cover` is related to of a set of `Term`'s, where a term applies either
to the overall length, or to a specific top rank.  The term may
be of the form "==", ">=", "<=" or "within a range".  The terms
are summarized in a lower profile and and upper profile which are
stored in a `ProfilePair`.

A set of terms is in the simplest sense independent of an actual
sum profile, so a term may be of the form "1-2" independently of
the upper limit for that term.  A set of such terms are combined
into a `Product`.

It is only when a sum profile is added that a product effectively
becomes a cover.  Unlike a product, a cover can be graded into a
trick vector that it would contribute to a tableau if it were
included.

A product may not use the lowest ranks available to it.  In this
case the product is in a sense not canonical, and it can be written
as a simpler product and a shift (a number of terms that are
ignored).  Such a representation is called a `FactoredProduct`.
