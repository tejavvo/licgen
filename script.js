// State
let licenses = {};
const REPO_OWNER = 'tejavvo';
const REPO_NAME = 'licgen';
const TEMPLATES_PATH = 'templates';

const dom = {
    licenseSelect: document.getElementById('license-select'),
    nameInput: document.getElementById('name-input'),
    yearInput: document.getElementById('year-input'),
    yearIncrement: document.getElementById('year-increment'),
    yearDecrement: document.getElementById('year-decrement'),
    nameFieldContainer: document.getElementById('name-field-container'),
    yearFieldContainer: document.getElementById('year-field-container'),
    previewText: document.getElementById('preview-text'),
    cmdPreview: document.getElementById('cmd-preview'),
    copyBtn: document.getElementById('copy-btn'),
    copyCmdBtn: document.getElementById('copy-cmd-btn'),
    tabText: document.getElementById('tab-text'),
    tabCmd: document.getElementById('tab-cmd'),
    contentText: document.getElementById('content-text'),
    contentCmd: document.getElementById('content-cmd'),
    themeToggle: document.getElementById('theme-toggle'),
    // Details
    detailsContent: document.getElementById('details-content'),
    detailsDesc: document.getElementById('details-desc')
};

async function init() {
    // Set default year
    dom.yearInput.value = new Date().getFullYear();

    // Show loading state
    dom.previewText.textContent = "Loading templates from GitHub...";
    dom.detailsDesc.textContent = "Fetching license definitions...";

    try {
        await fetchTemplates();

        // Populate Select Dropdown
        dom.licenseSelect.innerHTML = '';
        const sortedIds = Object.keys(licenses).sort();

        if (sortedIds.length === 0) {
            throw new Error("No templates found.");
        }

        sortedIds.forEach(id => {
            const lic = licenses[id];
            const option = document.createElement('option');
            option.value = lic.id;
            option.textContent = lic.name || lic.id;
            dom.licenseSelect.appendChild(option);
        });

        // Trigger initial update
        update();
        updateTabs();

    } catch (error) {
        console.error(error);
        dom.previewText.textContent = `Error loading templates: ${error.message}\n\nPlease check your internet connection or GitHub API rate limits.`;
        dom.detailsDesc.textContent = "Failed to load details.";
    }
}

async function fetchTemplates() {
    // 1. Get list of files in templates directory
    const listUrl = `https://api.github.com/repos/${REPO_OWNER}/${REPO_NAME}/contents/${TEMPLATES_PATH}`;
    const response = await fetch(listUrl);

    if (!response.ok) {
        throw new Error(`GitHub API Error: ${response.statusText}`);
    }

    const files = await response.json();
    const templateFiles = files.filter(f => f.name.endsWith('.template'));

    // 2. Fetch each template content
    const fetchPromises = templateFiles.map(async file => {
        const rawUrl = file.download_url; // Use the direct download URL provided by API
        const res = await fetch(rawUrl);
        const text = await res.text();
        return parseTemplate(text);
    });

    const parsedLicenses = await Promise.all(fetchPromises);

    // 3. Store in state
    licenses = {};
    parsedLicenses.forEach(lic => {
        if (lic && lic.id) {
            licenses[lic.id] = lic;
        }
    });
}

function parseTemplate(content) {
    // Split Metadata and Body
    // The format uses "---" to separate header and body
    const parts = content.split(/^---$/m); // multiline regex to find --- on its own line

    if (parts.length < 2) {
        console.warn("Invalid template format: missing separator");
        return null;
    }

    const header = parts[0];
    // Join the rest as body, in case there are multiple --- in text (unlikely but safe)
    const body = parts.slice(1).join('---').trim();

    const metadata = {
        id: '',
        name: '',
        description: '',
        permissions: [],
        conditions: [],
        limitations: []
    };

    // Parse Header Line by Line
    const lines = header.split('\n');
    let currentCtx = null; // 'Permissions', 'Conditions', 'Limitations'

    lines.forEach(line => {
        line = line.trim();
        if (!line) return;

        // Check for list items first
        if (line.startsWith('- ')) {
            if (currentCtx && Array.isArray(metadata[currentCtx])) {
                metadata[currentCtx].push(line.substring(2).trim());
            }
            return;
        }

        // Check for keys
        const colonIdx = line.indexOf(':');
        if (colonIdx !== -1) {
            const key = line.substring(0, colonIdx).trim();
            const value = line.substring(colonIdx + 1).trim();

            if (['Permissions', 'Conditions', 'Limitations'].includes(key)) {
                currentCtx = key.toLowerCase();
            } else {
                currentCtx = null; // Reset context for scalar values
                if (key.toLowerCase() === 'id') metadata.id = value;
                if (key.toLowerCase() === 'name') metadata.name = value;
                if (key.toLowerCase() === 'description') metadata.description = value;
                // Add other fields if needed (SPDX, Category)
            }
        }
    });

    return {
        ...metadata,
        text: body
    };
}

function update() {
    const licId = dom.licenseSelect.value;
    const name = dom.nameInput.value || "[fullname]";
    const year = dom.yearInput.value || new Date().getFullYear();

    if (!licenses[licId]) return;

    // Update Text
    let text = licenses[licId].text;
    text = text.replace(/{{year}}/g, year).replace(/{{name}}/g, name);
    dom.previewText.textContent = text;

    // Update Command
    let cmd = `licgen ${licId}`;
    if (dom.nameInput.value) cmd += ` --name "${name}"`;
    if (dom.yearInput.value) cmd += ` --year ${year}`;
    dom.cmdPreview.textContent = cmd;

    // Update Details Tab
    renderDetails(licenses[licId]);

    // Update Field Visibility
    updateFieldVisibility(licenses[licId]);
}

function updateFieldVisibility(lic) {
    if (!lic || !lic.text) return;

    const hasNamePlaceholder = lic.text.includes('{{name}}');
    const hasYearPlaceholder = lic.text.includes('{{year}}');

    // Show/hide fields based on placeholder presence
    if (dom.nameFieldContainer) {
        if (hasNamePlaceholder) {
            dom.nameFieldContainer.classList.remove('hidden');
        } else {
            dom.nameFieldContainer.classList.add('hidden');
        }
    }

    if (dom.yearFieldContainer) {
        if (hasYearPlaceholder) {
            dom.yearFieldContainer.classList.remove('hidden');
        } else {
            dom.yearFieldContainer.classList.add('hidden');
        }
    }
}

function renderDetails(lic) {
    if (!dom.detailsContent) return;

    // Description
    dom.detailsDesc.textContent = lic.description || "No description available.";

    // Helper for columns
    const createSection = (title, items, icon, colorClass) => {
        if (!items || items.length === 0) return '';
        const listItems = items.map(item => `<li class="flex items-center gap-2"><span class="${colorClass}">${icon}</span> <span>${item}</span></li>`).join('');
        return `<div class="space-y-2">
            <h4 class="font-semibold text-sm uppercase tracking-wider text-muted-foreground">${title}</h4>
            <ul class="text-sm space-y-1">${listItems}</ul>
        </div>`;
    };

    let html = '';
    html += createSection('Permissions', lic.permissions, '✓', 'text-green-500 font-bold');
    html += createSection('Conditions', lic.conditions, 'ⓘ', 'text-blue-500 font-bold');
    html += createSection('Limitations', lic.limitations, '✗', 'text-red-500 font-bold');

    dom.detailsContent.innerHTML = html;
}

// Event Listeners
dom.licenseSelect.addEventListener('change', update);
dom.nameInput.addEventListener('input', update);
dom.yearInput.addEventListener('input', update);

// Custom Year Controls
if (dom.yearIncrement) {
    dom.yearIncrement.addEventListener('click', () => {
        const currentYear = parseInt(dom.yearInput.value) || new Date().getFullYear();
        dom.yearInput.value = currentYear + 1;
        update();
    });
}

if (dom.yearDecrement) {
    dom.yearDecrement.addEventListener('click', () => {
        const currentYear = parseInt(dom.yearInput.value) || new Date().getFullYear();
        dom.yearInput.value = currentYear - 1;
        update();
    });
}

// Year input validation - only allow numeric input
if (dom.yearInput) {
    dom.yearInput.addEventListener('input', (e) => {
        // Remove non-numeric characters
        e.target.value = e.target.value.replace(/[^0-9]/g, '');
    });

    // Handle keyboard arrow up/down
    dom.yearInput.addEventListener('keydown', (e) => {
        if (e.key === 'ArrowUp') {
            e.preventDefault();
            const currentYear = parseInt(dom.yearInput.value) || new Date().getFullYear();
            dom.yearInput.value = currentYear + 1;
            update();
        } else if (e.key === 'ArrowDown') {
            e.preventDefault();
            const currentYear = parseInt(dom.yearInput.value) || new Date().getFullYear();
            dom.yearInput.value = currentYear - 1;
            update();
        }
    });
}

// Tabs
dom.tabText.addEventListener('click', () => {
    dom.tabText.dataset.state = 'active';
    dom.tabCmd.dataset.state = 'inactive';
    dom.contentText.classList.remove('hidden');
    dom.contentCmd.classList.add('hidden');
    updateTabs();
});

dom.tabCmd.addEventListener('click', () => {
    dom.tabText.dataset.state = 'inactive';
    dom.tabCmd.dataset.state = 'active';
    dom.contentText.classList.add('hidden');
    dom.contentCmd.classList.remove('hidden');
    updateTabs();
});

function updateTabs() {
    [dom.tabText, dom.tabCmd].forEach(tab => {
        if (tab.dataset.state === 'active') {
            tab.classList.add('bg-background', 'text-foreground', 'shadow-sm');
            tab.classList.remove('text-muted-foreground');
        } else {
            tab.classList.remove('bg-background', 'text-foreground', 'shadow-sm');
            tab.classList.add('text-muted-foreground');
        }
    });
}

// Copy Text
dom.copyBtn.addEventListener('click', () => {
    navigator.clipboard.writeText(dom.previewText.textContent);
    const originalText = dom.copyBtn.innerHTML;
    dom.copyBtn.innerHTML = 'Copied!';
    setTimeout(() => dom.copyBtn.innerHTML = originalText, 2000);
});

// Copy Command
dom.copyCmdBtn.addEventListener('click', () => {
    navigator.clipboard.writeText(dom.cmdPreview.textContent);
    const originalText = dom.copyCmdBtn.innerHTML;
    dom.copyCmdBtn.innerHTML = 'Copied!';
    setTimeout(() => dom.copyCmdBtn.innerHTML = originalText, 2000);
});

// Simple Theme Toggle
if (dom.themeToggle) {
    dom.themeToggle.addEventListener('click', () => {
        document.documentElement.classList.toggle('dark');
    });
}

// Start
init();
