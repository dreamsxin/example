## 将自然语言指令转为代码。

- https://github.com/browserbase/stagehand/blob/main/examples/example.ts
- https://github.com/browserbase/stagehand/blob/main/examples/operator-example.ts

```js
async function example() {
  const stagehand = new Stagehand({
    ...StagehandConfig,
    llmClient: new LangchainClient(
      new ChatOpenAI({
        model: "gpt-4o",
      }),
    ),
  });

  await stagehand.init();
  await stagehand.page.goto("https://news.ycombinator.com");

  const { story } = await stagehand.page.extract({
    schema: z.object({
      story: z.string().describe("the top story on the page"),
    }),
  });

  console.log("The top story is:", story);

  await stagehand.page.act("click the first story");

  await stagehand.close();
}
```

- browserbase/stagehand/blob/main/lib/StagehandPage.ts

```js
async act(
    actionOrOptions: string | ActOptions | ObserveResult,
  ): Promise<ActResult> {
      if (this.api) {
        const result = await this.api.act(actionOrOptions);
        await this._refreshPageFromAPI();
        this.stagehand.addToHistory("act", actionOrOptions, result);
        return result;
      }

      const requestId = Math.random().toString(36).substring(2);
      const llmClient: LLMClient = modelName
        ? this.stagehand.llmProvider.getClient(modelName, modelClientOptions)
        : this.llmClient;

      this.stagehand.log({
        category: "act",
        message: "running act",
        level: 1,
        auxiliary: {
          action: {
            value: action,
            type: "string",
          },
          requestId: {
            value: requestId,
            type: "string",
          },
          modelName: {
            value: llmClient.modelName,
            type: "string",
          },
        },
      });

      const result = await this.actHandler.observeAct(
        actionOrOptions,
        this.observeHandler,
        llmClient,
        requestId,
      );
      this.stagehand.addToHistory("act", actionOrOptions, result);
      return result;
    } catch (err: unknown) {
      if (err instanceof StagehandError || err instanceof StagehandAPIError) {
        throw err;
      }
      throw new StagehandDefaultError(err);
    }
}
```

- browserbase/stagehand/blob/main/lib/api.ts

```js
  async act(options: ActOptions | ObserveResult): Promise<ActResult> {
    return this.execute<ActResult>({
      method: "act",
      args: { ...options },
    });
  }

  private async execute<T>({
    method,
    args,
    params,
  }: ExecuteActionParams): Promise<T> {
    const urlParams = new URLSearchParams(params as Record<string, string>);
    const queryString = urlParams.toString();
    const url = `/sessions/${this.sessionId}/${method}${queryString ? `?${queryString}` : ""}`;

    const response = await this.request(url, {
      method: "POST",
      body: JSON.stringify(args),
    });

    if (!response.ok) {
      const errorBody = await response.text();
      throw new StagehandHttpError(
        `HTTP error! status: ${response.status}, body: ${errorBody}`,
      );
    }

    if (!response.body) {
      throw new StagehandResponseBodyError();
    }

    const reader = response.body.getReader();
    const decoder = new TextDecoder();
    let buffer = "";

    while (true) {
      const { value, done } = await reader.read();

      if (done && !buffer) {
        return null;
      }

      buffer += decoder.decode(value, { stream: true });
      const lines = buffer.split("\n\n");
      buffer = lines.pop() || "";

      for (const line of lines) {
        if (!line.startsWith("data: ")) continue;

        try {
          const eventData = JSON.parse(line.slice(6));

          if (eventData.type === "system") {
            if (eventData.data.status === "error") {
              throw new StagehandServerError(eventData.data.error);
            }
            if (eventData.data.status === "finished") {
              return eventData.data.result as T;
            }
          } else if (eventData.type === "log") {
            this.logger(eventData.data.message);
          }
        } catch (e) {
          console.error("Error parsing event data:", e);
          throw new StagehandResponseParseError(
            "Failed to parse server response",
          );
        }
      }

      if (done) break;
    }
  }
```

- browserbase/stagehand/blob/main/lib/handlers/actHandler.ts

```js
  public async observeAct(
    actionOrOptions: ActOptions,
    observeHandler: StagehandObserveHandler,
    llmClient: LLMClient,
    requestId: string,
  ): Promise<ActResult> {
    // Extract the action string
    let action: string;
    const observeOptions: Partial<ObserveOptions> = {};

    if (typeof actionOrOptions === "object" && actionOrOptions !== null) {
      if (!("action" in actionOrOptions)) {
        throw new StagehandInvalidArgumentError(
          "Invalid argument. Action options must have an `action` field.",
        );
      }

      if (
        typeof actionOrOptions.action !== "string" ||
        actionOrOptions.action.length === 0
      ) {
        throw new StagehandInvalidArgumentError(
          "Invalid argument. No action provided.",
        );
      }

      action = actionOrOptions.action;

      // Extract options that should be passed to observe
      if (actionOrOptions.modelName)
        observeOptions.modelName = actionOrOptions.modelName;
      if (actionOrOptions.modelClientOptions)
        observeOptions.modelClientOptions = actionOrOptions.modelClientOptions;
    } else {
      throw new StagehandInvalidArgumentError(
        "Invalid argument. Valid arguments are: a string, an ActOptions object with an `action` field not empty, or an ObserveResult with a `selector` and `method` field.",
      );
    }

    // doObserveAndAct is just a wrapper of observeAct and actFromObserveResult.
    // we did this so that we can cleanly call a Promise.race, and race
    // doObserveAndAct against the user defined timeoutMs (if one was defined)
    const doObserveAndAct = async (): Promise<ActResult> => {
      const instruction = buildActObservePrompt(
        action,
        Object.values(SupportedPlaywrightAction),
        actionOrOptions.variables,
      );

      const observeResults = await observeHandler.observe({
        instruction,
        llmClient,
        requestId,
        drawOverlay: false,
        returnAction: true,
        fromAct: true,
        iframes: actionOrOptions?.iframes,
      });

      if (observeResults.length === 0) {
        return {
          success: false,
          message: `Failed to perform act: No observe results found for action`,
          action,
        };
      }

      const element: ObserveResult = observeResults[0];

      if (actionOrOptions.variables) {
        Object.keys(actionOrOptions.variables).forEach((key) => {
          element.arguments = element.arguments.map((arg) =>
            arg.replace(`%${key}%`, actionOrOptions.variables![key]),
          );
        });
      }

      return this.actFromObserveResult(
        element,
        actionOrOptions.domSettleTimeoutMs,
      );
    };

    // if no user defined timeoutMs, just do observeAct + actFromObserveResult
    // with no timeout
    if (!actionOrOptions.timeoutMs) {
      return doObserveAndAct();
    }

    // Race observeAct + actFromObserveResult vs. the timeoutMs
    const { timeoutMs } = actionOrOptions;
    return await Promise.race([
      doObserveAndAct(),
      new Promise<ActResult>((resolve) => {
        setTimeout(() => {
          resolve({
            success: false,
            message: `Action timed out after ${timeoutMs}ms`,
            action,
          });
        }, timeoutMs);
      }),
    ]);
  }
```

## method

- act
- navigate
- observe
- agentExecute

/sessions/${this.sessionId}/${method}${queryString ? `?${queryString}` : ""}
