# PlayBall-Server
PlayBall game server side, by cpp , based on SDL_net socket

## 报文协议格式
采用非常简单的类xml协议格式基本组成为:
```
<{label}>{content}<{/label}>
```
标签内容以<>标志，结束标签以/开始，其它均与开始标签相同，所有标签必须关闭  
其中{label}为**开始标签区域** {/label}为**结束标签区域**{content}为**内容区域**

### 报文结构
报文以```msg```标签开始，由```head``` ```content```两个大部分组成，其中某些报文可以没有content
```
<msg>
    <head>
        ...
    </head>
    <content>
        ...
    </content>
</msg>
```
标签间的所有字符将被视为无效字符
```
...
    <head>
        ...
    </head>
    /*无效字符*/
    <content>
    ...
```
内容区域**允许出现标签**（即标签多层嵌套），但注意，如果一层存在子层标签，**则此层不允许直接出现内容**，如：
```
...
    <label>
        content here invalid
        <son1>hei</son1>
        content here invalid too
        <son2>www</son2>
    </label>
...
```
其中```<label>```标签内未被嵌套的两行将不被作为内容

**同级标签不允许重复**，否则按照最后一次出现作为内容

### 标签名称规约
标签名称（<>内内容）有严格的格式规定  
仅可以出现**大小写英文字母，下划线，数字**，关闭标签以/作为内容开始。  
注意，下面的特殊字符同样禁止出现在标签内

### 特殊内容字符
所有的```< > 空格 回车 制表符（Tab）均被视为特殊字符```  
其中```< >```直接出现在内容区域中将被直接视作标签的开始结束，而空格回车和制表符将被忽略
```
    ...
    
    <exp1>here<</exp>
    /*error! the < will recognized as begin of a end label*/
    <exp2>a enter below
    haha<exp2>
    /*space and enter will be ignore, content is aenterbelowhaha*/
    ...
```

如果想在内容中出现特殊字符，则必须使用转义即
|原内容|转义后|
|:-:|:-:|
|<| \\< |
|>| \\> |
|空格| \\b |
|换行| \\n |
|Tab| \\t |
|\\ | \\\\ |

### 头结构
报文头有固定的结构，它的内容不会出现特殊字符，不允许嵌套，内容中不允许出现上述特殊字符（转义也不行）  
结构如下
```
...
<head>
    <id>1</id>
    <version>1</version>
    <protocolType> GameXml </protocolType>
</head>
```
|名称|内容|
|:-:|:-:|
|id|发送者id|
|version|协议版本，数字|
|protocolType|上层协议类型，无特殊字符的字符串|  

head内容中出现的空格、Tab、换行将被直接忽略，而<>将被视为标签开始与结束符（产生错误）