// Documentation Page JavaScript - Dynamic Content Loading

document.addEventListener('DOMContentLoaded', function() {
    loadDocumentation();
});

// Load documentation from global DOCUMENTATION_DATA
function loadDocumentation() {
    try {
        // Check if documentation data exists
        if (typeof DOCUMENTATION_DATA === 'undefined') {
            throw new Error('DOCUMENTATION_DATA not found. Make sure documentation-data.js is loaded.');
        }
        
        const data = DOCUMENTATION_DATA;
        
        // Set page title
        document.getElementById('doc-title').textContent = data.title || 'Documentation';
        document.title = data.title || 'Plugin Documentation';
        
        // Generate content sections
        generateContent(data.sections);
        
        // Generate table of contents
        generateTableOfContents();
        
        // Setup scroll spy and smooth scrolling
        handleScrollSpy();
        setupSmoothScrolling();
        
    } catch (error) {
        console.error('Error loading documentation:', error);
        document.getElementById('main-content').innerHTML = 
            '<div class="error">Failed to load documentation. Error: ' + error.message + '</div>';
    }
}

// Generate content sections from JSON data
function generateContent(sections) {
    const mainContent = document.getElementById('main-content');
    mainContent.innerHTML = '';
    
    sections.forEach(section => {
        const sectionEl = document.createElement('section');
        sectionEl.className = 'content-section';
        sectionEl.id = section.id;
        
        // Add section heading
        const heading = document.createElement('h2');
        heading.textContent = section.title;
        sectionEl.appendChild(heading);
        
        // Parse and add body content
        const bodyContent = parseBody(section.body);
        sectionEl.appendChild(bodyContent);
        
        mainContent.appendChild(sectionEl);
    });
}

// Parse body content and convert special syntax to HTML
function parseBody(body) {
    const container = document.createElement('div');
    
    // Split by double newlines to get paragraphs/blocks
    const blocks = body.split('\n\n');
    
    blocks.forEach(block => {
        const trimmed = block.trim();
        if (!trimmed) return;
        
        // Check for special block types
        if (trimmed.startsWith('### ')) {
            // H3 heading
            const h3 = document.createElement('h3');
            h3.textContent = trimmed.substring(4);
            container.appendChild(h3);
        }
        else if (trimmed.startsWith('![')) {
            // Image: ![alt text](url)
            const match = trimmed.match(/!\[(.*?)\]\((.*?)\)/);
            if (match) {
                const imgContainer = document.createElement('div');
                imgContainer.className = 'image-container';
                
                const img = document.createElement('img');
                img.src = match[2];
                img.alt = match[1];
                img.className = 'doc-image';
                
                imgContainer.appendChild(img);
                
                // Add caption if alt text exists
                if (match[1]) {
                    const caption = document.createElement('div');
                    caption.className = 'image-caption';
                    caption.textContent = match[1];
                    imgContainer.appendChild(caption);
                }
                
                container.appendChild(imgContainer);
            }
        }
        else if (trimmed.startsWith('```')) {
            // Code block
            const codeMatch = trimmed.match(/```(\w+)?\n([\s\S]*?)```/);
            if (codeMatch) {
                const pre = document.createElement('pre');
                const code = document.createElement('code');
                code.textContent = codeMatch[2];
                pre.appendChild(code);
                container.appendChild(pre);
            }
        }
        else if (trimmed.startsWith('> **Note:**') || trimmed.startsWith('> **Warning:**')) {
            // Note box
            const noteBox = document.createElement('div');
            noteBox.className = 'note-box';
            const content = trimmed.substring(2); // Remove '> '
            noteBox.innerHTML = parseInlineFormatting(content);
            container.appendChild(noteBox);
        }
        else if (trimmed.match(/^\d+\.\s/)) {
            // Ordered list
            const ol = document.createElement('ol');
            const items = trimmed.split('\n');
            items.forEach(item => {
                const match = item.match(/^\d+\.\s(.+)/);
                if (match) {
                    const li = document.createElement('li');
                    li.innerHTML = parseInlineFormatting(match[1]);
                    ol.appendChild(li);
                }
            });
            container.appendChild(ol);
        }
        else if (trimmed.match(/^[-*]\s/)) {
            // Unordered list
            const ul = document.createElement('ul');
            const items = trimmed.split('\n');
            items.forEach(item => {
                const match = item.match(/^[-*]\s(.+)/);
                if (match) {
                    const li = document.createElement('li');
                    li.innerHTML = parseInlineFormatting(match[1]);
                    ul.appendChild(li);
                }
            });
            container.appendChild(ul);
        }
        else {
            // Regular paragraph
            const p = document.createElement('p');
            p.innerHTML = parseInlineFormatting(trimmed);
            container.appendChild(p);
        }
    });
    
    return container;
}

// Parse inline formatting (bold, code, links)
function parseInlineFormatting(text) {
    // Code: `code`
    text = text.replace(/`([^`]+)`/g, '<code>$1</code>');
    
    // Bold: **bold**
    text = text.replace(/\*\*([^*]+)\*\*/g, '<strong>$1</strong>');
    
    // Links: [text](url)
    text = text.replace(/\[([^\]]+)\]\(([^)]+)\)/g, '<a href="$2">$1</a>');
    
    return text;
}

// Generate Table of Contents from sections
function generateTableOfContents() {
    const tocList = document.getElementById('toc-list');
    const sections = document.querySelectorAll('.content-section');
    
    tocList.innerHTML = '';
    
    sections.forEach(section => {
        const heading = section.querySelector('h2');
        if (heading) {
            const li = document.createElement('li');
            li.className = 'toc-item';
            
            const a = document.createElement('a');
            a.href = `#${section.id}`;
            a.className = 'toc-link';
            a.textContent = heading.textContent;
            a.setAttribute('data-section', section.id);
            
            li.appendChild(a);
            tocList.appendChild(li);
        }
    });
}

// Smooth scrolling for TOC links
function setupSmoothScrolling() {
    document.getElementById('toc-list').addEventListener('click', function(e) {
        if (e.target.classList.contains('toc-link')) {
            e.preventDefault();
            const targetId = e.target.getAttribute('href').substring(1);
            const targetSection = document.getElementById(targetId);
            
            if (targetSection) {
                const headerOffset = 80;
                const elementPosition = targetSection.getBoundingClientRect().top;
                const offsetPosition = elementPosition + window.pageYOffset - headerOffset;
                
                window.scrollTo({
                    top: offsetPosition,
                    behavior: 'smooth'
                });
            }
        }
    });
}

// Scroll spy - highlight active section in TOC
function handleScrollSpy() {
    let ticking = false;
    
    function updateActiveLink() {
        const sections = document.querySelectorAll('.content-section');
        const tocLinks = document.querySelectorAll('.toc-link');
        let currentSection = '';
        const scrollPosition = window.pageYOffset + 100;
        
        sections.forEach(section => {
            const sectionTop = section.offsetTop;
            const sectionHeight = section.clientHeight;
            
            if (scrollPosition >= sectionTop && scrollPosition < sectionTop + sectionHeight) {
                currentSection = section.id;
            }
        });
        
        tocLinks.forEach(link => {
            link.classList.remove('active');
            if (link.getAttribute('data-section') === currentSection) {
                link.classList.add('active');
                
                const sidebar = document.getElementById('sidebar');
                const linkTop = link.offsetTop;
                const linkHeight = link.clientHeight;
                const sidebarHeight = sidebar.clientHeight;
                const sidebarScroll = sidebar.scrollTop;
                
                if (linkTop < sidebarScroll || linkTop + linkHeight > sidebarScroll + sidebarHeight) {
                    sidebar.scrollTo({
                        top: linkTop - sidebarHeight / 2 + linkHeight / 2,
                        behavior: 'smooth'
                    });
                }
            }
        });
    }
    
    window.addEventListener('scroll', function() {
        if (!ticking) {
            window.requestAnimationFrame(function() {
                updateActiveLink();
                ticking = false;
            });
            ticking = true;
        }
    });
    
    setTimeout(updateActiveLink, 100);
}