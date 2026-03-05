# Troubleshooting exercise

This repository is a small media application with a real build, real tests and
a support ticket that came in against it. The exercise is about how you find
your way around code you have never seen, not about how much of it you can
write.

## Before the session

Please do these two things and nothing more. Budget about an hour; if you are
still fighting the build after that, stop and say so — where you got stuck is
useful information, not a failure.

1. **Get it building and running.** Start from the README. The supported path
   is the container in `docker/`.

2. **Reproduce the ticket.** Read `docs/reports/MER-4417.md` and see the
   behaviour it describes for yourself.

Then write a few lines — a paragraph is plenty — covering:

- what you did to build and run it, and anything that got in your way
- what you actually observe, in your own words
- your first hypotheses about where the cause might be, and how you would
  test them

Send that over before the session. It does not need to be right. We are
interested in how you frame the problem before you have the answer.

## During the session

We will work through it together for about an hour, screen shared, in this
repository. You drive. Expect to be asked what you are looking at and why.

You will not be asked to memorise anything. Use the debugger, add logging,
read the docs, search the history, use whatever tooling you normally use — the
same as you would on a Tuesday afternoon.

There is no requirement to produce a fix. Reaching a clear account of what is
happening and why is the whole exercise. If you get there early, we will talk
about what you would do about it.

## What we are looking for

- How you turn a vague report into something you can observe
- How you navigate a codebase nobody is around to explain
- Whether you distinguish what the code does from what it should do
- How you decide when you have enough evidence to stop looking

## Notes

- `git log` and the other branches are fair game and may be useful.
- The tests pass. That is not a claim that the behaviour is correct.
