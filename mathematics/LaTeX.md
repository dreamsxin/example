#

- https://github.com/latex3/latex2e
- https://github.com/latex3/latex3
- https://tex.stackexchange.com/
- https://github.com/learnlatex/learnlatex.github.io

## 编辑器

- https://www.texstudio.org/
- https://github.com/TeXworks/texworks
- https://github.com/brucemiller/LaTeXML
- 

# :sparkles: LaTeX 数学公式语法大全
## 保留字符

LaTeX 环境中具有特殊含义的保留字符，不能直接使用，必须通过指定的语法实现：

| 序号   |  符号     |   LaTeX   | 序号   |    符号       |    LaTeX     |
| :---: | :-----:   | :-------: | :---: | :----------: | :----------: |
| **1** |   #       |   `\#`    | **6** |     {        |     `\{`     |
| **2** |   %       |   `\%`    | **7** |     }        |     `\}`     |
| **3** |   ^       | `^\wedge` | **8** |     ~        |    `\sim`    |
| **4** |   &       |   `\&`    | **9** |     ∖        | `\backslash` |
| **5** |   _       |   `\_`    |       |              |              |

## 希腊字母

|  序号  |  标准符号  |   LaTeX    |  首字母大写  |   LaTeX    |   使用 `var` 前缀    |          LaTeX           |    读音     |
| :----: | :--------: | :--------: | :----------: | :--------: | :-------------------: | :----------------------: | :---------: |
| **1**  |  $\alpha$  |  `\alpha`  |              |            |                       |                          |   /ˈælfə/   |
| **2**  |  $\beta$   |  `\beta`   |              |            |                       |                          |  /ˈbeɪtə/   |
| **3**  |  $\gamma$  |  `\gamma`  |   $\Gamma$   |  `\Gamma`  |      $\varGamma$      |       `\varGamma`        |   /ˈɡæmə/   |
| **4**  |  $\delta$  |  `\delta`  |   $\Delta$   |  `\Delta`  |      $\varDelta$      |       `\varDelta`        |  /ˈdɛltə/   |
| **5**  | $\epsilon$ | `\epsilon` |              |            |     $\varepsilon$     |      `\varepsilon`       | /ˈɛpsɪlɒn/  |
| **6**  |  $\zeta$   |  `\zeta`   |              |            |                       |                          |  /ˈzeɪtə/   |
| **7**  |   $\eta$   |   `\eta`   |              |            |                       |                          |   /ˈeɪtə/   |
| **8**  |  $\theta$  |  `\theta`  |   $\Theta$   |  `\Theta`  | $\vartheta \varTheta$ | `\vartheta` `\varTheta`  |  /ˈθiːtə/   |
| **9**  |  $\iota$   |  `\iota`   |              |            |                       |                          |  /aɪˈoʊtə/  |
| **10** |  $\kappa$  |  `\kappa`  |              |            |      $\varkappa$      |       `\varkappa`        |   /ˈkæpə/   |
| **11** | $\lambda$  | `\lambda`  |  $\Lambda$   | `\Lambda`  |      $\varLambda$     |       `\varLambda`       |  /ˈlæmdə/   |
| **12** |   $\mu$    |   `\mu`    |              |            |                       |                          |   /mjuː/    |
| **13** |   $\nu$    |   `\nu`    |              |            |                       |                          |   /njuː/    |
| **14** |   $\xi$    |   `\xi`    |    $\Xi$     |   `\Xi`    |        $\varXi$       |         `\varXi`         | /zaɪ, ksaɪ/ |
| **15** |    $o$     |    `o`     |     $O$      |    `O`     |                       |                          | /ˈɒmɪkrɒn/  |
| **16** |   $\pi$    |   `\pi`    |    $\Pi$     |   `\Pi`    |    $\varpi \varPi$    |    `\varpi` `\varPi`     |    /paɪ/    |
| **17** |   $\rho$   |   `\rho`   |              |            |       $\varrho$       |        `\varrho`         |    /roʊ/    |
| **18** |  $\sigma$  |  `\sigma`  |   $\Sigma$   |  `\Sigma`  | $\varsigma \varSigma$ | `\varsigma` `\varSigma`  |  /ˈsɪɡmə/   |
| **19** |   $\tau$   |   `\tau`   |              |            |                       |                          | /taʊ, tɔː/  |
| **20** | $\upsilon$ | `\upsilon` |  $\Upsilon$  | `\Upsilon` |     $\varUpsilon$     |      `\varUpsilon`       | /ˈʌpsɪlɒn/  |
| **21** |   $\phi$   |   `\phi`   |    $\Phi$    |   `\Phi`   |   $\varphi \varPhi$   |   `\varphi` `\varPhi`    |    /faɪ/    |
| **22** |   $\chi$   |   `\chi`   |              |            |                       |                          |    /kaɪ/    |
| **23** |   $\psi$   |   `\psi`   |    $\Psi$    |   `\Psi`   |       $\varPsi$       |        `\varPsi`         |   /psaɪ/    |
| **24** |  $\omega$  |  `\omega`  |   $\Omega$   |  `\Omega`  |      $\varOmega$      |       `\varOmega`        | /oʊˈmeɪɡə/  |
| **25** | $\digamma$ | `\digamma` |              |            |                       |                          | /daɪ'gæmə/  |

## 希伯来字母

| 序号  |  符号  |  LaTeX   | 英文  | 序号  |  符号   |   LaTeX   |  英文  |
| :---: | :------: | :------: | :---: | :---: | :-------: | :-------: | :----: |
| **1** | $\aleph$ | `\aleph` | aleph | **3** | $\gimel$  | `\gimel`  | gimel  |
| **2** | $\beth$  | `\beth`  | beth  | **4** | $\daleth$ | `\daleth` | daleth |

## 二元运算符

|  序号  |        符号      |       LaTeX       |  序号  |         符号       |       LaTeX        |
| :----: | :--------------: | :--------------: | :----: | :----------------: | :----------------: |
| **1**  |       $+$        |       `+`        | **20** |     $\bullet$      |     `\bullet`      |
| **2**  |       $-$        |       `-`        | **21** |      $\oplus$      |      `\oplus`      |
| **3**  |     $\times$     |     `\times`     | **22** |     $\ominus$      |     `\ominus`      |
| **4**  |      $\div$      |      `\div`      | **23** |      $\odot$       |      `\odot`       |
| **5**  |      $\pm$       |      `\pm`       | **24** |     $\oslash$      |     `\oslash`      |
| **6**  |      $\mp$       |      `\mp`       | **25** |     $\otimes$      |     `\otimes`      |
| **7**  | $\triangleleft$  | `\triangleleft`  | **26** |     $\bigcirc$     |     `\bigcirc`     |
| **8**  | $\triangleright$ | `\triangleright` | **27** |     $\diamond$     |     `\diamond`     |
| **9**  |     $\cdot$      |     `\cdot`      | **28** |      $\uplus$      |      `\uplus`      |
| **10** |   $\setminus$    |   `\setminus`    | **29** |  $\bigtriangleup$  |  `\bigtriangleup`  |
| **11** |     $\star$      |     `\star`      | **30** | $\bigtriangledown$ | `\bigtriangledown` |
| **12** |      $\ast$      |      `\ast`      | **31** |       $\lhd$       |       `\lhd`       |
| **13** |      $\cup$      |      `\cup`      | **32** |       $\rhd$       |       `\rhd`       |
| **14** |      $\cap$      |      `\cap`      | **33** |      $\unlhd$      |      `\unlhd`      |
| **15** |     $\sqcup$     |     `\sqcup`     | **34** |      $\unrhd$      |      `\unrhd`      |
| **16** |     $\sqcap$     |     `\sqcap`     | **35** |      $\amalg$      |      `\amalg`      |
| **17** |      $\vee$      |      `\vee`      | **36** |       $\wr$        |       `\wr`        |
| **18** |     $\wedge$     |     `\wedge`     | **37** |     $\dagger$      |     `\dagger`      |
| **19** |     $\circ$      |     `\circ`      | **38** |     $\ddagger$     |     `\ddagger`     |

## 二元关系符

|  序号   |                  符号                    |                  LaTeX                   |  序号   |     符号     |     LaTeX      |
| :----: | :--------------------------------------: | :--------------------------------------: | :----: | :----------: | :------------: |
| **1**  |                   =                      |                   `=`                    | **49** |    $\gneq$     |    `\gneq`     |
| **2**  |                  $\ne$                   |                  `\ne`                   | **50** |    $\geqq$     |    `\geqq`     |
| **3**  |                  $\neq$                  |                  `\neq`                  | **51** |    $\ngeq$     |    `\ngeq`     |
| **4**  |                 $\equiv$                 |                 `\equiv`                 | **52** |    $\ngeqq$    |    `\ngeqq`    |
| **5**  |               $\not\equiv$               |               `\not\equiv`               | **53** |    $\gneqq$    |    `\gneqq`    |
| **6**  |                 $\doteq$                 |                 `\doteq`                 | **54** |  $\gvertneqq$  |  `\gvertneqq`  |
| **7**  |               $\doteqdot$                |               `\doteqdot`                | **55** |   $\lessgtr$   |   `\lessgtr`   |
| **8**  | $\overset{\underset{\mathrm{def}}{}}{=}$ | `\overset{\underset{\mathrm{def}}{}}{=}` | **56** |  $\lesseqgtr$  |  `\lesseqgtr`  |
| **9**  |                   :=                     |                   `:=`                   | **57** | $\lesseqqgtr$  | `\lesseqqgtr`  |
| **10** |                  $\sim$                  |                  `\sim`                  | **58** |   $\gtrless$   |   `\gtrless`   |
| **11** |                 $\nsim$                  |                 `\nsim`                  | **59** |  $\gtreqless$  |  `\gtreqless`  |
| **12** |                $\backsim$                |                `\backsim`                | **60** | $\gtreqqless$  | `\gtreqqless`  |
| **13** |               $\thicksim$                |               `\thicksim`                | **61** |  $\leqslant$   |  `\leqslant`   |
| **14** |                 $\simeq$                 |                 `\simeq`                 | **62** |  $\nleqslant$  |  `\nleqslant`  |
| **15** |               $\backsimeq$               |               `\backsimeq`               | **63** | $\eqslantless$ | `\eqslantless` |
| **16** |                 $\eqsim$                 |                 `\eqsim`                 | **64** |  $\geqslant$   |  `\geqslant`   |
| **17** |                 $\cong$                  |                 `\cong`                  | **65** |  $\ngeqslant$  |  `\ngeqslant`  |
| **18** |                 $\ncong$                 |                 `\ncong`                 | **66** | $\eqslantgtr$  | `\eqslantgtr`  |
| **19** |                $\approx$                 |                `\approx`                 | **67** |   $\lesssim$   |   `\lesssim`   |
| **20** |              $\thickapprox$              |              `\thickapprox`              | **68** |    $\lnsim$    |    `\lnsim`    |
| **21** |               $\approxeq$                |               `\approxeq`                | **69** | $\lessapprox$  | `\lessapprox`  |
| **22** |                 $\asymp$                 |                 `\asymp`                 | **70** |  $\lnapprox$   |  `\lnapprox`   |
| **23** |                $\propto$                 |                `\propto`                 | **71** |   $\gtrsim$    |   `\gtrsim`    |
| **24** |               $\varpropto$               |               `\varpropto`               | **72** |    $\gnsim$    |    `\gnsim`    |
| **25** |                   <                      |                   `<`                    | **73** |  $\gtrapprox$  |  `\gtrapprox`  |
| **26** |                 $\nless$                 |                 `\nless`                 | **74** |  $\gnapprox$   |  `\gnapprox`   |
| **27** |                  $\ll$                   |                  `\ll`                   | **75** |    $\prec$     |    `\prec`     |
| **28** |                $\not\ll$                 |                `\not\ll`                 | **76** |    $\nprec$    |    `\nprec`    |
| **29** |                  $\lll$                  |                  `\lll`                  | **77** |   $\preceq$    |   `\preceq`    |
| **30** |                $\not\lll$                |                `\not\lll`                | **78** |   $\npreceq$   |   `\npreceq`   |
| **31** |                $\lessdot$                |                `\lessdot`                | **79** |  $\precneqq$   |  `\precneqq`   |
| **32** |                   >                      |                   `>`                    | **80** |    $\succ$     |    `\succ`     |
| **33** |                 $\ngtr$                  |                 `\ngtr`                  | **81** |    $\nsucc$    |    `\nsucc`    |
| **34** |                  $\gg$                   |                  `\gg`                   | **82** |   $\succeq$    |   `\succeq`    |
| **35** |                $\not\gg$                 |                `\not$\gg`                | **83** |   $\nsucceq$   |   `\nsucceq`   |
| **36** |                  $\ggg$                  |                  `\ggg`                  | **84** |  $\succneqq$   |  `\succneqq`   |
| **37** |                $\not\ggg$                |                `\not$\ggg`               | **85** | $\preccurlyeq$ | `\preccurlyeq` |
| **38** |                $\gtrdot$                 |                `\gtrdot`                 | **86** | $\curlyeqprec$ | `\curlyeqprec` |
| **39** |                  $\le$                   |                  `\le`                   | **87** | $\succcurlyeq$ | `\succcurlyeq` |
| **40** |                  $\leq$                  |                  `\leq`                  | **88** | $\curlyeqsucc$ | `\curlyeqsucc` |
| **41** |                 $\lneq$                  |                 `\lneq`                  | **89** |   $\precsim$   |   `\precsim`   |
| **42** |                 $\leqq$                  |                 `\leqq`                  | **90** |  $\precnsim$   |  `\precnsim`   |
| **43** |                 $\nleq$                  |                 `\nleq`                  | **91** | $\precapprox$  | `\precapprox`  |
| **44** |                 $\nleqq$                 |                 `\nleqq`                 | **92** | $\precnapprox$ | `\precnapprox` |
| **45** |                 $\lneqq$                 |                 `\lneqq`                 | **93** |   $\succsim$   |   `\succsim`   |
| **46** |               $\lvertneqq$               |               `\lvertneqq`               | **94** |  $\succnsim$   |  `\succnsim`   |
| **47** |                  $\ge$                   |                  `\ge`                   | **95** | $\succapprox$  | `\succapprox`  |
| **48** |                  $\geq$                  |                  `\geq`                  | **96** | $\succnapprox$ | `\succnapprox` |

## 几何符号

|  序号  |         符号         |        LaTeX        |   序号  |          符号          |         LaTeX         |
| :----: | :-----------------: | :-----------------: | :----: | :-------------------: | :-------------------: |
| **1**  |     $\parallel$     |     `\parallel`     | **14** |      $\lozenge$       |      `\lozenge`       |
| **2**  |    $\nparallel$     |    `\nparallel`     | **15** |    $\blacklozenge$    |    `\blacklozenge`    |
| **3**  |  $\shortparallel$   |  `\shortparallel`   | **16** |      $\bigstar$       |      `\bigstar`       |
| **4**  |  $\nshortparallel$  |  `\nshortparallel`  | **17** |      $\bigcirc$       |      `\bigcirc`       |
| **5**  |       $\perp$       |       `\perp`       | **18** |      $\triangle$      |      `\triangle`      |
| **6**  |      $\angle$       |      `\angle`       | **19** |   $\bigtriangleup$    |   `\bigtriangleup`    |
| **7**  |  $\sphericalangle$  |  `\sphericalangle`  | **20** |  $\bigtriangledown$   |  `\bigtriangledown`   |
| **8**  |  $\measuredangle$   |  `\measuredangle`   | **21** |    $\vartriangle$     |    `\vartriangle`     |
| **9**  |     45^$\circ$      |     `45^\circ`      | **22** |    $\triangledown$    |    `\triangledown`    |
| **10** |       $\Box$        |       `\Box`        | **23** |   $\blacktriangle$    |   `\blacktriangle`    |
| **11** |   $\blacksquare$    |   `\blacksquare`    | **24** | $\blacktriangledown$  | `\blacktriangledown`  |
| **12** |     $\diamond$      |     `\diamond`      | **25** | $\blacktriangleleft$  | `\blacktriangleleft`  |
| **13** | $\Diamond \lozenge$ | `\Diamond \lozenge` | **26** | $\blacktriangleright$ | `\blacktriangleright` |

## 逻辑符号

|  序号  |       符号      |      LaTeX       |  序号  |         符号          |         LaTeX          |
| :----: | :--------------: | :--------------: | :----: | :--------------------: | :--------------------: |
| **1**  |    $\forall$     |    `\forall`     | **20** |         $\neg$         |         `\neg`         |
| **2**  |    $\exists$     |    `\exists`     | **21** | $\not\operatorname{R}$ | `\not\operatorname{R}` |
| **3**  |    $\nexists$    |    `\nexists`    | **22** |         $\bot$         |         `\bot`         |
| **4**  |   $\therefore$   |   `\therefore`   | **23** |         $\top$         |         `\top`         |
| **5**  |    $\because$    |    `\because`    | **24** |        $\vdash$        |        `\vdash`        |
| **6**  |      $\And$      |      `\And`      | **25** |        $\dashv$        |        `\dashv`        |
| **7**  |      $\lor$      |      `\lor`      | **26** |        $\vDash$        |        `\vDash`        |
| **8**  |      $\vee$      |      `\vee`      | **27** |        $\Vdash$        |        `\Vdash`        |
| **9**  |   $\curlyvee$    |   `\curlyvee`    | **28** |       $\models$        |       `\models`        |
| **10** |    $\bigvee$     |    `\bigvee`     | **29** |       $\Vvdash$        |       `\Vvdash`        |
| **11** |     $\land$      |     `\land`      | **30** |       $\nvdash$        |       `\nvdash`        |
| **12** |     $\wedge$     |     `\wedge`     | **31** |       $\nVdash$        |       `\nVdash`        |
| **13** |  $\curlywedge$   |  `\curlywedge`   | **32** |       $\nvDash$        |       `\nvDash`        |
| **14** |   $\bigwedge$    |   `\bigwedge`    | **33** |       $\nVDash$        |       `\nVDash`        |
| **15** |    $\bar{q}$     |    `\bar{q}`     | **34** |      $\ulcorner$       |      `\ulcorner`       |
| **16** |   $\bar{abc}$    |   `\bar{abc}`    | **35** |      $\urcorner$       |      `\urcorner`       |
| **17** |  $\overline{q}$  |  `\overline{q}`  | **36** |      $\llcorner$       |      `\llcorner`       |
| **18** | $\overline{abc}$ | `\overline{abc}` | **37** |      $\lrcorner$       |      `\lrcorner`       |
| **19** |     $\lnot$      |     `\lnot`      |        |                        |                        |

## 集合符号

|  序号  |      符号      |      LaTeX       |  序号  |      符号      |      LaTeX       |
| :----: | :------------: | :--------------: | :----: | :------------: | :--------------: |
| **1**  |     $\{$ $\}$    |     `\{ \}`      | **23** |   $\sqsubset$    |   `\sqsubset`    |
| **2**  |   $\emptyset$    |   `\emptyset`    | **24** |    $\supset$     |    `\supset`     |
| **3**  |  $\varnothing$   |  `\varnothing`   | **25** |    $\Supset$     |    `\Supset`     |
| **4**  |      $\in$       |      `\in`       | **26** |   $\sqsupset$    |   `\sqsupset`    |
| **5**  |     $\notin$     |     `\notin`     | **27** |   $\subseteq$    |   `\subseteq`    |
| **6**  |      $\ni$       |      `\ni`       | **28** |   $\nsubseteq$   |   `\nsubseteq`   |
| **7**  |      $\cap$      |      `\cap`      | **29** |   $\subsetneq$   |   `\subsetneq`   |
| **8**  |      $\Cap$      |      `\Cap`      | **30** | $\varsubsetneq$  | `\varsubsetneq`  |
| **9**  |     $\sqcap$     |     `\sqcap`     | **31** |  $\sqsubseteq$   |  `\sqsubseteq`   |
| **10** |    $\bigcap$     |    `\bigcap`     | **32** |   $\supseteq$    |   `\supseteq`    |
| **11** |      $\cup$      |      `\cup`      | **33** |   $\nsupseteq$   |   `\nsupseteq`   |
| **12** |      $\Cup$      |      `\Cup`      | **34** |   $\supsetneq$   |   `\supsetneq`   |
| **13** |     $\sqcup$     |     `\sqcup`     | **35** | $\varsupsetneq$  | `\varsupsetneq`  |
| **14** |    $\bigcup$     |    `\bigcup`     | **36** |  $\sqsupseteq$   |  `\sqsupseteq`   |
| **15** |   $\bigsqcup$    |   `\bigsqcup`    | **37** |   $\subseteqq$   |   `\subseteqq`   |
| **16** |     $\uplus$     |     `\uplus`     | **38** |  $\nsubseteqq$   |  `\nsubseteqq`   |
| **17** |   $\biguplus$    |   `\biguplus`    | **39** |  $\subsetneqq$   |  `\subsetneqq`   |
| **18** |   $\setminus$    |   `\setminus`    | **40** | $\varsubsetneqq$ | `\varsubsetneqq` |
| **19** | $\smallsetminus$ | `\smallsetminus` | **41** |   $\supseteqq$   |   `\supseteqq`   |
| **20** |     $\times$     |     `\times`     | **42** |  $\nsupseteqq$   |  `\nsupseteqq`   |
| **21** |    $\subset$     |    `\subset`     | **43** |  $\supsetneqq$   |  `\supsetneqq`   |
| **22** |    $\Subset$     |    `\Subset`     | **44** | $\varsupsetneqq$ | `\varsupsetneqq` |

## 箭头符号

|  序号  |        符号         |         LaTeX         |  序号  |         符号         |         LaTeX          |
| :----: | :-----------------: | :-------------------: | :----: | :------------------: | :--------------------: |
| **1**  |    $\Rrightarrow$     |    `\Rrightarrow`     | **36** |     $\longmapsto$      |     `\longmapsto`      |
| **2**  |     $\Lleftarrow$     |     `\Lleftarrow`     | **37** |   $\rightharpoonup$    |   `\rightharpoonup`    |
| **3**  |     $\Rightarrow$     |     `\Rightarrow`     | **38** |  $\rightharpoondown$   |  `\rightharpoondown`   |
| **4**  |    $\nRightarrow$     |    `\nRightarrow`     | **39** |    $\leftharpoonup$    |    `\leftharpoonup`    |
| **5**  |   $\Longrightarrow$   |   `\Longrightarrow`   | **40** |   $\leftharpoondown$   |   `\leftharpoondown`   |
| **6**  |      $\implies$       |      `\implies`       | **41** |    $\upharpoonleft$    |    `\upharpoonleft`    |
| **7**  |     $\Leftarrow$      |     `\Leftarrow`      | **42** |   $\upharpoonright$    |   `\upharpoonright`    |
| **8**  |     $\nLeftarrow$     |     `\nLeftarrow`     | **43** |   $\downharpoonleft$   |   `\downharpoonleft`   |
| **9**  |   $\Longleftarrow$    |   `\Longleftarrow`    | **44** |  $\downharpoonright$   |  `\downharpoonright`   |
| **10** |   $\Leftrightarrow$   |   `\Leftrightarrow`   | **45** |  $\rightleftharpoons$  |  `\rightleftharpoons`  |
| **11** |  $\nLeftrightarrow$   |  `\nLeftrightarrow`   | **46** |  $\leftrightharpoons$  |  `\leftrightharpoons`  |
| **12** | $\Longleftrightarrow$ | `\Longleftrightarrow` | **47** |   $\curvearrowleft$    |   `\curvearrowleft`    |
| **13** |        $\iff$         |        `\iff`         | **48** |   $\circlearrowleft$   |   `\circlearrowleft`   |
| **14** |      $\Uparrow$       |      `\Uparrow`       | **49** |         $\Lsh$         |         `\Lsh`         |
| **15** |     $\Downarrow$      |     `\Downarrow`      | **50** |     $\upuparrows$      |     `\upuparrows`      |
| **16** |    $\Updownarrow$     |    `\Updownarrow`     | **51** |  $\rightrightarrows$   |  `\rightrightarrows`   |
| **17** |     $\rightarrow$     |     `\rightarrow`     | **52** |   $\rightleftarrows$   |   `\rightleftarrows`   |
| **18** |         $\to$         |         `\to`         | **53** |   $\rightarrowtail$    |   `\rightarrowtail`    |
| **19** |    $\nrightarrow$     |    `\nrightarrow`     | **54** |   $\looparrowright$    |   `\looparrowright`    |
| **20** |   $\longrightarrow$   |   `\longrightarrow`   | **55** |   $\curvearrowright$   |   `\curvearrowright`   |
| **21** |     $\leftarrow$      |     `\leftarrow`      | **56** |  $\circlearrowright$   |  `\circlearrowright`   |
| **22** |        $\gets$        |        `\gets`        | **57** |         $\Rsh$         |         `\Rsh`         |
| **23** |     $\nleftarrow$     |     `\nleftarrow`     | **58** |   $\downdownarrows$    |   `\downdownarrows`    |
| **24** |   $\longleftarrow$    |   `\longleftarrow`    | **59** |   $\leftleftarrows$    |   `\leftleftarrows`    |
| **25** |   $\leftrightarrow$   |   `\leftrightarrow`   | **60** |   $\leftrightarrows$   |   `\leftrightarrows`   |
| **26** |  $\nleftrightarrow$   |  `\nleftrightarrow`   | **61** |    $\leftarrowtail$    |    `\leftarrowtail`    |
| **27** | $\longleftrightarrow$ | `\longleftrightarrow` | **62** |    $\looparrowleft$    |    `\looparrowleft`    |
| **28** |      $\uparrow$       |      `\uparrow`       | **63** |   $\hookrightarrow$    |   `\hookrightarrow`    |
| **29** |     $\downarrow$      |     `\downarrow`      | **64** |    $\hookleftarrow$    |    `\hookleftarrow`    |
| **30** |    $\updownarrow$     |    `\updownarrow`     | **65** |      $\multimap$       |      `\multimap`       |
| **31** |      $\nearrow$       |      `\nearrow`       | **66** | $\leftrightsquigarrow$ | `\leftrightsquigarrow` |
| **32** |      $\swarrow$       |      `\swarrow`       | **67** |   $\rightsquigarrow$   |   `\rightsquigarrow`   |
| **33** |      $\nwarrow$       |      `\nwarrow`       | **68** |  $\twoheadrightarrow$  |  `\twoheadrightarrow`  |
| **34** |      $\searrow$       |      `\searrow`       | **69** |  $\twoheadleftarrow$   |  `\twoheadleftarrow`   |
| **35** |       $\mapsto$       |       `\mapsto`       |        |                      |                        |

## 特殊符号

|  序号  |      符号      |      LaTeX       |  序号  |       符号        |        LaTeX        |
| :----: | :------------: | :--------------: | :----: | :---------------: | :-----------------: |
| **1**  |     $\infty$     |     `\infty`     | **33** |       $\flat$       |       `\flat`       |
| **2**  |     $\aleph$     |     `\aleph`     | **34** |     $\natural$      |     `\natural`      |
| **3**  |  $\complement$   |  `\complement`   | **35** |      $\sharp$       |      `\sharp`       |
| **4**  |  $\backepsilon$  |  `\backepsilon`  | **36** |      $\diagup$      |      `\diagup`      |
| **5**  |      $\eth$      |      `\eth`      | **37** |     $\diagdown$     |     `\diagdown`     |
| **6**  |     $\Finv$      |     `\Finv`      | **38** |    $\centerdot$     |    `\centerdot`     |
| **7**  |     $\hbar$      |     `\hbar`      | **39** |      $\ltimes$      |      `\ltimes`      |
| **8**  |      $\Im$       |      `\Im`       | **40** |      $\rtimes$      |      `\rtimes`      |
| **9**  |     $\imath$     |     `\imath`     | **41** |  $\leftthreetimes$  |  `\leftthreetimes`  |
| **10** |     $\jmath$     |     `\jmath`     | **42** | $\rightthreetimes$  | `\rightthreetimes`  |
| **11** |     $\Bbbk$      |     `\Bbbk`      | **43** |      $\eqcirc$      |      `\eqcirc`      |
| **12** |      $\ell$      |      `\ell`      | **44** |      $\circeq$      |      `\circeq`      |
| **13** |      $\mho$      |      `\mho`      | **45** |    $\triangleq$     |    `\triangleq`     |
| **14** |      $\wp$       |      `\wp`       | **46** |      $\bumpeq$      |      `\bumpeq`      |
| **15** |      $\Re$       |      `\Re`       | **47** |      $\Bumpeq$      |      `\Bumpeq`      |
| **16** |   $\circledS$    |   `\circledS`    | **48** |     $\doteqdot$     |     `\doteqdot`     |
| **17** |     $\amalg$     |     `\amalg`     | **49** |   $\risingdotseq$   |   `\risingdotseq`   |
| **18** |       $\%$       |       `\%`       | **50** |  $\fallingdotseq$   |  `\fallingdotseq`   |
| **19** |    $\dagger$     |    `\dagger`     | **51** |     $\intercal$     |     `\intercal`     |
| **20** |    $\ddagger$    |    `\ddagger`    | **52** |     $\barwedge$     |     `\barwedge`     |
| **21** |     $\ldots$     |     `\ldots`     | **53** |      $\veebar$      |      `\veebar`      |
| **22** |     $\cdots$     |     `\cdots`     | **54** |  $\doublebarwedge$  |  `\doublebarwedge`  |
| **23** |     $\smile$     |     `\smile`     | **55** |     $\between$      |     `\between`      |
| **24** |     $\frown$     |     `\frown`     | **56** |    $\pitchfork$     |    `\pitchfork`     |
| **25** |      $\wr$       |      `\wr`       | **57** | $\vartriangleleft$  | `\vartriangleleft`  |
| **26** | $\triangleleft$  | `\triangleleft`  | **58** |  $\ntriangleleft$   |  `\ntriangleleft`   |
| **27** | $\triangleright$ | `\triangleright` | **59** | $\vartriangleright$ | `\vartriangleright` |
| **28** |  $\diamondsuit$  |  `\diamondsuit`  | **60** |  $\ntriangleright$  |  `\ntriangleright`  |
| **29** |   $\heartsuit$   |   `\heartsuit`   | **61** |  $\trianglelefteq$  |  `\trianglelefteq`  |
| **30** |   $\clubsuit$    |   `\clubsuit`    | **62** | $\ntrianglelefteq$  | `\ntrianglelefteq`  |
| **31** |   $\spadesuit$   |   `\spadesuit`   | **63** | $\trianglerighteq$  | `\trianglerighteq`  |
| **32** |     $\Game$      |     `\Game`      | **64** | $\ntrianglerighteq$ | `\ntrianglerighteq` |

## 分数

|  类型  |                      符号                       |                           LaTeX                             |
| :--- | :---------------------------------------------- | :----------------------------------------------------------- |
| **分数**              | $\frac{2}{4}x=0.5x or {2\over4}x=0.5x$   | `\frac{2}{4}x=0.5x or {2\over4}x=0.5x`              |
| **小型分数**          | $\tfrac{2}{4}x = 0.5x$                   | `\tfrac{2}{4}x = 0.5x`                               |
| **大型分数** （不嵌套） | $\dfrac{2}{4} = 0.5 \qquad \dfrac{2}{c + \dfrac{2}{d + \dfrac{2}{4}}} = a$ | `\dfrac{2}{4} = 0.5 \qquad$ \dfrac{2}{c + \dfrac{2}{d + \dfrac{2}{4}}} = a` |
| **大型分数** （嵌套）   | $\cfrac{2}{c + \cfrac{2}{d + \cfrac{2}{4}}} = a$           | `\cfrac{2}{c + $\cfrac{2}{d + \cfrac{2}{4}}} = a`             |
