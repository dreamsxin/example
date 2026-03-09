import { chromium } from 'playwright';

// ---------- 核心函数：生成精简 HTML，refs 存储定位信息 ----------
let refCounter = 0;
function resetRefs() { refCounter = 0; }
function nextRef() { return `e${++refCounter}`; }

const INTERACTIVE_ROLES = new Set(['button','link','textbox','checkbox','radio','combobox','listbox','menuitem','option','searchbox','slider','spinbutton','switch','tab','treeitem']);
const CONTENT_ROLES = new Set(['heading','cell','gridcell','columnheader','rowheader','listitem','article','region','main','navigation']);
const STRUCTURAL_ROLES = new Set(['generic','group','list','table','row','rowgroup','grid','treegrid','menu','menubar','toolbar','tablist','tree','directory','document','application','presentation','none']);

function parseAriaLine(line: string): { indent: number; role: string; name?: string; attrs: Record<string, string> } | null {
  const indentMatch = line.match(/^(\s*)-/);
  if (!indentMatch) return null;
  const indent = indentMatch[1].length;
  const content = line.slice(indent + 1).trim();
  const roleMatch = content.match(/^(\w+)/);
  if (!roleMatch) return null;
  const role = roleMatch[1];
  let remaining = content.slice(role.length).trim();
  let name: string | undefined;
  const nameMatch = remaining.match(/^"([^"]*)"/);
  if (nameMatch) { name = nameMatch[1]; remaining = remaining.slice(nameMatch[0].length).trim(); }
  const attrs: Record<string, string> = {};
  const attrRegex = /\[([^\]=]+)(?:=("[^"]*"|[^\]]*))?\]/g;
  let attrMatch;
  while ((attrMatch = attrRegex.exec(remaining)) !== null) {
    const key = attrMatch[1];
    let value = attrMatch[2];
    if (value && value.startsWith('"') && value.endsWith('"')) value = value.slice(1, -1);
    attrs[key] = value || '';
  }
  return { indent, role, name, attrs };
}

function roleToHtml(role: string, name: string | undefined, attrs: Record<string, string>, ref?: string): { tag: string; attributes: Record<string, string>; textContent?: string; selfClosing: boolean } {
  role = role.toLowerCase();
  const baseAttrs: Record<string, string> = {};
  if (ref) baseAttrs['data-ref'] = ref;
  switch (role) {
    case 'heading': { const level = attrs['level'] ? parseInt(attrs['level'], 10) : 2; const tag = `h${Math.min(6, Math.max(1, level))}`; return { tag, attributes: baseAttrs, textContent: name, selfClosing: false }; }
    case 'link': { const attributes = { ...baseAttrs }; if (attrs['url']) attributes['href'] = attrs['url']; return { tag: 'a', attributes, textContent: name, selfClosing: false }; }
    case 'button': return { tag: 'button', attributes: baseAttrs, textContent: name, selfClosing: false };
    case 'textbox':
    case 'searchbox': return { tag: 'input', attributes: { ...baseAttrs, type: 'text', 'aria-label': name || '' }, selfClosing: true };
    case 'checkbox': return { tag: 'input', attributes: { ...baseAttrs, type: 'checkbox', 'aria-label': name || '' }, selfClosing: true };
    case 'radio': return { tag: 'input', attributes: { ...baseAttrs, type: 'radio', 'aria-label': name || '' }, selfClosing: true };
    case 'combobox': return { tag: 'select', attributes: { ...baseAttrs, 'aria-label': name || '' }, selfClosing: false };
    case 'listbox': return { tag: 'select', attributes: { ...baseAttrs, multiple: true, 'aria-label': name || '' }, selfClosing: false };
    case 'paragraph': return { tag: 'p', attributes: baseAttrs, textContent: name, selfClosing: false };
    default: {
      const attributes = { ...baseAttrs, role };
      if (name) {
        if (['article','region','main','navigation','listitem'].includes(role)) return { tag: 'div', attributes, textContent: name, selfClosing: false };
        else attributes['aria-label'] = name;
      }
      return { tag: 'div', attributes, selfClosing: false };
    }
  }
}

interface AriaNode { level: number; role: string; name?: string; attrs: Record<string, string>; ref?: string; children: AriaNode[]; }

function buildTree(lines: AriaNode[]): AriaNode[] {
  const root: AriaNode[] = []; const stack: { node: AriaNode; level: number }[] = [];
  for (const node of lines) {
    while (stack.length && stack[stack.length-1].level >= node.level) stack.pop();
    if (stack.length === 0) { root.push(node); stack.push({ node, level: node.level }); }
    else { stack[stack.length-1].node.children.push(node); stack.push({ node, level: node.level }); }
  }
  return root;
}

function renderNode(node: AriaNode): string {
  const { tag, attributes, textContent, selfClosing } = roleToHtml(node.role, node.name, node.attrs, node.ref);
  const attrString = Object.entries(attributes).map(([k, v]) => v ? `${k}="${v.replace(/"/g, '&quot;')}"` : k).join(' ');
  const openTag = attrString ? `<${tag} ${attrString}>` : `<${tag}>`;
  if (selfClosing) return openTag;
  const childrenHtml = node.children.map(child => renderNode(child)).join('');
  const content = textContent || '';
  return `${openTag}${content}${childrenHtml}</${tag}>`;
}

// 生成精简 HTML 并返回 refs 映射（角色、名称、索引）
async function getSimplifiedHTML(page: any, options?: { selector?: string; compact?: boolean; interactiveOnly?: boolean }): Promise<{ html: string; refs: Record<string, { role: string; name: string; nth?: number }> }> {
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
    const { indent, role, name, attrs } = parsed;
    const roleLower = role.toLowerCase();
    if (options?.compact && STRUCTURAL_ROLES.has(roleLower) && !name) continue;
    if (options?.interactiveOnly && !INTERACTIVE_ROLES.has(roleLower)) continue;
    const shouldHaveRef = INTERACTIVE_ROLES.has(roleLower) || (CONTENT_ROLES.has(roleLower) && !!name);
    const node: AriaNode = { level: indent, role: roleLower, name, attrs, children: [] };
    if (shouldHaveRef) {
      const ref = nextRef();
      node.ref = ref;
      const key = `${roleLower}:${name ?? ''}`;
      const count = roleNameCounts.get(key) || 0;
      roleNameCounts.set(key, count + 1);
      refs[ref] = {
        role: roleLower,
        name: name ?? '',
        nth: count // 第几个同名元素（从0开始）
      };
    }
    rawNodes.push(node);
  }
  const tree = buildTree(rawNodes);
  const bodyChildren = tree.map(node => renderNode(node)).join('');
  const fullHtml = `<body>\n${bodyChildren}\n</body>`;
  return { html: fullHtml, refs };
}

// ---------- 新增：通过 ref 获取定位器 ----------
/**
 * 根据 ref 从 refs 映射中获取元素定位器
 * @param page Playwright 页面对象
 * @param refs getSimplifiedHTML 返回的 refs 映射
 * @param ref 要查找的 ref 字符串（如 "e1"）
 * @returns Playwright Locator
 * @throws 如果 ref 不存在则抛出错误
 */
function getLocatorByRef(page: any, refs: Record<string, { role: string; name: string; nth?: number }>, ref: string) {
  const info = refs[ref];
  if (!info) {
    throw new Error(`Ref "${ref}" not found`);
  }
  const { role, name, nth } = info;
  let locator = page.getByRole(role, { name, exact: true });
  if (nth !== undefined) {
    locator = locator.nth(nth);
  }
  return locator;
}

/**
 * 通过 ref 点击元素（便捷方法）
 */
async function clickByRef(page: any, refs: Record<string, { role: string; name: string; nth?: number }>, ref: string) {
  const locator = getLocatorByRef(page, refs, ref);
  await locator.click();
}

// ---------- 主流程：演示通过 ref 查找并点击 ----------
(async () => {
  const browser = await chromium.launch({ headless: false });
  const page = await browser.newPage();
  console.log('🌐 正在打开百度...');
  await page.goto('https://www.baidu.com', { waitUntil: 'networkidle' });

  // 1. 获取精简 HTML 和 refs 映射
  console.log('🔍 获取精简 HTML...');
  const { html, refs } = await getSimplifiedHTML(page, { compact: true });
  console.log('📄 生成的精简 HTML（部分）:\n', html.slice(0, 500) + '...\n');
  console.log('🔖 可用元素引用（包含角色和名称）:', Object.entries(refs).map(([ref, info]) => `${ref}: ${info.role} "${info.name}" (nth=${info.nth})`));

  // 2. 通过角色找到文本框和按钮的 ref
  const searchBoxEntry = Object.entries(refs).find(([_, info]) => info.role === 'textbox');
  const buttonEntry = Object.entries(refs).find(([_, info]) => info.role === 'button');

  if (!searchBoxEntry || !buttonEntry) {
    console.log('❌ 未找到文本框或按钮');
    await browser.close();
    return;
  }

  const [searchBoxRef, searchBoxInfo] = searchBoxEntry;
  const [buttonRef, buttonInfo] = buttonEntry;

  console.log(`\n🔍 找到文本框 ref: ${searchBoxRef}，按钮 ref: ${buttonRef}`);

  // 3. 演示通过 ref 直接点击（使用 getLocatorByRef 获取定位器，然后操作）
  console.log(`\n✍️ 通过 ref="${searchBoxRef}" 点击搜索框...`);
  const searchBoxLocator = getLocatorByRef(page, refs, searchBoxRef);
  await searchBoxLocator.click();
  await page.keyboard.type('playwright 自动化');
  await page.waitForTimeout(500);

  console.log(`\n🖱️ 通过 ref="${buttonRef}" 点击“百度一下”按钮...`);
  await clickByRef(page, refs, buttonRef); // 使用便捷方法

  await page.waitForTimeout(3000);

   // 4. 获取搜索结果，精简 HTML 和 refs 映射
  console.log('🔍 获取精简 HTML...');

  const res = await getSimplifiedHTML(page, { compact: true });
  console.log('📄 生成的精简 HTML（部分）:\n', res.html.slice(0, 500) + '...\n');
  console.log('🔖 可用元素引用（包含角色和名称）:', Object.entries(res.refs).map(([ref, info]) => `${ref}: ${info.role} "${info.name}" (nth=${info.nth})`));

  // 5. 点击最后一个链接
  const linkEntry = Object.entries(res.refs).find(([_, info]) => info.role === 'link');
  if (!linkEntry) {
    console.log('❌ 未找到任何链接');
    await browser.close();
    return;
  }
  
  const [lastLinkRef, lastLinkInfo] = linkEntry;
  console.log(`\n🔍 找到链接 ref: ${lastLinkRef}`);
  await clickByRef(page, res.refs, "e64"); // 使用便捷方法

  await page.waitForTimeout(10000);
  await browser.close();
  console.log('✅ 演示完成。');
})();
