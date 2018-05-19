## Coroutinecc 协程库

- 基于*nix的`<ucontext.h>`实现
- 参考云风的[coroutine](https://github.com/cloudwu/coroutine)实现

### 构建 && 测试
```s
$ g++ coroutine.cc test.cc -std=c++11 -o test
$ ./test
```