reviewdog 是一款能够自动化的检测 ( Pull / Merge Request) 里一些语法和格式错误并提交CR评论的工具。它支持各种CI工具，目前支持以下平台的CI

- GitHub Actions
- Travis CI
- Circle CI
- GitLab CI
- Bitbucket Pipelines
- Common (Jenkins, local, etc…)
 
## install
```shell
# scoop install reviewdog
go install github.com/reviewdog/reviewdog/cmd/reviewdog@latest
```

##

```shell
reviewdog -list
```
