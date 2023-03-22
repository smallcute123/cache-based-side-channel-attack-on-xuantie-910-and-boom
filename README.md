# Cache-based Side-channel Attack on Xuantie-910 and Boom

The work based on this code has been published in Cache Design Effect on Microarchitecture Security: A Contrast between Xuantie-910 and BOOM，
## 介绍
本项目首先分析和比较了Xuantie-910和BOOMv3中不同的缓存设计，以及这些差异对实现需要通过准确刷新缓存来构建基于缓存的侧通道的Spectre攻击的影响。Xuantie-910和BOOM都是基于RISC-V的处理器。缓存有两种类型，物理类型的缓存索引物理标记（PIPT）缓存（玄铁910采用）和虚拟索引物理标记（VIPT）缓存（BOOM采用），分别对应两种不同的缓存寻址形式。 VIPT 比 PIPT 具有更高的寻址性能，因为它可以直接从虚拟地址中获取缓存行索引。因此BOOMv3 VIPT的缓存设计允许刷新缓存的精确位置，这是成功进行基于缓存的侧信道攻击的重要一步，相比之下，Xuantie-910 PIPT的缓存设计不能实现缓存的精确位置刷新，因此也会一定程度防御基于缓存的信道攻击。我们通过软件模拟和 FPGA 原型验证对 BOOMv3 和Xuantie-910 实施基于缓存的侧信道攻击来证明我们的分析，并为提高基于 RISC-V 的微体系结构的安全性提供重要的缓存设计见解。
实现对缓存的精确位置冲刷流程如下：
<img src="image/Process of accurate flushing function.png" style="zoom:40%;"/>

以下内容仅针对Xuantie-910架构，对于BOOMv3架构的的攻击详细内容请见https://gitee.com/chip-security/spectre_-risc-v#a-study-of-mcu-level-attacks-and-defenses-on-power-distribution-fusion-terminals
## 安装教程&运行方法

建立一个工作目录以下所有操作都在该工作目录中运行

1.下载C910 Qemu

打开xuantie发行版页面： https://github.com/c-sky/buildroot/releases C910对应的版本是： `c9xx linux-5.10` 参考网站readme.txt quick start for qemu部分；

2.添加共享文件夹

a.主机中创建共享文件夹

mkdir /home/chengxiaoyu/share

b.启动时添加如下两行参数

path为主机中共享文件夹的位置

```markdown
-fsdev local,security_model=passthrough,id=fsdev0,path=/home/chengxiaoyu/share -device virtio-9p-device,id=fs0,fsdev=fsdev0,mount_tag=hostshare
```

c.在qemu中创建文件夹

```
mkdir /tmp/share
```

3.挂载

```
mount -t 9p -o trans=virtio,version=9p2000.L hostshare /tmp/share/
```

4.将攻击文件xuantie-attack放入share文件夹中，进入攻击文件路径，运行makefile,注意makefile文件中路径的修改

`make`

在bin文件夹中生成了riscv可执行文件

5.启动qemu，在qemu共享文件夹中找到可执行文件运行，示例：

`./flushCacheTest.riscv `

## 使用说明

1.运行bin文件下的flushCacheTest.riscv文件验证Xuantie-910上的完整缓存冲刷功能。
  详细代码可见flushCache.h和flushCacheTest.c
2.运行dCacheTest.riscv文件验证Xuantie-910上的精准缓存冲刷功能。
  详细代码可见flushCache.h和flushCacheTest.c
3.运行condBranchMispred.riscv文件验证利用精准冲刷的SpectreV1在Xuantie-910上不能攻击成功。

## 预期结果
-在Xuantie-910上的完整缓存冲刷的结果
<img src="image/The result of entire cache flushing function on Xuantie-910.png" style="zoom:50%;"/>
-在Xuantie-910上的精确缓存冲刷的结果
<img src="image/The result of accurate cache flushing function on Xuantie-910.png" style="zoom:50%;"/>
-在Xuantie-910上的进行SpectreV1攻击的结果


