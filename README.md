## Coroutinecc 协程库[![Build Status](https://travis-ci.org/cunfate/coroutinecc.svg?branch=master)](https://travis-ci.org/cunfate/coroutinecc)

- 基于*nix的`<ucontext.h>`实现
- 参考云风的[coroutine](https://github.com/cloudwu/coroutine)实现
- 支持linux、unix、Mac OS X
- 协程库实现相关文章可以参考[从ucontext到coroutine](https://www.jianshu.com/p/a96b31da3ab0)

## 未来改进：

1. 提供对等协程实现
2. hook进一些systemcall，将所有fd设为非阻塞，并在epoll中切换协程，实现用户侧阻塞IO，实现侧非阻塞效率。

### 构建 && 测试
```s
$ make
$ bin/test
```
控制台应当打印：
```s
main start
coroutine 0 : 0
coroutine 1 : 200
coroutine 0 : 1
coroutine 1 : 201
coroutine 0 : 2
coroutine 1 : 202
coroutine 0 : 3
coroutine 1 : 203
coroutine 0 : 4
coroutine 1 : 204
main end
```
简单的性能测试可以：
```s
$ bin/test-perform
```
在Ubuntu 16.04LTS, i7-5500U 3GB(VMWARE虚拟机)测试结果为：
```s
switch times test start!
switch times test end!
It can switch 879039.736991 times
```
