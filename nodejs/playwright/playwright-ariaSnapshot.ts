import { chromium } from 'playwright';

// ---------- 核心函数：生成精简 HTML 和 refs 映射 ----------
let refCounter = 0;
function resetRefs() { refCounter = 0; }
function nextRef() { return `e${++refCounter}`; }

// 角色分类
const INTERACTIVE_ROLES = new Set(['button','link','textbox','checkbox','radio','combobox','listbox','menuitem','option','searchbox','slider','spinbutton','switch','tab','treeitem']);
const CONTENT_ROLES = new Set(['heading','cell','gridcell','columnheader','rowheader','listitem','article','region','main','navigation','paragraph']);
const STRUCTURAL_ROLES = new Set(['generic','group','list','table','row','rowgroup','grid','treegrid','menu','menubar','toolbar','tablist','tree','directory','document','application','presentation','none']);

/** 解析 ARIA 快照中的一行，提取缩进、角色、名称、文本内容和属性 */
function parseAriaLine(line: string): {
  indent: number;
  role: string;
  name?: string;
  textContent?: string;
  attrs: Record<string, string>;
} | null {
  const indentMatch = line.match(/^(\s*)-/);
  if (!indentMatch) return null;
  const indent = indentMatch[1].length;
  const content = line.slice(indent + 1).trim();
  const roleMatch = content.match(/^(\w+)/);
  if (!roleMatch) return null;
  const role = roleMatch[1];

  let remaining = content.slice(role.length).trim();
  let name: string | undefined;
  let textContent: string | undefined;

  // 处理带引号的名称（如 - button "Submit"）
  const nameMatch = remaining.match(/^"([^"]*)"/);
  if (nameMatch) {
    name = nameMatch[1];
    remaining = remaining.slice(nameMatch[0].length).trim();
  }
  // 处理冒号文本（如 - paragraph: 这是一段文字）
  else if (remaining.startsWith(':')) {
    const textMatch = remaining.slice(1).match(/^([^[]*)/);
    if (textMatch) {
      textContent = textMatch[1].trim();
      remaining = remaining.slice(textMatch[0].length + 1).trim();
    }
  }

  // 提取属性 [key=value]
  const attrs: Record<string, string> = {};
  const attrRegex = /\[([^\]=]+)(?:=("[^"]*"|[^\]]*))?\]/g;
  let attrMatch;
  while ((attrMatch = attrRegex.exec(remaining)) !== null) {
    const key = attrMatch[1];
    let value = attrMatch[2];
    if (value && value.startsWith('"') && value.endsWith('"')) {
      value = value.slice(1, -1);
    }
    attrs[key] = value || '';
  }

  return { indent, role, name, textContent, attrs };
}

/** 将 ARIA 角色映射为 HTML 标签，生成展示用的元素（带 data-ref） */
function roleToHtml(
  role: string,
  name: string | undefined,
  textContent: string | undefined,
  attrs: Record<string, string>,
  ref?: string
): { tag: string; attributes: Record<string, string>; textContent?: string; selfClosing: boolean } {
  role = role.toLowerCase();
  const baseAttrs: Record<string, string> = {};
  if (ref) baseAttrs['data-ref'] = ref;

  switch (role) {
    case 'heading': {
      const level = attrs['level'] ? parseInt(attrs['level'], 10) : 2;
      const tag = `h${Math.min(6, Math.max(1, level))}`;
      return { tag, attributes: baseAttrs, textContent: name, selfClosing: false };
    }
    case 'link': {
      const attributes = { ...baseAttrs };
      if (attrs['url']) attributes['href'] = attrs['url'];
      return { tag: 'a', attributes, textContent: name, selfClosing: false };
    }
    case 'button':
      return { tag: 'button', attributes: baseAttrs, textContent: name, selfClosing: false };
    case 'textbox':
    case 'searchbox':
      return {
        tag: 'input',
        attributes: { ...baseAttrs, type: 'text', 'aria-label': name || '' },
        selfClosing: true
      };
    case 'checkbox':
      return {
        tag: 'input',
        attributes: { ...baseAttrs, type: 'checkbox', 'aria-label': name || '' },
        selfClosing: true
      };
    case 'radio':
      return {
        tag: 'input',
        attributes: { ...baseAttrs, type: 'radio', 'aria-label': name || '' },
        selfClosing: true
      };
    case 'combobox':
      return {
        tag: 'select',
        attributes: { ...baseAttrs, 'aria-label': name || '' },
        selfClosing: false
      };
    case 'listbox':
      return {
        tag: 'select',
        attributes: { ...baseAttrs, multiple: 'multiple', 'aria-label': name || '' },
        selfClosing: false
      };
    case 'paragraph':
      return { tag: 'p', attributes: baseAttrs, textContent: textContent || name, selfClosing: false };
    default: {
      const attributes: Record<string, string> = { ...baseAttrs, role };
      if (name) {
        if (['article', 'region', 'main', 'navigation', 'listitem'].includes(role)) {
          return { tag: 'div', attributes, textContent: name, selfClosing: false };
        } else {
          attributes['aria-label'] = name;
        }
      }
      return { tag: 'div', attributes, selfClosing: false };
    }
  }
}

interface AriaNode {
  level: number;
  role: string;
  name?: string;
  textContent?: string;
  attrs: Record<string, string>;
  ref?: string;
  children: AriaNode[];
}

/** 根据缩进构建树结构 */
function buildTree(lines: AriaNode[]): AriaNode[] {
  const root: AriaNode[] = [];
  const stack: { node: AriaNode; level: number }[] = [];
  for (const node of lines) {
    while (stack.length && stack[stack.length - 1].level >= node.level) {
      stack.pop();
    }
    if (stack.length === 0) {
      root.push(node);
      stack.push({ node, level: node.level });
    } else {
      stack[stack.length - 1].node.children.push(node);
      stack.push({ node, level: node.level });
    }
  }
  return root;
}

/** 递归渲染节点为 HTML 字符串 */
function renderNode(node: AriaNode): string {
  const { tag, attributes, textContent, selfClosing } = roleToHtml(
    node.role,
    node.name,
    node.textContent,
    node.attrs,
    node.ref
  );
  const attrString = Object.entries(attributes)
    .map(([k, v]) => (v ? `${k}="${v.replace(/"/g, '&quot;')}"` : k))
    .join(' ');
  const openTag = attrString ? `<${tag} ${attrString}>` : `<${tag}>`;
  if (selfClosing) return openTag;
  const childrenHtml = node.children.map(child => renderNode(child)).join('');
  const content = textContent || '';
  return `${openTag}${content}${childrenHtml}</${tag}>`;
}

/**
 * 获取精简 HTML 和 refs 映射
 * @param page Playwright 页面对象
 * @param options 配置项
 * @returns { html: 带 data-ref 的精简 HTML, refs: 角色/名称/索引映射 }
 */
async function getSimplifiedHTML(
  page: any,
  options?: { selector?: string; compact?: boolean; interactiveOnly?: boolean }
): Promise<{ html: string; refs: Record<string, { role: string; name: string; nth?: number }> }> {
  resetRefs();
  const refs: Record<string, { role: string; name: string; nth?: number }> = {};
  const locator = options?.selector ? page.locator(options.selector) : page.locator(':root');
  const ariaTree = await locator.ariaSnapshot();
  if (!ariaTree) return { html: '<body></body>', refs };

  const lines = ariaTree.split('\n');
  const rawNodes: AriaNode[] = [];
  const roleNameCounts = new Map<string, number>();

  for (const line of lines) {
    const parsed = parseAriaLine(line);
    if (!parsed) continue;
    const { indent, role, name, textContent, attrs } = parsed;
    const roleLower = role.toLowerCase();

    // 应用过滤
    if (options?.compact && STRUCTURAL_ROLES.has(roleLower) && !name && !textContent) continue;
    if (options?.interactiveOnly && !INTERACTIVE_ROLES.has(roleLower)) continue;

    // 决定是否分配 ref：交互元素，或带名称/文本的内容元素
    const shouldHaveRef = INTERACTIVE_ROLES.has(roleLower) ||
      (CONTENT_ROLES.has(roleLower) && (!!name || !!textContent));

    const node: AriaNode = {
      level: indent,
      role: roleLower,
      name,
      textContent,
      attrs,
      children: []
    };

    if (shouldHaveRef) {
      const ref = nextRef();
      node.ref = ref;
      // 对于名称，优先使用 name，若不存在则使用文本内容（仅用于计数去重，不作为定位依据）
      const displayName = name ?? textContent ?? '';
      const key = `${roleLower}:${displayName}`; // 使用显示名称去重，保证相同文本的段落有不同 nth
      const count = roleNameCounts.get(key) || 0;
      roleNameCounts.set(key, count + 1);
      refs[ref] = {
        role: roleLower,
        name: displayName, // 存储显示名称，用于去重，定位时可能不依赖它（见 getLocatorByRef）
        nth: count
      };
    }
    rawNodes.push(node);
  }

  const tree = buildTree(rawNodes);
  const bodyChildren = tree.map(node => renderNode(node)).join('');
  const fullHtml = `<body>\n${bodyChildren}\n</body>`;
  return { html: fullHtml, refs };
}

// ---------- 通过 ref 操作元素的辅助函数 ----------

/**
 * 根据 ref 获取 Playwright Locator
 * @param page Playwright 页面对象
 * @param refs getSimplifiedHTML 返回的 refs 映射
 * @param ref 要查找的 ref 字符串（如 "e1"）
 * @returns Playwright Locator
 */
function getLocatorByRef(
  page: any,
  refs: Record<string, { role: string; name: string; nth?: number }>,
  ref: string
) {
  const info = refs[ref];
  if (!info) {
    throw new Error(`Ref "${ref}" not found`);
  }
  const { role, name, nth } = info;
  // 只有名称非空时才使用 name 选项，避免错误匹配空名称的元素
  const options: { name?: string; exact?: boolean } = {};
  if (name && name.trim() !== '') {
    options.name = name;
    options.exact = true;
  }
  let locator = page.getByRole(role, options);
  if (nth !== undefined) {
    locator = locator.nth(nth);
  }
  return locator;
}

/** 通过 ref 点击元素的便捷函数 */
async function clickByRef(
  page: any,
  refs: Record<string, { role: string; name: string; nth?: number }>,
  ref: string
) {
  const locator = getLocatorByRef(page, refs, ref);
  await locator.click();
}

// ---------- 主流程：打开百度，演示通过 ref 操作 ----------
(async () => {
  const browser = await chromium.launch({ headless: false }); // 设为 false 以观察操作
  const page = await browser.newPage();
  console.log('🌐 正在打开百度...');
  await page.goto('https://www.baidu.com', { waitUntil: 'networkidle' });

  // 1. 获取精简 HTML 和 refs 映射
  console.log('🔍 获取精简 HTML...');
  const { html, refs } = await getSimplifiedHTML(page, { compact: true });
  console.log('📄 生成的精简 HTML（部分）:\n', html.slice(0, 500) + '...\n');
  console.log('🔖 可用元素引用（包含角色和名称）:', Object.entries(refs).map(([ref, info]) =>
    `${ref}: ${info.role} "${info.name}" (nth=${info.nth})`
  ));

  // 2. 通过角色找到文本框和按钮的 ref
  const searchBoxEntry = Object.entries(refs).find(([_, info]) => info.role === 'textbox');
  const buttonEntry = Object.entries(refs).find(([_, info]) => info.role === 'button');

  if (!searchBoxEntry || !buttonEntry) {
    console.log('❌ 未找到文本框或按钮');
    await browser.close();
    return;
  }

  const [searchBoxRef] = searchBoxEntry;
  const [buttonRef] = buttonEntry;

  console.log(`\n🔍 找到文本框 ref: ${searchBoxRef}，按钮 ref: ${buttonRef}`);

  // 3. 通过 ref 点击搜索框并输入文字
  console.log(`\n✍️ 通过 ref="${searchBoxRef}" 点击搜索框...`);
  const searchBox = getLocatorByRef(page, refs, searchBoxRef);
  await searchBox.click();
  await page.keyboard.type('playwright 自动化');
  await page.waitForTimeout(500);

  // 4. 通过 ref 点击“百度一下”按钮
  console.log(`\n🖱️ 通过 ref="${buttonRef}" 点击“百度一下”按钮...`);
  await clickByRef(page, refs, buttonRef); // 使用便捷函数

  // 等待搜索结果加载，观察效果
  await page.waitForTimeout(3000);
  await browser.close();
  console.log('✅ 演示完成。');
})();
