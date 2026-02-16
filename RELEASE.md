Publishing Releases
===================

This repository includes an Actions workflow to build and publish a release when you push a tag matching `v*`.

Automated (recommended)

1. Create a tag locally and push it:

```powershell
git tag v1.0.0
git push origin v1.0.0
```

2. The workflow `.github/workflows/release-on-tag.yml` will run on `windows-latest`, build `ToDoList.exe`, create a GitHub Release for the pushed tag, and upload `ToDoList.exe` as the release asset.

Manual (via `gh`)

If you already built the executable locally (in `build/Release/ToDoList.exe`) you can publish it directly with the GitHub CLI:

```powershell
gh release create v1.0.0 build/Release/ToDoList.exe --title "v1.0.0" --notes "Initial release"
```

Manual (web UI)

1. Build the executable locally.
2. Open the repository on GitHub, go to "Releases" → "Draft a new release".
3. Create the tag (e.g., `v1.0.0`) and upload the `ToDoList.exe` binary.

Notes

- The workflow uses the automatically provided `GITHUB_TOKEN` so no additional secrets are required for the upload step.
- Ensure your build produced `build/Release/ToDoList.exe`. On some toolchains the path may be `build\ToDoList.exe`—adjust the workflow if needed.
