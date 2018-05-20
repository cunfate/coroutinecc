## Coroutinecc 协程库

- 基于*nix的`<ucontext.h>`实现
- 参考云风的[coroutine](https://github.com/cloudwu/coroutine)实现
- 支持linux、unix、Mac OS X

### 构建 && 测试
```s
$ g++ coroutine.cc test.cc -std=c++11 -o test
$ ./test
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