## 安装配置

```shell
$env:OPENAI_API_KEY = "xxx"
npm install -g openclaw-cn
openclaw onboard --install-daemon
openclaw onboard
openclaw gateway restart
# 打开控制台
openclaw dashboard

# 检查服务状态
openclaw status
# 深度健康检查
openclaw health
# 诊断配置问题
openclaw doctor
```

## 配置 Brave Search API
`openclaw configure --section web`

##
让 AI 记录错误和经验，转化为长期记忆，避免重复犯同样的错。
```shell
npm install -g undici
npm i -g clawdhub
clawdhub install self-improving-agent
```

## 添加飞书

```shell
openclaw channels add
openclaw config set channels.feishu.appId "<App_ID>"
openclaw config set channels.feishu.appSecret "<App_Secret>"
openclaw config set channels.feishu.enabled true
openclaw config set channels.feishu.connectionMode websocket
openclaw config set channels.feishu.dmPolicy pairing
openclaw config set channels.feishu.groupPolicy allowlist
openclaw config set channels.feishu.requireMention true
```

**配置**
私信机器人，获得配对码
```shell
openclaw pairing approve feishu V3WUEKUX
```

## auth-profiles.json

`\.openclaw\agents\main\agent\auth-profiles.json`
```json
{
  "version": 1,
  "profiles": {
    "zai:default": {
      "type": "api_key",
      "provider": "zai",
      "key": "xxxx"
    },
    "bailian:default": {
      "type": "api_key",
      "provider": "bailian",
      "key": "xxxx"
    }
  },
  "usageStats": {
    "zai:default": {
      "errorCount": 0,
      "lastUsed": 1772687495639
    },
    "bailian:default": {
      "errorCount": 0,
      "lastUsed": 1772688448531
    }
  }
}
```

## auth-profiles.json

`\.openclaw\openclaw.json`
```json
{
  "wizard": {
    "lastRunAt": "2026-03-05T04:52:11.307Z",
    "lastRunVersion": "2026.3.2",
    "lastRunCommand": "onboard",
    "lastRunMode": "local"
  },
  "auth": {
    "profiles": {
      "zai:default": {
        "provider": "zai",
        "mode": "api_key"
      },
      "bailian:default": {
        "provider": "bailian",
        "mode": "api_key"
      }
    }
  },
  "models": {
    "mode": "merge",
    "providers": {

      "zai": {
        "baseUrl": "https://open.bigmodel.cn/api/coding/paas/v4",
        "api": "openai-completions",
        "models": [
          {
            "id": "glm-5",
            "name": "GLM-5",
            "reasoning": true,
            "input": [
              "text"
            ],
            "cost": {
              "input": 0,
              "output": 0,
              "cacheRead": 0,
              "cacheWrite": 0
            },
            "contextWindow": 204800,
            "maxTokens": 131072
          },
          {
            "id": "glm-4.7",
            "name": "GLM-4.7",
            "reasoning": true,
            "input": [
              "text"
            ],
            "cost": {
              "input": 0,
              "output": 0,
              "cacheRead": 0,
              "cacheWrite": 0
            },
            "contextWindow": 204800,
            "maxTokens": 131072
          },
          {
            "id": "glm-4.7-flash",
            "name": "GLM-4.7 Flash",
            "reasoning": true,
            "input": [
              "text"
            ],
            "cost": {
              "input": 0,
              "output": 0,
              "cacheRead": 0,
              "cacheWrite": 0
            },
            "contextWindow": 204800,
            "maxTokens": 131072
          },
          {
            "id": "glm-4.7-flashx",
            "name": "GLM-4.7 FlashX",
            "reasoning": true,
            "input": [
              "text"
            ],
            "cost": {
              "input": 0,
              "output": 0,
              "cacheRead": 0,
              "cacheWrite": 0
            },
            "contextWindow": 204800,
            "maxTokens": 131072
          }
        ]
      },
      "bailian": {
        "baseUrl": "https://dashscope.aliyuncs.com/compatible-mode/v1",
        "api": "openai-completions",
        "models": [
          {
            "id": "qwen3-max-2026-01-23",
            "name": "qwen3-max-thinking",
            "reasoning": false,
            "input": [
              "text"
            ],
            "cost": {
              "input": 0,
              "output": 0,
              "cacheRead": 0,
              "cacheWrite": 0
            },
            "contextWindow": 262144,
            "maxTokens": 65536
          }
        ]
      }
    }
  },
  "agents": {
    "defaults": {
      "model": {
        "primary": "bailian/qwen3-max-2026-01-23"
      },
      "models": {
        "bailian/qwen3-max-2026-01-23": {
          "alias": "qwen3-max-thinking"
        }
      },
      "workspace": "C:\\Users\\admin\\.openclaw\\workspace",
      "compaction": {
        "mode": "safeguard"
      },
      "maxConcurrent": 4,
      "subagents": {
        "maxConcurrent": 8
      }
    }
  },
  "tools": {
    "profile": "messaging"
  },
  "messages": {
    "ackReactionScope": "group-mentions"
  },
  "commands": {
    "native": "auto",
    "nativeSkills": "auto",
    "restart": true,
    "ownerDisplay": "raw"
  },
  "session": {
    "dmScope": "per-channel-peer"
  },
  "gateway": {
    "port": 18789,
    "mode": "local",
    "bind": "loopback",
    "auth": {
      "mode": "token",
      "token": "6f502d6264eb8888d28fd91d6331df26f075a52e59a95a0d"
    },
    "tailscale": {
      "mode": "off",
      "resetOnExit": false
    },
    "nodes": {
      "denyCommands": [
        "camera.snap",
        "camera.clip",
        "screen.record",
        "contacts.add",
        "calendar.add",
        "reminders.add",
        "sms.send"
      ]
    }
  },
  "meta": {
    "lastTouchedVersion": "2026.3.2",
    "lastTouchedAt": "2026-03-05T04:52:11.318Z"
  }
}

```
