# 这是一个通信协议抽象层，

<div id="vditor-id-0" class="vditor-reset">

# 详细通信协议规定[<svg viewBox="0 0 16 16" version="1.1" width="16" height="16"><path fill-rule="evenodd" d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg>]


## 修订历史[<svg viewBox="0 0 16 16" version="1.1" width="16" height="16"><path fill-rule="evenodd" d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg>](#修订历史)

<table>
<thead>
<tr>
<th>日期</th>
<th>版本</th>
<th>更新内容</th>
</tr>
</thead>
<tbody>
<tr>
<td>2020/6/30</td>
<td>0.0.1</td>
<td>-</td>
</tr>
</tbody>
</table>

## 指令格式详解[<svg viewBox="0 0 16 16" version="1.1" width="16" height="16"><path fill-rule="evenodd" d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg>](#指令格式详解)

### 指令包格式[<svg viewBox="0 0 16 16" version="1.1" width="16" height="16"><path fill-rule="evenodd" d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg>](#指令包格式)

<table>
<thead>
<tr>
<th>字节数</th>
<th>4bytes</th>
<th>1bytes</th>
<th>4bytes</th>
<th>1bytes</th>
<th>… …</th>
<th>… …</th>
<th>… …</th>
<th>1 bytes</th>
</tr>
</thead>
<tbody>
<tr>
<td>名称</td>
<td>包头</td>
<td>通道地址</td>
<td>包长度</td>
<td>指令</td>
<td>参数1</td>
<td>…</td>
<td>参数2</td>
<td>校验和</td>
</tr>
<tr>
<td>内容</td>
<td>0x59485A53</td>
<td>xxxx</td>
<td>xxxx</td>
<td>xxxx</td>
<td>xxxx</td>
<td>…</td>
<td>xxxx</td>
<td>xxxx</td>
</tr>
</tbody>
</table>

1. 所有多字节的**低字节**在前
2. 包头固定为四字节的**0x59485A53**；
3. 通道地址1到x对应软件上的CH1到CHx，**CH1为0x01**，CH2为0x02；
4. 包长度为从包头到校验的所有数据长度。
5. 指令为相应的功能码。
6. 参数为指令需要参数时加入。
7. 校验为校验和方式——8位。

### 指令详解[<svg viewBox="0 0 16 16" version="1.1" width="16" height="16"><path fill-rule="evenodd" d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg>](#指令详解)

#### 指令汇总[<svg viewBox="0 0 16 16" version="1.1" width="16" height="16"><path fill-rule="evenodd" d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg>](#指令汇总)

<table>
<thead>
<tr>
<th></th>
<th>**下位机——&amp;gt;上位机：**</th>
<th></th>
</tr>
</thead>
<tbody>
<tr>
<td>指令</td>
<td>参数</td>
<td>功能</td>
</tr>
<tr>
<td>0x01</td>
<td>1个，目标值，int类型</td>
<td>设置上位机通道的目标值</td>
</tr>
<tr>
<td>0x02</td>
<td>1个，实际值，int类型</td>
<td>设置上位机通道实际值</td>
</tr>
<tr>
<td>0x03</td>
<td>3个，P、I、D，float类型</td>
<td>设置上位机PID值</td>
</tr>
<tr>
<td>0x04</td>
<td>无</td>
<td>设置上位机启动指令（同步上位机的按钮状态）</td>
</tr>
<tr>
<td>0x05</td>
<td>无</td>
<td>设置上位机停止指令（同步上位机的按钮状态）</td>
</tr>
<tr>
<td>0x06</td>
<td>1个，目标值，unsigned int类型</td>
<td>设置上位机周期</td>
</tr>
</tbody>
</table>
<table>
<thead>
<tr>
<th></th>
<th>**上位机——&amp;gt;下位机：**</th>
<th></th>
</tr>
</thead>
<tbody>
<tr>
<td>指令</td>
<td>参数</td>
<td>功能</td>
</tr>
<tr>
<td>0x10</td>
<td>3个，P、I、D，float类型</td>
<td>设置下位机的PID值</td>
</tr>
<tr>
<td>0x11</td>
<td>1个，目标值，int类型</td>
<td>设置下位机的目标值</td>
</tr>
<tr>
<td>0x12</td>
<td>无</td>
<td>启动指令</td>
</tr>
<tr>
<td>0x13</td>
<td>无</td>
<td>停止指令</td>
</tr>
<tr>
<td>0x14</td>
<td>无</td>
<td>复位指令</td>
</tr>
<tr>
<td>0x15</td>
<td>1个，目标值，unsigned int类型</td>
<td>设置下位机周期</td>
</tr>
</tbody>
</table>

#### 设置上位机通道的目标值[<svg viewBox="0 0 16 16" version="1.1" width="16" height="16"><path fill-rule="evenodd" d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg>](#设置上位机通道的目标值)

* 功能说明：设置上位机对应通道的目标值。
* 输入参数：目标值，int类型的数据。

指令包格式：

<table>
<thead>
<tr>
<th>字节数</th>
<th>4bytes</th>
<th>1bytes</th>
<th>4bytes</th>
<th>1bytes</th>
<th>4bytes</th>
<th>1 bytes</th>
</tr>
</thead>
<tbody>
<tr>
<td>名称</td>
<td>包头</td>
<td>通道地址</td>
<td>包长度</td>
<td>指令</td>
<td>目标值</td>
<td>校验和</td>
</tr>
<tr>
<td>内容</td>
<td>　0x59485A53</td>
<td>xxxx</td>
<td>0x0F</td>
<td>0x01</td>
<td>　xxxx</td>
<td>　xxxx</td>
</tr>
</tbody>
</table>

目标值：

<pre><div class="vditor-copy"><textarea></textarea><span aria-label="复制" onmouseover="this.setAttribute('aria-label', '复制')" class="vditor-tooltipped vditor-tooltipped__w" onclick="this.previousElementSibling.select();document.execCommand('copy');this.setAttribute('aria-label', '已复制')"><svg><use xlink:href="#vditor-icon-copy"></use></svg></span></div>`设置上位机的目标值（<span class="hljs-keyword">int</span>类型数据）

`</pre>

#### 发送通道的实际值[<svg viewBox="0 0 16 16" version="1.1" width="16" height="16"><path fill-rule="evenodd" d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg>](#发送通道的实际值)

* 功能说明：发送实际值给上位机。
* 输入参数：实际值，int类型的数据。

指令包格式：

<table>
<thead>
<tr>
<th>字节数</th>
<th>4bytes</th>
<th>1bytes</th>
<th>4bytes</th>
<th>1bytes</th>
<th>4 bytes</th>
<th>1 bytes</th>
</tr>
</thead>
<tbody>
<tr>
<td>名称</td>
<td>包头</td>
<td>通道地址</td>
<td>包长度</td>
<td>指令</td>
<td>实际值</td>
<td>校验和</td>
</tr>
<tr>
<td>内容</td>
<td>0x59485A53</td>
<td>xxxx</td>
<td>0x0F</td>
<td>0x02</td>
<td>xxxx</td>
<td>xxxx</td>
</tr>
</tbody>
</table>

实际值：

<pre><div class="vditor-copy"><textarea></textarea><span aria-label="复制" onmouseover="this.setAttribute('aria-label', '复制')" class="vditor-tooltipped vditor-tooltipped__w" onclick="this.previousElementSibling.select();document.execCommand('copy');this.setAttribute('aria-label', '已复制')"><svg><use xlink:href="#vditor-icon-copy"></use></svg></span></div>`设置上位机的实际值（<span class="hljs-keyword">int</span>类型数据）

`</pre>

#### 发送PID参数给上位机[<svg viewBox="0 0 16 16" version="1.1" width="16" height="16"><path fill-rule="evenodd" d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg>](#发送PID参数给上位机)

功能说明：下位机发送P、I、D参数给上位机。

输入参数：P、I、D参数，三个float类型的数据。

<table>
<thead>
<tr>
<th>字节数</th>
<th>4bytes</th>
<th>1bytes</th>
<th>4bytes</th>
<th>1bytes</th>
<th>4bytes</th>
<th>4bytes</th>
<th>4bytes</th>
<th>1 bytes</th>
</tr>
</thead>
<tbody>
<tr>
<td>名称</td>
<td>包头</td>
<td>通道地址</td>
<td>包长度</td>
<td>指令</td>
<td>P参数</td>
<td>I参数</td>
<td>D参数</td>
<td>校验和</td>
</tr>
<tr>
<td>内容</td>
<td>0x59485A53　</td>
<td>xxxx</td>
<td>0x17　</td>
<td>0x03</td>
<td>xxxx</td>
<td>xxxx</td>
<td>xxxx</td>
<td>xxxx</td>
</tr>
</tbody>
</table>

**P** 参数:

<pre><div class="vditor-copy"><textarea></textarea><span aria-label="复制" onmouseover="this.setAttribute('aria-label', '复制')" class="vditor-tooltipped vditor-tooltipped__w" onclick="this.previousElementSibling.select();document.execCommand('copy');this.setAttribute('aria-label', '已复制')"><svg><use xlink:href="#vditor-icon-copy"></use></svg></span></div>`下位机要发送的<span class="hljs-selector-tag">P</span>参数（<span class="hljs-attribute">float</span>类型数据）

`</pre>

**I** 参数:

<pre><div class="vditor-copy"><textarea></textarea><span aria-label="复制" onmouseover="this.setAttribute('aria-label', '复制')" class="vditor-tooltipped vditor-tooltipped__w" onclick="this.previousElementSibling.select();document.execCommand('copy');this.setAttribute('aria-label', '已复制')"><svg><use xlink:href="#vditor-icon-copy"></use></svg></span></div>`下位机要发送的<span class="hljs-selector-tag">I</span>参数（<span class="hljs-attribute">float</span>类型数据）

`</pre>

**D** 参数:

<pre><div class="vditor-copy"><textarea></textarea><span aria-label="复制" onmouseover="this.setAttribute('aria-label', '复制')" class="vditor-tooltipped vditor-tooltipped__w" onclick="this.previousElementSibling.select();document.execCommand('copy');this.setAttribute('aria-label', '已复制')"><svg><use xlink:href="#vditor-icon-copy"></use></svg></span></div>`下位机要发送的D参数（<span class="hljs-attribute">float</span>类型数据）

`</pre>

#### 下位机同步启动按钮指令[<svg viewBox="0 0 16 16" version="1.1" width="16" height="16"><path fill-rule="evenodd" d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg>](#下位机同步启动按钮指令)

* 功能说明：下位机发送同步启动按钮状态指令给上位机。
* 输入参数：无。

指令包格式：

<table>
<thead>
<tr>
<th>字节数</th>
<th>4bytes</th>
<th>1bytes</th>
<th>4bytes</th>
<th>1bytes</th>
<th>1 bytes</th>
</tr>
</thead>
<tbody>
<tr>
<td>名称</td>
<td>包头</td>
<td>通道地址</td>
<td>包长度</td>
<td>指令</td>
<td>校验和</td>
</tr>
<tr>
<td>内容</td>
<td>0x59485A53</td>
<td>xxxx</td>
<td>0x0000000F</td>
<td>0x04</td>
<td>xxxx</td>
</tr>
</tbody>
</table>

#### 下位机同步停止按钮指令[<svg viewBox="0 0 16 16" version="1.1" width="16" height="16"><path fill-rule="evenodd" d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg>](#下位机同步停止按钮指令)

* 功能说明：下位机发送同步停止按钮状态指令给上位机。
* 输入参数：无。

指令包格式：

<table>
<thead>
<tr>
<th>字节数</th>
<th>4bytes</th>
<th>1bytes</th>
<th>4bytes</th>
<th>1bytes</th>
<th>1 bytes</th>
</tr>
</thead>
<tbody>
<tr>
<td>名称</td>
<td>包头</td>
<td>通道地址</td>
<td>包长度</td>
<td>指令</td>
<td>校验和</td>
</tr>
<tr>
<td>内容</td>
<td>0x59485A53</td>
<td>xxxx</td>
<td>0x0000000F</td>
<td>0x05</td>
<td>xxxx</td>
</tr>
</tbody>
</table>

#### 下位机发送周期值[<svg viewBox="0 0 16 16" version="1.1" width="16" height="16"><path fill-rule="evenodd" d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg>](#下位机发送周期值)

* 功能说明：下位机发送周期给上位机。
* 输入参数：周期，正整数类型的数据。

指令包格式：

<table>
<thead>
<tr>
<th>字节数</th>
<th>4bytes</th>
<th>1bytes</th>
<th>4bytes</th>
<th>1bytes</th>
<th>4 bytes</th>
<th>1 bytes</th>
</tr>
</thead>
<tbody>
<tr>
<td>名称</td>
<td>包头</td>
<td>通道地址</td>
<td>包长度</td>
<td>指令</td>
<td>周期</td>
<td>校验和</td>
</tr>
<tr>
<td>内容</td>
<td>0x59485A53</td>
<td>xxxx</td>
<td>0x0F</td>
<td>0x06</td>
<td>xxxx</td>
<td>xxxx</td>
</tr>
</tbody>
</table>

周期:

<pre><div class="vditor-copy"><textarea></textarea><span aria-label="复制" onmouseover="this.setAttribute('aria-label', '复制')" class="vditor-tooltipped vditor-tooltipped__w" onclick="this.previousElementSibling.select();document.execCommand('copy');this.setAttribute('aria-label', '已复制')"><svg><use xlink:href="#vditor-icon-copy"></use></svg></span></div>`下位机需要设置的周期值（正整数类型数据：<span class="hljs-keyword">unsigned</span> <span class="hljs-keyword">int</span>）

`</pre>

#### 上位机发送PID参数[<svg viewBox="0 0 16 16" version="1.1" width="16" height="16"><path fill-rule="evenodd" d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg>](#上位机发送PID参数)

* 功能说明：上位机发送P、I、D参数给下位机。
* 输入参数：P、I、D参数，三个float类型的数据。

<table>
<thead>
<tr>
<th>字节数</th>
<th>4bytes</th>
<th>1bytes</th>
<th>4bytes</th>
<th>1bytes</th>
<th>4bytes</th>
<th>4bytes</th>
<th>4bytes</th>
<th>1 bytes</th>
</tr>
</thead>
<tbody>
<tr>
<td>名称</td>
<td>包头</td>
<td>通道地址</td>
<td>包长度</td>
<td>指令</td>
<td>P参数</td>
<td>I参数</td>
<td>D参数</td>
<td>校验和</td>
</tr>
<tr>
<td>内容</td>
<td>0x59485A53　</td>
<td>xxxx</td>
<td>0x17　</td>
<td>0x10</td>
<td>xxxx</td>
<td>xxxx</td>
<td>xxxx</td>
<td>xxxx</td>
</tr>
</tbody>
</table>

**P** 参数:

<pre><div class="vditor-copy"><textarea></textarea><span aria-label="复制" onmouseover="this.setAttribute('aria-label', '复制')" class="vditor-tooltipped vditor-tooltipped__w" onclick="this.previousElementSibling.select();document.execCommand('copy');this.setAttribute('aria-label', '已复制')"><svg><use xlink:href="#vditor-icon-copy"></use></svg></span></div>`上位机要发送的<span class="hljs-selector-tag">P</span>参数（<span class="hljs-attribute">float</span>类型数据）

`</pre>

**I** 参数:

<pre><div class="vditor-copy"><textarea></textarea><span aria-label="复制" onmouseover="this.setAttribute('aria-label', '复制')" class="vditor-tooltipped vditor-tooltipped__w" onclick="this.previousElementSibling.select();document.execCommand('copy');this.setAttribute('aria-label', '已复制')"><svg><use xlink:href="#vditor-icon-copy"></use></svg></span></div>`上位机要发送的<span class="hljs-selector-tag">I</span>参数（<span class="hljs-attribute">float</span>类型数据）

`</pre>

**D** 参数:

<pre><div class="vditor-copy"><textarea></textarea><span aria-label="复制" onmouseover="this.setAttribute('aria-label', '复制')" class="vditor-tooltipped vditor-tooltipped__w" onclick="this.previousElementSibling.select();document.execCommand('copy');this.setAttribute('aria-label', '已复制')"><svg><use xlink:href="#vditor-icon-copy"></use></svg></span></div>`上位机要发送的D参数（<span class="hljs-attribute">float</span>类型数据）

`</pre>

#### 上位机发送目标值[<svg viewBox="0 0 16 16" version="1.1" width="16" height="16"><path fill-rule="evenodd" d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg>](#上位机发送目标值)

* 功能说明：上位机发送目标值给下位机。
* 输入参数：目标值，int类型的数据。

指令包格式：

<table>
<thead>
<tr>
<th>字节数</th>
<th>4bytes</th>
<th>1bytes</th>
<th>4bytes</th>
<th>1bytes</th>
<th>4 bytes</th>
<th>1 bytes</th>
</tr>
</thead>
<tbody>
<tr>
<td>名称</td>
<td>包头</td>
<td>通道地址</td>
<td>包长度</td>
<td>指令</td>
<td>目标值</td>
<td>校验和</td>
</tr>
<tr>
<td>内容</td>
<td>0x59485A53</td>
<td>xxxx</td>
<td>0x0F</td>
<td>0x11</td>
<td>xxxx</td>
<td>xxxx</td>
</tr>
</tbody>
</table>

目标值：

<pre><div class="vditor-copy"><textarea></textarea><span aria-label="复制" onmouseover="this.setAttribute('aria-label', '复制')" class="vditor-tooltipped vditor-tooltipped__w" onclick="this.previousElementSibling.select();document.execCommand('copy');this.setAttribute('aria-label', '已复制')"><svg><use xlink:href="#vditor-icon-copy"></use></svg></span></div>`上位机发送给下位机的目标值（<span class="hljs-keyword">int</span>类型数据）

`</pre>

#### 上位机发送启动指令[<svg viewBox="0 0 16 16" version="1.1" width="16" height="16"><path fill-rule="evenodd" d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg>](#上位机发送启动指令)

* 功能说明：上位机发送启动指令给下位机。
* 输入参数：无。

指令包格式：

<table>
<thead>
<tr>
<th>字节数</th>
<th>4bytes</th>
<th>1bytes</th>
<th>4bytes</th>
<th>1bytes</th>
<th>1 bytes</th>
</tr>
</thead>
<tbody>
<tr>
<td>名称</td>
<td>包头</td>
<td>通道地址</td>
<td>包长度</td>
<td>指令</td>
<td>校验和</td>
</tr>
<tr>
<td>内容</td>
<td>0x59485A53</td>
<td>xxxx</td>
<td>0x0000000F</td>
<td>0x12</td>
<td>xxxx</td>
</tr>
</tbody>
</table>

#### 上位机发送停止指令[<svg viewBox="0 0 16 16" version="1.1" width="16" height="16"><path fill-rule="evenodd" d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg>](#上位机发送停止指令)

* 功能说明：上位机发送停止指令给下位机。
* 输入参数：无。

指令包格式：

<table>
<thead>
<tr>
<th>字节数</th>
<th>4bytes</th>
<th>1bytes</th>
<th>4bytes</th>
<th>1bytes</th>
<th>1 bytes</th>
</tr>
</thead>
<tbody>
<tr>
<td>名称</td>
<td>包头</td>
<td>通道地址</td>
<td>包长度</td>
<td>指令</td>
<td>校验和</td>
</tr>
<tr>
<td>内容</td>
<td>0x59485A53</td>
<td>xxxx</td>
<td>0x0000000F</td>
<td>0x13</td>
<td>xxxx</td>
</tr>
</tbody>
</table>

#### 上位机发送复位指令[<svg viewBox="0 0 16 16" version="1.1" width="16" height="16"><path fill-rule="evenodd" d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg>](#上位机发送复位指令)

* 功能说明：上位机发送停止指令给下位机。
* 输入参数：无。

指令包格式：

<table>
<thead>
<tr>
<th>字节数</th>
<th>4bytes</th>
<th>1bytes</th>
<th>4bytes</th>
<th>1bytes</th>
<th>1 bytes</th>
</tr>
</thead>
<tbody>
<tr>
<td>名称</td>
<td>包头</td>
<td>通道地址</td>
<td>包长度</td>
<td>指令</td>
<td>校验和</td>
</tr>
<tr>
<td>内容</td>
<td>0x59485A53</td>
<td>xxxx</td>
<td>0x0000000F</td>
<td>0x14</td>
<td>xxxx</td>
</tr>
</tbody>
</table>

#### 上位机发送周期[<svg viewBox="0 0 16 16" version="1.1" width="16" height="16"><path fill-rule="evenodd" d="M4 9h1v1H4c-1.5 0-3-1.69-3-3.5S2.55 3 4 3h4c1.45 0 3 1.69 3 3.5 0 1.41-.91 2.72-2 3.25V8.59c.58-.45 1-1.27 1-2.09C10 5.22 8.98 4 8 4H4c-.98 0-2 1.22-2 2.5S3 9 4 9zm9-3h-1v1h1c1 0 2 1.22 2 2.5S13.98 12 13 12H9c-.98 0-2-1.22-2-2.5 0-.83.42-1.64 1-2.09V6.25c-1.09.53-2 1.84-2 3.25C6 11.31 7.55 13 9 13h4c1.45 0 3-1.69 3-3.5S14.5 6 13 6z"></path></svg>](#上位机发送周期)

* 功能说明：上位机发送周期给下位机。
* 输入参数：周期，正整数类型的数据。

指令包格式：

<table>
<thead>
<tr>
<th>字节数</th>
<th>4bytes</th>
<th>1bytes</th>
<th>4bytes</th>
<th>1bytes</th>
<th>4 bytes</th>
<th>1 bytes</th>
</tr>
</thead>
<tbody>
<tr>
<td>名称</td>
<td>包头</td>
<td>通道地址</td>
<td>包长度</td>
<td>指令</td>
<td>周期</td>
<td>校验和</td>
</tr>
<tr>
<td>内容</td>
<td>0x59485A53</td>
<td>xxxx</td>
<td>0x0F</td>
<td>0x15</td>
<td>xxxx</td>
<td>xxxx</td>
</tr>
</tbody>
</table>

</div>

