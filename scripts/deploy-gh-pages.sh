#!/bin/sh
set -eu

build_dir="${EM_BUILD_DIR:-dist}"
remote="${DEPLOY_REMOTE:-origin}"
branch="${DEPLOY_BRANCH:-gh-pages}"
message="${DEPLOY_MESSAGE:-Deploy web build}"

if [ ! -d "$build_dir" ] || [ ! -f "$build_dir/index.html" ] || [ ! -f "$build_dir/index.js" ]; then
  echo "Missing web build in $build_dir. Run make wasm first." >&2
  exit 1
fi

worktree="$(mktemp -d "${TMPDIR:-/tmp}/sdl-tetris-gh-pages.XXXXXX")"

cleanup() {
  git worktree remove --force "$worktree" >/dev/null 2>&1 || rm -rf "$worktree"
}
trap cleanup EXIT INT TERM

git fetch "$remote" "$branch" >/dev/null 2>&1 || true

if git show-ref --verify --quiet "refs/remotes/$remote/$branch"; then
  git worktree add "$worktree" "$remote/$branch" >/dev/null
else
  git worktree add --detach "$worktree" HEAD >/dev/null
  git -C "$worktree" checkout --orphan "$branch" >/dev/null
fi

find "$worktree" -mindepth 1 -maxdepth 1 ! -name .git -exec rm -rf {} +
cp -R "$build_dir"/. "$worktree"/

git -C "$worktree" add --all
if git -C "$worktree" diff --cached --quiet; then
  echo "No deploy changes."
  exit 0
fi

git -C "$worktree" commit -m "$message"
git -C "$worktree" push "$remote" "HEAD:$branch"
