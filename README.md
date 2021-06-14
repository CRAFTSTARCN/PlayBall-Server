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
|id|发送者id，特殊的，初次向服务器索取ID时置为-1，<br>若发向客户端，则为接收用户的id|
|version|协议版本，数字|
|protocolType|上层协议类型，无特殊字符的字符串|  

head内容中出现的空格、Tab、换行将被直接忽略，而<>将被视为标签开始与结束符（产生错误）

## 通信建立流程

TCP连接建立后，应客户端应立即向服务器发送一个确认服务报文，格式如下
```
<msg>
    <head>
        <id>-1</id>
        <version>{service_version}</version>
        <protocolType>certification</protocolType>
    </head>
</msg>
```
这是一个确认服务报文，-1是为了索取此客户在服务端的id，上层协议位certification，version为服务版本  
客户端可能接收到两种回复  
**第一种，服务生效**
```
<msg>
    <head>
        <id>{id}</id>
        <version>{service_version}</version>
        <protocolType>ACK</protocolType>
    </head>
</msg>
```
id字段中的内容即为用户的id，后面发送报文时id字段应设置为此
**第二种，版本错误**
```
<msg>
    <head>
        <id>-1</id>
        <version>{right_service_version}</version>
        <protocolType>REJ</protocolType>
    </head>
    <content>
        <reason>Wrong_Version</reason>
    </content>
</msg>
```
version字段将回复正确的版本号，id字段将为-1，此时客户端应切换正确版本进行通信

## 通用服务报文
除了上面的服务生效，版本错误等，还会产生下面的通用服务报文
```
/*略去开始和结束标签*/
<head>
    <id>{id}</id>
    <version>{version}</version>
    <protocolType>ACK</protocolType>
</head>
<content>
    <joinserver>valid<joinserver>
</content>
```
此报文用于告知用户，其已经成为合法（valid）用户，如果服务器带有合法用户验证（如密码等），此报文应使用在用户确认密码之后，而没有密码，则服务端应在用户认证成功后立即发送

```
<head>
    <id>{id}</id>
    <version>{version}</version>
    <protocolType>REJ</protocolType>
</head>
<content>
    <reason>Wrong_Id</reason>
</content>
```
在用户成功确认服务后，若发送来的报文出现id错误情况，应回复此拒绝报文

```
<head>
    <id>{id}</id>
    <version>{version}</version>
    <protocolType>REJ</protocolType>
</head>
<content>
    <reason>Parse_Error</reason>
</content>
```
在解析报文内容（content）时出错应回复，但位可选  
如果在上层发现报文内容有误，应发送一个该协议的报文回复给，并在content内解析

**以上报文内容均由服务器发送**

## GTP(Game Transport Protocol) 报文

仅描述在content内的内容
### 创建游戏
```
<option>CREATE</option>
```
创建成功后，客户端会收到一个回复，成功/失败
```
<option>CREATE</option>
<success>{gameid/-1}</success>
```
```success```标签中>=0为成功，返回游戏房间id，-1为失败

### 加入游戏
```
<option>JOIN</option>
<room>{room_id}</room>
```
发送后有两个结果，成功/失败
```
<option>JOIN</option>
<room>{room_id/-1}</room>
```
```room```标签中>=0表示成功，-1失败  
同时向所有同房间用户（除它自己）有发送
```
<option>UJOIN<option>
<playerid>{id}<playerid>
```

### 退出游戏

```
<option>QUIT</option>
```
发送后应等待，知道收到应答
```
<option>QUIT</option>
```
这表示，你已经不属于任何房间了  
同时向同房间用户发送
```
<option>UQUIT<option>
<playerid>{id}<playerid>
```

### 更新关键帧
```
<option>UPDATE</option>
<NoParse>
posx1:1;;;
posxy:2;;;
speedx1:1;;;
speedy1:1;;;
what:BULLET/PLAYER;;;
hp1:1;;;
playerid1:{id};;;
</NoParse>
.....
```
由房间创建者发送向所有普通用户
由于只是转发，所以NoParse里的东西时纯文本（各种意义上）故不会被解析

### 用户操作
```
<option>OPERATE<option>
<NoParse>
id:1;;;
movex:114;;;
movey:514;;;
shootx:191;;
shooty:981;;;
</NoParse>
```

由普通用户发送，转发至房主

### 房主退出
房主退出，就地解散