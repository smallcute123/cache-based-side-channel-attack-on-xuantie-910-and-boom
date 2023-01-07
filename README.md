# Cache-based Side-channel Attack on Xuantie-910 and Boom

The work based on this code has been published in Cache Design Effect on Microarchitecture Security: A Contrast between Xuantie-910 and BOOM，

## Getting started

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

5.启动qemu，在qemu共享文件夹中找到可执行文件运行

`./flushCacheTest.riscv `