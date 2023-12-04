# 这是一个通信协议抽象层

<div id="vditor-id-0" class="vditor-reset">
# 详细通信协议规定




## 修订历史

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
<td>1.0.0</td>
<td>-</td>
</tr>
</tbody>
</table>

## 指令格式详解

### 指令包格式

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

1. 所有多字节的**低字节**在前（**LSB**先行）
2. 包头固定为四字节的**0x59485A53**；
3. 通道地址1到x对应软件上的CH1到CHx，可以理解为对应被操作的子设备
4. 包长度为从包头到校验的所有数据长度。
5. 指令为相应的功能码。
6. 参数为指令需要参数时加入。
7. 校验为校验和方式——8位（**CRC8**）。

### 指令详解

#### 指令汇总

<table>
<thead>
<tr>
<th></th>
<th>下位机——>上位机</th>
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
<td>无</td>
<td>设置上位机启动指令（同步上位机的按钮状态）</td>
</tr>
<tr>
<td>0x04</td>
<td>无</td>
<td>设置上位机停止指令（同步上位机的按钮状态）</td>
</tr>
<tr>
<td>0x05</td>
<td>1个，目标值，unsigned int类型</td>
<td>设置上位机速度或周期</td>
</tr>
</tbody>
</table>
<table>
<thead>
<tr>
<th></th>
<th>上位机——>下位机</th>
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
<td>设置下位机速度或周期</td>
</tr>
</tbody>
</table>


#### 设置上位机通道的目标值

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
#### 发送通道的实际值

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
#### 下位机同步启动按钮指令

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
<td>0x0F</td>
<td>0x03</td>
<td>xxxx</td>
</tr>
</tbody>
</table>


#### 下位机同步停止按钮指令

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
<td>0x0F</td>
<td>0x04</td>
<td>xxxx</td>
</tr>
</tbody>
</table>


#### 下位机发送周期值

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
<td>0x05</td>
<td>xxxx</td>
<td>xxxx</td>
</tr>
</tbody>
</table>

#### 上位机发送目标值

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
#### 上位机发送启动指令

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
<td>0x0F</td>
<td>0x12</td>
<td>xxxx</td>
</tr>
</tbody>
</table>


#### 上位机发送停止指令

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
<td>0x0F</td>
<td>0x13</td>
<td>xxxx</td>
</tr>
</tbody>
</table>


#### 上位机发送复位指令

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
<td>0x0F</td>
<td>0x14</td>
<td>xxxx</td>
</tr>
</tbody>
</table>


#### 上位机发送周期

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
