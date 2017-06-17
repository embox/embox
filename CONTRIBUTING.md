Contributing to Embox
=====================

Thank you for taking the time to make Embox better! This makes you awesome by
default, but there are some rules to adhere, and following this guide will help
you make the whole thing about 20% cooler.

This is just an excerpt to check with from time to time, and it is mostly based
on the **[Git workflow](https://github.com/embox/embox/wiki/Contributing:-Git-workflow)**
wiki article. There are also the **[Git cheatsheet](https://github.com/embox/embox/wiki/Contributing:-Git-cheatsheet)**
article on fixing some common problems and rewriting the history.

Experienced Git users will likely be happy with just this guide though.


Git workflow
------------
*([More details](https://github.com/embox/embox/wiki/Contributing:-Git-workflow)
on our Wiki)*

We use [GitHub flow](https://guides.github.com/introduction/flow/) for the
development, which is basically about committing into branches and merging
through pull requests. **TL; DR**:

  - Git setup: real name and email; Editors/IDE: proper indentation, auto strip
    [trailing whitespaces](http://codeimpossible.com/2012/04/02/Trailing-whitespace-is-evil-Don-t-commit-evil-into-your-repo-/)

  - **Never push to `master`**; start each new feature in a branch:
    - Branch naming: *`short-name-through-hyphens`*; use *`NNNN-bug-name`*
      to refer an issue
    - Within a branch, please keep the history linear, i.e. `rebase` instead
      of `merge` to keep the branch up-to-date

  - [Atomic](http://www.freshconsulting.com/atomic-commits/) commits;
    no "oops" commits, [squash](#squash-commits-into-a-single-one) if needed;
    **[Git commit message agreements](https://github.com/embox/embox/wiki/Contributing:-Git-commit-message-agreements)**

  - Respect those who will review your changes: prepare your branch **before**
    opening a PR, cleanup commits and rebase to catch-up recent changes
    from `upstream/master`

  - Do not rebase already published changes and force-push **until review is
    over**: push `fixup!` commits, then rebase one last time once getting LGTM
    from maintainers


Commits
-------

For each commit, besides the above, please also be sure that:
  - [x] You used your real name and email to commit
  - [x] The commit is [atomic](http://www.freshconsulting.com/atomic-commits/):
    - Commit each fix or task as a separate change; only commit when a block
      of work is complete (or commit and squash afterwards)
    - Layer changes, e.g. first make a separate commit formatting the code or
      refactoring, then commit the actual logic change. This way, the latter
      commit contains the only minimal diff, which is to be reviewed

  - [x] That is, the commit don't mix together:
    - Whitespace changes or code formatting with functional code changes
    - Large refactorings and logic changes
    - Two unrelated functional changes

  <details><summary>The commit log message follows <b><a href="https://github.com/embox/embox/wiki/Contributing:-Git-commit-message-agreements">the agreements</a></b></summary><p>

 1. **Start the subject by specifying the subsystem / module**

    > `subsys:` is where the change belongs to: `arch: `/`kernel: `/`stm32: `/`util: `/`mk: `/...<br/>
    > `(label)` is what kind of change it is: `(docs)`/`(template)`/`(refactor)`/`(minor)`/`(MAJOR)`

 2. Use the imperative mood in the subject line: `Fix` instead of fix~~ing~~ / fix~~ed~~

 3. Capitalize the sentence after the subsystem / labels

 4. Do not end the subject line with a period

 5. **Separate subject from body with a blank line**

 6. **Wrap the body at 72 characters**:
    [stopwritingramblingcommitmessages.com](http://stopwritingramblingcommitmessages.com)

    > Except for code blocks, long URLs, references to other commits: paste
    > these verbatim, but indent with 4 spaces and surround with empty lines.

 7. Subject: **what** is changed; body: **why** that, but not **_how_**
    (the diff tells _how_)

</p></details>

 - Compare:

    | Good | Bad |
    | ---- | --- |
    | *fs: Add missing 'foo -> bar' dependence*                     | *Fix build*
    | *arm/stm32: (template) Decrease thread pool size to fit RAM*  | *Fix arm/stm32 template*
    | *util: (refactor) Extract foo_check_xxx() from foo_func()*    | *util: Work on foo refactoring*

Open a Pull Request
-------------------

Before proposing a new Pull Request, please check that:
  - [x] None of the commits introduce whitespace errors
    - Rebase with `--whitespace=fix` to fix them, if any; see the
      [cheatsheet](#fix-whitespace-errors)

  - [x] There are no merge commits, e.g. you didn't accidentally `pull` without
    `--rebase`

  - [x] There are no "oops" or fixup commits; don't forget to squash them

  - [x] (ideally) The whole set of commits is bisectable, i.e. the code
    builds and runs fine after applying each commit one by one
    - Run `git rebase upstream/master -i --exec make` to check that

Everything's OK? Now create the PR:

  - Provide the title and the description
    - Follow [commit message agreements](https://github.com/embox/embox/wiki/Contributing:-Git-commit-message-agreements) as a rule of thumb


Integrate feedback
------------------

  - Do not rebase already published changes and force-push **until review is
    over**
    - Push `fixup!` commits instead [[cheatsheet](https://github.com/embox/embox/wiki/Contributing:-Git-cheatsheet#option-b-commit---fixup-followed-by-rebase---autosquash)], then rebase one last time once getting LGTM from maintainers :shipit:

  - [x] For maintainers: Travis CI has finishes building the PR
    - Don't merge until receiving the green mark, even if you believe
      everything's OK. No need to hurry.
