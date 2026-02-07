# LicGen Web UI

A static, single-page interface for generating software licenses.

## Overview

This project provides a frontend for generating license text and CLI commands. It mimics the Shadcn UI aesthetic using Tailwind CSS (via CDN) and runs entirely in the browser without a build step.

Key features:
- **Dynamic Fetching**: Queries the GitHub API to list and retrieve license templates from the `templates/` directory on the `master` branch.
- **Client-side Parsing**: Parses custom `.template` files (metadata headers + body) in JavaScript to display permissions, conditions, and limitations.
- **Reactive Preview**: Updates license text and command previews in real-time.

## Limitations & Known Issues

### API Rate Limiting
The application uses the public GitHub API to fetch template lists. Unauthenticated requests are limited to **60 requests per hour** per IP address. If this limit is exceeded, the template dropdown will fail to populate until the limit resets.

To resolve this for high-traffic deployments, you would need to implement a caching proxy or build step to bundle templates at deploy time.

### Network Dependency
This is a purely static site but relies on external resources:
-   **Tailwind CSS**: Loaded via CDN. The UI will not style correctly without internet access.
-   **GitHub API**: Required to fetch license data.

## Adding Templates

To add a new license, create a `.template` file in the `templates/` directory and push it to the **`master`** branch. The file must use the following YAML-header format:

```text
ID: mylic
Name: My Custom License
Description: Short description.
Permissions:
- Commercial use
Conditions:
- License notice
---
Copyright (c) {{year}} {{name}}
[License Text]
```
