## prompt

- https://github.com/browserbase/codegen-example/blob/master/src/app/actions.ts

```js
"use server";

import Anthropic from "@anthropic-ai/sdk";
import playwright from "playwright";

// Automatically sets apiKey using process.env.ANTHROPIC_API_KEY
const anthropic = new Anthropic();

const browserbaseApiKey = process.env.BROWSERBASE_API_KEY;
if (!browserbaseApiKey) {
  throw new Error("BROWSERBASE_API_KEY is required");
}

export async function getBrowserbasePage(targetUrl: string) {
  try {
    const browser = await playwright.chromium.connectOverCDP(
      `wss://connect.browserbase.com?apiKey=${browserbaseApiKey}`,
    );
    const defaultContext = browser.contexts()[0];
    const page = defaultContext.pages()[0];
    await page.goto(targetUrl);
    let pageHTML = "";
    pageHTML = await page.evaluate(() => {
      return document.documentElement.outerHTML;
    });
    if (!pageHTML) {
      pageHTML = await page.content();
    }
    return {
      pageHTML,
      pageUrl: page.url(),
    };
  } catch (err: unknown) {
    console.error(err);
    throw new Error("Failed to connect to session");
  }
}

export async function codegenAction(
  cmd: string,
  script: string,
  pageHTML: string,
) {
  let pageHTMLPrompt = "";

  if (pageHTML) {
    pageHTMLPrompt = `\n**Page HTML:**\n\`\`\`html\n${pageHTML}\n\`\`\``;
  }

  const prompt = `
  You are an AI that produces snippets of Playwright JS code.

  Your task is to extend a Playwright JS script according to an instruction. You will be provided with a Playwright script and the associated HTML page source. You must respond with new lines of Playwright JS code to be evaluated and appended the script.
  ${pageHTMLPrompt}

  **Playwright JS script to extend:**
  \`\`\`js
  ${script}
  \`\`\`

  **IMPORTANT:**
  - The script above was already executed. Your code will be appended to the end of the script and executed automatically without review. Your response MUST NOT include any text that isn't Playwright JS code. DO NOT include an explanation or any text that isn't valid playwright JS code in your response.
  - If you write a comment, add it to the line above the line you're commenting on. DO NOT add comments to the same line as a JS command as it will cause a SyntaxError.
  - Prefer \`let\` variable assignments to \`const\`. Do not re-assign variables; always use new variables or re-use existing variables that have the required value.
  - If you aren't asked you to open a new page, you must use an existing page from the script. You should infer the correct page index from the sequence of open pages. If you cannot infer a page variable, you should open a new page. Example:
    If the ACTIVE PAGE is '2. https://example2.com'. As it's index #2 in the list of open pages, it can be inferred that \`defaultContext.pages()[2]\` is the correct page to use in your response.

  **Playwright rules:**
  - Create new pages with the default context variable, not the browser variable. Example: \`const newPage = await defaultContext.newPage()\`
  - Follow Playwright best practices, such as:
    - Use waitForSelector or waitForLoadState when necessary to ensure elements are ready.
    - Prefer locator methods over deprecated elementHandle methods, example: \`page.locator()\`.
    - Avoid locator strict mode errors by adding \`.first()\` to a locator when accessing the first element. Example: \`page.locator('button').first()\`.
    - Use specific, robust locators: \`page.locator('button[data-testid="submit-btn"]')\` instead of \`page.locator('button')\`
    - Use page.evaluate for complex interactions that require JavaScript execution in the browser context.

  Instruction instruction: ${cmd}

  Remember: Your code will be appended to the end of the script and executed automatically. Ensure it's complete, correct, and safe to run.

  If you lack sufficient information to complete the user instruction, respond with:
  \`console.error("Unable to complete request: [Brief, actionable explanation]")\`
  Example: \`console.error("Unable to complete request: Cannot find element. Please provide more specific selector or element text.")\`
  `;
  try {
    const params: Anthropic.MessageCreateParams = {
      messages: [{ role: "user", content: prompt }],
      model: "claude-3-5-sonnet-20240620",
      max_tokens: 2048,
    };
    const message = await anthropic.messages.create(params);
    const lastMessage = message.content[message.content.length - 1];
    const response = lastMessage.type === "text" ? lastMessage.text : "";
    return response;
  } catch (err: unknown) {
    return `console.error("Couldn't execute command")`;
  }
}
```
